/**
 * @file Issues.hpp ERS issues used by the toolbox functions and classes
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_
#define TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_

#include <ers/Issue.hpp>

#include <string>

namespace dunedaq {

// Disable coverage collection LCOV_EXCL_START

ERS_DECLARE_ISSUE(toolbox,
                  ServiceNotFound,
                  "The service " << service << " was not found in DNS",
                  ((std::string)service))
ERS_DECLARE_ISSUE(toolbox,
                  NameNotFound,
                  "The hostname " << name << " could not be resolved: " << error,
                  ((std::string)name)((std::string)error))
// Reenable coverage collection LCOV_EXCL_STOP
} // namespace dunedaq

#endif // TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_
