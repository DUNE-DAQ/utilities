/**
 *
 * @file ReusableThread_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "toolbox/Issues.hpp"
#include "toolbox/ReusableThread.hpp"

#define BOOST_TEST_MODULE ReusableThread_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>

using namespace dunedaq::toolbox;

std::atomic<int> result;
void
test_fun(int input)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  result = input;
}

BOOST_AUTO_TEST_CASE(Constructors)
{
  auto rt = new ReusableThread(1);
  delete rt;
}

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<ReusableThread>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<ReusableThread>);
  BOOST_REQUIRE(!std::is_move_constructible_v<ReusableThread>);
  BOOST_REQUIRE(!std::is_move_assignable_v<ReusableThread>);
}

BOOST_AUTO_TEST_CASE(SetWork)
{
  ReusableThread worker(2);

  result = 0;
  auto res = worker.set_work(test_fun, 5);
  BOOST_REQUIRE_EQUAL(res, true);
  res = worker.set_work(test_fun, 3);
  BOOST_REQUIRE_EQUAL(res, false);

  while (!worker.get_readiness()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  BOOST_REQUIRE_EQUAL(result, 5);
}