/**
 *
 * @file ZmqUri.cpp ZmqUri struct method implementations
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/ZmqUri.hpp"
#include "utilities/get_ips.hpp"
#include "utilities/Issues.hpp"

#include <string>
#include <vector>


// ZMQ URIs are formatted as follows: tcp://{host}:{port}
dunedaq::utilities::ZmqUri::ZmqUri(std::string connection_string)
{
  if (connection_string.find("://") == std::string::npos) {
    throw InvalidUri(ERS_HERE, connection_string);
  }

  scheme = connection_string.substr(0, connection_string.find("://"));
  connection_string = connection_string.substr(connection_string.find("://") + 3);

  if (connection_string.find(":") != std::string::npos) {
    port = connection_string.substr(connection_string.find(":") + 1);
    connection_string = connection_string.substr(0, connection_string.find(":"));
  }
  host = connection_string;
}

std::vector<std::string>
dunedaq::utilities::ZmqUri::get_uri_ip_addresses()
{
  if (scheme == "tcp") {
    auto output = get_hostname_ips(host);

    for (size_t ii = 0; ii < output.size(); ++ii) {
      output[ii] = "tcp://" + output[ii] + ":" + port;
    }
    return output;
  } else {
    return { to_string() };
  }
}
