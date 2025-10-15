/**
 *
 * @file ZmqUri_test.cxx ZmqUri struct unit tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/Issues.hpp"
#include "utilities/ZmqUri.hpp"

#include "logging/Logging.hpp"

#define BOOST_TEST_MODULE ZmqUri_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace dunedaq::utilities;

BOOST_AUTO_TEST_CASE(Construct)
{
  ZmqUri test("inproc://test");
  BOOST_REQUIRE_EXCEPTION(ZmqUri bad("bad"), InvalidUri, [](InvalidUri const&) { return true; });

  BOOST_REQUIRE_EQUAL(test.scheme, "inproc");
  BOOST_REQUIRE_EQUAL(test.host, "test");
  BOOST_REQUIRE_EQUAL(test.port, "");
  BOOST_REQUIRE_EQUAL(test.to_string(), "inproc://test");

  ZmqUri test_tcp("tcp://localhost:1234");
  BOOST_REQUIRE_EQUAL(test_tcp.scheme, "tcp");
  BOOST_REQUIRE_EQUAL(test_tcp.host, "localhost");
  BOOST_REQUIRE_EQUAL(test_tcp.port, "1234");
  BOOST_REQUIRE_EQUAL(test_tcp.to_string(), "tcp://localhost:1234");
}

BOOST_AUTO_TEST_CASE(UriLookup)
{
  TLOG() << "Test UriLookup BEGIN";
  ZmqUri test("tcp://127.0.0.1:1234");
  auto res = test.get_uri_ip_addresses();
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "tcp://127.0.0.1:1234");

  ZmqUri test2("tcp://localhost:1234");
  res = test2.get_uri_ip_addresses();
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "tcp://127.0.0.1:1234" || res[0] == "tcp://::1:1234");

  ZmqUri test3("inproc://foo");
  res = test3.get_uri_ip_addresses();
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "inproc://foo");

  TLOG() << "Test UriLookup END";
}
