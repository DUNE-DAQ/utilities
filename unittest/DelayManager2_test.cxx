/**
 * @file DelayManager2_test.cxx  DelayManager2 class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/DelayManager.hpp"

/**
 * @brief This test module handles the testing of the DelayManager when
 *        there *is* a TRACE_FILE set in the environment.
 */
#define BOOST_TEST_MODULE DelayManager2_test // NOLINT

#include "boost/date_time/posix_time/posix_time.hpp"
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
  // set the TRACE_FILE env var to point to a moderately unique file name
  std::string user_string("unknown");
  auto user_val = std::getenv("USER");
  if (user_val != nullptr) {
    std::string tmp_string(user_val);
    user_string = tmp_string;
  }
  time_t now = time(0);
  std::string timestamp_string = boost::posix_time::to_iso_string(boost::posix_time::from_time_t(now));
  std::string trace_file_path(std::filesystem::temp_directory_path());
  std::string trace_file_name =
    trace_file_path + "/DelayManagerUnitTest_" + user_string + "_" + timestamp_string + ".trace";
  auto retval = setenv("TRACE_FILE", trace_file_name.c_str(), 1);
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

  // configure the DelayManager with a simple map
  std::map<std::string, uint32_t> delay_map;
  delay_map["hello"] = 500000;  // usec
  delay_map["world"] = 1500000; // usec
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

  // set the appropriate TRACE level that globally enables delays
  retval = std::system("trace_cntl -n \"DelayManager.hpp\" lvlset 0x4000000000000 0 0");
  BOOST_CHECK_EQUAL(retval, 0);
  retval = std::system("trace_cntl modeM 1");
  BOOST_CHECK_EQUAL(retval, 0);

  // now, some of the delay requests should run (and return true)
  auto start_time = std::chrono::steady_clock::now();
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("abc", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("null", ""), false);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("hello", ""), true);
  BOOST_CHECK_EQUAL(delay_mgr->maybe_delay("world", ""), true);
  auto end_time = std::chrono::steady_clock::now();

  // verify that the elapsed time is greater than the 1.5 sec that we requested
  // in the two map entries
  auto delta_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
  BOOST_CHECK(delta_time_ms > 1500);

  // delete the temporary TRACE_FILE
  BOOST_CHECK(std::filesystem::remove(trace_file_name));
}

BOOST_AUTO_TEST_SUITE_END()
