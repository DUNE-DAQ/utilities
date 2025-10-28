/**
 * @file Issues.hpp ERS issues used by the utilities functions and classes
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_
#define UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_

#include "logging/Logging.hpp" // NOTE: if ISSUES ARE DECLARED BEFORE include logging/Logging.hpp, TLOG_DEBUG<<issue wont work.

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
ERS_DECLARE_ISSUE(utilities,
                  InterfaceNotFound,
                  "The interface " << name << " does not exist or does not have an assigned IP address",
                  ((std::string)name))
ERS_DECLARE_ISSUE(utilities, InvalidUri, "The URI string " << uri << " is not valid", ((std::string)uri))
/**
 * @brief An ERS Issue raised when a threading state error occurs
 */
ERS_DECLARE_ISSUE(utilities, ThreadingIssue, "Threading Issue detected: " << err, ((std::string)err))

ERS_DECLARE_ISSUE(utilities, InvalidTimeSync, "An invalid TimeSync message was received", ERS_EMPTY)

ERS_DECLARE_ISSUE(utilities,
                  EarlyTimeSync,
                  "The most recent TimeSync message is ahead of current system time by " << time_diff << " us.",
                  ((uint64_t)time_diff)) // NOLINT

ERS_DECLARE_ISSUE(utilities,
                  LateTimeSync,
                  "The most recent TimeSync message is behind current system time by " << time_diff << " us.",
                  ((uint64_t)time_diff)) // NOLINT

ERS_DECLARE_ISSUE(utilities,
                  FailedToGetTimestampEstimate,
                  "Failed to get timestamp estimate (was interrupted)",
                  ERS_EMPTY)
// Re-enable coverage collection LCOV_EXCL_STOP
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_ISSUES_HPP_
