/**
 *
 * @file ZmqUri.hpp Utilities for working with ZeroMQ URI strings
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_ZMQURI_HPP_
#define UTILITIES_INCLUDE_UTILITIES_ZMQURI_HPP_

#include "logging/Logging.hpp"
#include "utilities/Issues.hpp"

#include <string>
#include <vector>

namespace dunedaq::utilities {

struct ZmqUri
{
  std::string scheme{ "" };
  std::string endpoint_host{ "" };
  std::string endpoint_port{ "" };
  std::string host{ "" };
  std::string port{ "" };

  std::string to_string();

  explicit ZmqUri(std::string connection_string);

  std::vector<std::string> get_uri_ip_addresses();
};

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_ZMQURI_HPP_
