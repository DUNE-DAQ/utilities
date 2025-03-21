/**
 *
 * @file Resolver.cpp DNS Resolver implementations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/Resolver.hpp"

std::vector<std::string>
dunedaq::utilities::get_ips_from_hostname(std::string hostname)
{
  std::vector<std::string> output;

  TLOG_DEBUG(12) << "Name is " << hostname;

  struct addrinfo* result;
  auto s = getaddrinfo(hostname.c_str(), nullptr, nullptr, &result);

  if (s != 0) {
    ers::error(NameNotFound(ERS_HERE, hostname, std::string(gai_strerror(s))));
    return output;
  }

  for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    // Let's skip all the IPv6 here
    if (rp->ai_family == AF_INET6)
      continue;

    getnameinfo(rp->ai_addr, rp->ai_addrlen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    auto result = std::string(hbuf);
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
dunedaq::utilities::resolve_uri_hostname(std::string connection_string)
{
  auto uri = parse_connection_string(connection_string);

  if(uri.scheme == "tcp") {
  auto output = get_ips_from_hostname(uri.host);

  for (size_t ii = 0; ii < output.size(); ++ii) {
    output[ii] = "tcp://" + output[ii] + ":" + uri.port;
  }
  return output;
  } else {
  return { connection_string }; 
  }
}

dunedaq::utilities::ZmqUri
dunedaq::utilities::parse_connection_string(std::string connection_string)
{
  // ZMQ URIs are formatted as follows: tcp://{host}:{port}
  ZmqUri output;

  if (connection_string.find("://") == std::string::npos) {
    throw InvalidUri(ERS_HERE, connection_string);
  }

  output.scheme = connection_string.substr(0, connection_string.find("://"));
  connection_string = connection_string.substr(connection_string.find("://") + 3);

  if (connection_string.find(":") != std::string::npos) {
    output.port = connection_string.substr(connection_string.find(":") + 1);
    connection_string = connection_string.substr(0, connection_string.find(":"));
  }
  output.host = connection_string;

  return output;
}
