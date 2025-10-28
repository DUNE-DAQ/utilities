/**
 *
 * @file get_ips_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/Issues.hpp"
#include "utilities/get_ips.hpp"

#include "logging/Logging.hpp"

#define BOOST_TEST_MODULE get_ips_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <regex>

using namespace dunedaq::utilities;

BOOST_AUTO_TEST_CASE(HostnameLookup)
{
  TLOG() << "Test HostnameLookup BEGIN";
  auto res = get_hostname_ips("127.0.0.1");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE_EQUAL(res[0], "127.0.0.1");

  res = get_hostname_ips("localhost");
  BOOST_REQUIRE_GE(res.size(), 1);
  BOOST_REQUIRE(res[0] == "127.0.0.1" || res[0] == "::1");

  res = get_hostname_ips("cern.ch");
  BOOST_REQUIRE_GT(res.size(), 0);

  // Ports are not accepted in input
  res = get_hostname_ips("127.0.0.1:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);

  res = get_hostname_ips("localhost:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);

  // ZMQ URIs are not accepted as input
  res = get_hostname_ips("tcp://localhost:1234");
  BOOST_REQUIRE_EQUAL(res.size(), 0);
  TLOG() << "Test HostnameLookup END";
}

BOOST_AUTO_TEST_CASE(InterfaceLookup)
{
  std::string loopback_if_name;
  struct ifaddrs* ifaddr = nullptr;
  getifaddrs(&ifaddr);

  for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_name[0] == 'l') {
      loopback_if_name = std::string(ifa->ifa_name);
      break;
    }
  }

  freeifaddrs(ifaddr);
    
  auto res = get_interface_ip(loopback_if_name);
  BOOST_REQUIRE_EQUAL(res, "127.0.0.1");

  BOOST_REQUIRE_EXCEPTION(get_interface_ip("thisifdoesntexist", true), InterfaceNotFound, [](InterfaceNotFound const&){return true;});
  res = get_interface_ip("thisifdoesntexist");
  BOOST_REQUIRE(res.size() > 0);
}
