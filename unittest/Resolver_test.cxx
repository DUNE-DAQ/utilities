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

BOOST_AUTO_TEST_CASE(HostnameLookup) {
    auto res = get_ips_from_hostname("127.0.0.1");
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE_EQUAL(res[0], "127.0.0.1");

    res = get_ips_from_hostname("127.0.0.1:1234");
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE_EQUAL(res[0], "127.0.0.1:1234");

    res = get_ips_from_hostname("127.0.0.1",1234);
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE_EQUAL(res[0], "127.0.0.1:1234");

    res = get_ips_from_hostname("localhost");
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE(res[0] == "127.0.0.1" || res[0] == "::1");

    res = get_ips_from_hostname("localhost:1234");
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE(res[0] == "127.0.0.1:1234" || res[0] == "::1:1234");

    res = get_ips_from_hostname("localhost", 1234);
    BOOST_REQUIRE_GE(res.size(), 1);
    BOOST_REQUIRE(res[0] == "127.0.0.1:1234" || res[0] == "::1:1234");

    res = get_ips_from_hostname("cern.ch");
    BOOST_REQUIRE_GT(res.size(), 0);

    res = get_ips_from_hostname("tcp://localhost:1234");
    BOOST_REQUIRE_EQUAL(res.size(), 0);
}

BOOST_AUTO_TEST_CASE(UriLookup)
{
  auto res = resolve_uri_hostname("tcp://127.0.0.1:1234");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "tcp://127.0.0.1:1234");

  res = resolve_uri_hostname("tcp://localhost:1234");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "tcp://127.0.0.1:1234" || res[0] == "tcp://::1:1234");

  res = resolve_uri_hostname("inproc://foo");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "inproc://foo");

  BOOST_REQUIRE_EXCEPTION(res = resolve_uri_hostname("blah"), InvalidUri, [](InvalidUri const&) { return true; });
}

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
