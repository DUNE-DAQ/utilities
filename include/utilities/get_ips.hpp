/**
 *
 * @file get_ips.hpp Translate a hostname to a list of IP addresses associated with that hostname in DNS, or an
 * interface name to the first bound IP for that interface (via getnameinfo)
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_GET_IPS_HPP_
#define UTILITIES_INCLUDE_UTILITIES_GET_IPS_HPP_

#include "logging/Logging.hpp"
#include "utilities/Issues.hpp"

#include <arpa/nameser.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/types.h>

#include <cerrno> // NOLINT(runtime/output_format
#include <string>
#include <vector>

namespace dunedaq::utilities {

inline std::vector<std::string>
get_hostname_ips(std::string hostname)
{
  std::vector<std::string> output;

  TLOG_DEBUG(12) << "Name is " << hostname;

  struct addrinfo* addrinfo = nullptr;
  auto s = getaddrinfo(hostname.c_str(), nullptr, nullptr, &addrinfo);

  if (s != 0) {
    ers::error(NameNotFound(ERS_HERE, hostname, std::string(gai_strerror(s))));
    return output;
  }

  for (auto rp = addrinfo; rp != nullptr; rp = rp->ai_next) {
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV]; // NOLINT

    // Let's skip all the IPv6 here
    if (rp->ai_family == AF_INET6)
      continue;

    getnameinfo(rp->ai_addr, rp->ai_addrlen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    auto ipaddr = std::string(hbuf);
    bool duplicate = false;
    for (auto& ip : output) {
      if (ip == ipaddr) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      TLOG_DEBUG(13) << "Found address " << ipaddr << " for hostname " << hostname;
      output.push_back(ipaddr);
    }
  }

  freeaddrinfo(addrinfo);

  return output;
}

inline std::string
get_interface_ip(std::string eth_device_name, bool throw_if_missing = false)
{
  std::string ipaddr = "0.0.0.0";
  char hostname[NI_MAXHOST]; // NOLINT This is a char array for interfacing with the C networking API
  if (gethostname(&hostname[0], NI_MAXHOST) == 0) {
    ipaddr = std::string(hostname);
  }
  bool eth_found = false;
  if (eth_device_name != "") {
    // Work out which ip address goes with this device
    struct ifaddrs* ifaddr = nullptr;
    getifaddrs(&ifaddr);

    for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == nullptr || std::string(ifa->ifa_name) != eth_device_name) {
        continue;
      }

      char ip[NI_MAXHOST]; // NOLINT This is a char array for interfacing with the C networking API
      int status = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), ip, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
      if (status != 0) {
        continue;
      }
      eth_found = true;
      ipaddr = std::string(ip);
      break;
    }
    freeifaddrs(ifaddr);
  }
  if (!eth_found) {
    auto err = InterfaceNotFound(ERS_HERE, eth_device_name);
    
    if (throw_if_missing)
        throw err;
    else
        ers::warning(err);
  }

  return ipaddr;
}

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_GET_IPS_HPP_
