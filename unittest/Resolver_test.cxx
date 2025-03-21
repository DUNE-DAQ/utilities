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

#include "logging/Logging.hpp"

#define BOOST_TEST_MODULE Resolver_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace dunedaq::utilities;

BOOST_AUTO_TEST_CASE(HostnameLookup)
{
  TLOG() << "Test HostnameLookup BEGIN";
  auto res = get_ips_from_hostname("127.0.0.1");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE_EQUAL(res[0], "127.0.0.1");

  res = get_ips_from_hostname("localhost");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "127.0.0.1" || res[0] == "::1");

  res = get_ips_from_hostname("cern.ch");
  BOOST_REQUIRE_GT(res.size(), 0);

  // Ports are not accepted in input
  res = get_ips_from_hostname("127.0.0.1:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);

  res = get_ips_from_hostname("localhost:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);

  // ZMQ URIs are not accepted as input
  res = get_ips_from_hostname("tcp://localhost:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);
  TLOG() << "Test HostnameLookup END";
}

BOOST_AUTO_TEST_CASE(UriLookup)
{
  TLOG() << "Test UriLookup BEGIN";
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
  TLOG() << "Test UriLookup END";
}

