/**
 *
 * @file get_artificial_delay_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/get_artificial_delay_value.hpp"

#include "logging/Logging.hpp"

#define BOOST_TEST_MODULE get_artificial_delay_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <cstdlib>
#include <string>
#include <vector>

using namespace dunedaq::utilities;

BOOST_AUTO_TEST_CASE(BasicTests)
{
  std::vector<std::string> key_list = { "AAA", "BBB" };

  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{malformed_json", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"AAA\": 123}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 123);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"BBB\": 987}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"CCC\": 987}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"AAA\": {\"BBB\": 456}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 456);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"AAA\": {\"AAA\": 579}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"BBB\": {\"BBB\": 579}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"AAA\": {\"CCC\": 579}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"CCC\": {\"BBB\": 579}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);

  setenv("DUNEDAQ_ARTIFICIAL_DELAYS", "{\"AAA\": {\"BBB\": {\"CCC\": 579}}}", 1);
  BOOST_REQUIRE_EQUAL(get_artificial_delay_value(key_list), 0);
}
