/**
 *
 * @file Resolver.hpp DNS Resolver methods
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_
#define UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_

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

std::vector<std::string>
get_ips_from_hostname(std::string hostname);

std::vector<std::string>
resolve_uri_hostname(std::string uri);

std::vector<std::string>
get_service_addresses(std::string service_name, std::string const& hostname = "");

} // namespace utilities
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_RESOLVER_HPP_
