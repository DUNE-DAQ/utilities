/**
 * @file DelayManager1_test.cxx  DelayManager1 class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/DelayManager.hpp"

/**
 * @brief This test module handles the testing of the DelayManager when
 *        there is no TRACE_FILE set in the environment.
 */
#define BOOST_TEST_MODULE DelayManager1_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

using namespace dunedaq;

BOOST_AUTO_TEST_SUITE(BOOST_TEST_MODULE)

BOOST_AUTO_TEST_CASE(Basics)
{
  // un-set any existing value for the TRACE_FILE env var
  auto retval = unsetenv("TRACE_FILE");
  BOOST_CHECK_EQUAL(retval, 0);

  // fetch the single instance of the DelayManager
  std::shared_ptr<utilities::DelayManager> delay_mgr = utilities::DelayManager::get();

  // if we haven't configured the DelayManager yet, and we haven't globally-enabled
  // the DelayManager, all queries should return false
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled(""), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("abc"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("null"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("hello"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("world"), false);

  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("abc", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("null", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("hello", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("world", ""), false);

  // let's see what happens if we configure the DelayManager with an empty map
  std::map<std::string, uint32_t> delay_map;
  delay_mgr->configure(delay_map);

  // we expect that all queries should still return false
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled(""), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("abc"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("null"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("hello"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("world"), false);

  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("abc", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("null", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("hello", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("world", ""), false);

  // we'll verify that the DelayManager made a copy of the map by modifying
  // the local copy of the map and confirming that the DelayManager behavior
  // doesn't change
  delay_map["hello"] = 500000;  // usec
  delay_map["world"] = 1500000; // usec
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("hello"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("world"), false);

  // configure the DelayManager with a simple map
  delay_mgr->configure(delay_map);

  // some queries should now return true
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled(""), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("abc"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("null"), false);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("hello"), true);
  BOOST_CHECK_EQUAL(delay_mgr->is_delay_enabled("world"), true);

  // but, requests for executing delays will still not happen
  // because we haven't turned on the global enable
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("abc", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("null", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("hello", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("world", ""), false);
}

BOOST_AUTO_TEST_SUITE_END()
