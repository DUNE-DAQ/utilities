#ifndef UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_
#define UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_

/**
 *
 * @file Resolver.hpp DNS Resolver methods
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "logging/Logging.hpp"
#include "utilities/Issues.hpp"

#include <arpa/nameser.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/types.h>

#include <string>
#include <vector>

namespace dunedaq {

namespace utilities {

    std::vector<std::string> get_ips_from_hostname(std::string hostname, int port = 0) {

        // ZMQ URIs are formatted as follows: tcp://{name}:{port}
        std::vector<std::string> output;
        std::string name = hostname;
        std::string portstr = "";
        bool appendZmqScheme = false;
        if (port != 0) portstr = std::to_string(port);
        if (hostname.find("tcp://") == 0) {
            appendZmqScheme = true;
            name = hostname.substr(6);
        }
        if (name.find(":") != std::string::npos) {
            portstr = name.substr(name.find(":") + 1);
            name = name.substr(0, name.find(":"));
        }
        TLOG_DEBUG(12) << "Name is " << name << ", port is " << portstr;

        struct addrinfo* result;
        auto s = getaddrinfo(name.c_str(), portstr != "" ? portstr.c_str() : nullptr, nullptr, &result);

        if (s != 0) {
            ers::error(NameNotFound(ERS_HERE, name, std::string(gai_strerror(s))));
            return output;
        }

        for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
            getnameinfo(rp->ai_addr, rp->ai_addrlen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
            auto result = std::string(hbuf);
            auto portresult = std::string(sbuf);
            if (portresult != "" && portresult != "0") {
                result += ":" + portresult;
            }
            if (appendZmqScheme) {
                result = "tcp://" + result;
            }
            bool duplicate = false;
            for (auto& res : output) {
                if (res == result) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                TLOG_DEBUG(13) << "Found address " << result << " for hostname " << hostname;
                output.push_back(result);
            }
        }

        freeaddrinfo(result);

        return output;
    }

std::vector<std::string>
get_service_addresses(std::string service_name, std::string const& hostname = "")
{
  std::vector<std::string> output;
  unsigned char query_buffer[1024];

  // Check if we're given a "bare" service name, convert to DNS service name, assuming TCP
  if (std::count(service_name.begin(), service_name.end(), '.') == 0) {
    service_name = "_" + service_name + "._tcp";

    if (!hostname.empty()) {
      service_name += "." + hostname;
    }
  }

  auto response = res_search(service_name.c_str(), C_IN, ns_t_srv, query_buffer, sizeof(query_buffer));
  if (response < 0) {
    ers::error(ServiceNotFound(ERS_HERE, service_name));
    return output;
  }

  ns_msg nsMsg;
  ns_initparse(query_buffer, response, &nsMsg);

  for (int x = 0; x < ns_msg_count(nsMsg, ns_s_an); x++) {
    ns_rr rr;
    ns_parserr(&nsMsg, ns_s_an, x, &rr);

    char name[1024];
    dn_expand(ns_msg_base(nsMsg), ns_msg_end(nsMsg), ns_rr_rdata(rr) + 6, name, sizeof(name));

    auto port = ntohs(*((unsigned short*)ns_rr_rdata(rr) + 2)); // NOLINT(runtime/int)
    auto ips = get_ips_from_hostname(name, port);
    for (auto& ip : ips) {
        output.push_back(ip);
    }
  }
  return output;
}

} // namespace utilities
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_
