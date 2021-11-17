/**
 *
 * @file Resolver_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/Issues.hpp"
#include "utilities/Resolver.hpp"

#define BOOST_TEST_MODULE Resolver_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace dunedaq::utilities;

BOOST_AUTO_TEST_CASE(NoService)
{
  auto res = get_service_addresses("NonExistantService");
  BOOST_REQUIRE(res.empty());

  res = get_service_addresses("NonExistantService", "NonExistantHost");
  BOOST_REQUIRE(res.empty());
}

BOOST_AUTO_TEST_CASE(Ldap)
{
  auto res = get_service_addresses("_ldap._tcp");
  if (res.empty())
    return;

  BOOST_REQUIRE(res.size() > 0);

  auto res2 = get_service_addresses("ldap");
  BOOST_REQUIRE_EQUAL(res2.size(), res.size());

  std::regex regex("\\d+\\.\\d+\\.\\d+\\.\\d+:\\d+");
  BOOST_REQUIRE(std::regex_match(res[0], regex));
}
