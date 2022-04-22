/**
 * @file Issues.hpp ERS issues used by the utilities functions and classes
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_
#define UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_

#include <ers/Issue.hpp>

#include <string>

namespace dunedaq {

// Disable coverage collection LCOV_EXCL_START

ERS_DECLARE_ISSUE(utilities,
                  ServiceNotFound,
                  "The service " << service << " was not found in DNS",
                  ((std::string)service))
ERS_DECLARE_ISSUE(utilities,
                  NameNotFound,
                  "The hostname " << name << " could not be resolved: " << error,
                  ((std::string)name)((std::string)error))
ERS_DECLARE_ISSUE(utilities, InvalidUri, "The URI string " << uri << " is not valid", ((std::string)uri))
// Reenable coverage collection LCOV_EXCL_STOP
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_
