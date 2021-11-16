/**
 *
 * @file UtilityFunctions_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "toolbox/Issues.hpp"
#include "toolbox/UtilityFunctions.hpp"

#define BOOST_TEST_MODULE UtilityFunctions_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <vector>

using namespace dunedaq;

BOOST_AUTO_TEST_CASE(to_string_t)
{
  BOOST_REQUIRE_EQUAL(to_string(5), "5");
  unsigned test2 = 10;
  BOOST_REQUIRE_EQUAL(to_string(test2), "10");
}

enum TestEnum : int
{
  Value1 = 1,
  Value3 = 3,
  Value4 = 4,
  NotFound,
  ERROR
};

BOOST_AUTO_TEST_CASE(safe_enum_cast_t)
{
  std::vector<TestEnum> valid{ Value1, Value3, Value4 };
  safe_enum_cast(4, valid, NotFound);
}

BOOST_AUTO_TEST_CASE(millisleep_t)
{
  auto begin = std::chrono::steady_clock::now();
  millisleep(500);
  auto end = std::chrono::steady_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

  BOOST_REQUIRE_CLOSE(static_cast<double>(diff), 500.0, 0.002);
}

BOOST_AUTO_TEST_CASE(strprintf_t) // NOLINT
{
  auto res = strprintf("Printf test  %d %s", 5, "test"); // NOLINT
  BOOST_REQUIRE_EQUAL(res, "Printf test  5 test");
}

BOOST_AUTO_TEST_CASE(shell_expand_paths_t)
{
  auto paths = shell_expand_paths("one $PATH three");
  BOOST_REQUIRE_EQUAL(paths[0], "one");
  BOOST_REQUIRE_EQUAL(paths[2], "three");

  BOOST_REQUIRE_EXCEPTION(
    shell_expand_paths("$(exec something)"), std::runtime_error, [&](std::runtime_error const&) { return true; });
}

BOOST_AUTO_TEST_CASE(shellExpandPath_t)
{
  auto path = shellExpandPath("$HOME");

  BOOST_REQUIRE_EXCEPTION(shellExpandPath("a b"), std::runtime_error, [&](std::runtime_error const&) { return true; });
}

BOOST_AUTO_TEST_CASE(throw_if_not_file_t)
{
  auto temp_dir = std::filesystem::temp_directory_path();
  std::ofstream(temp_dir / "test.bin");

  throw_if_not_file((temp_dir / "test.bin").string());

  BOOST_REQUIRE_EXCEPTION(throw_if_not_file((temp_dir / "non_existing_file_name").string()),
                          toolbox::FileNotFound,
                          [&](toolbox::FileNotFound const&) { return true; });
  BOOST_REQUIRE_EXCEPTION(throw_if_not_file(temp_dir.string()),
                          toolbox::FileIsDirectory,
                          [&](toolbox::FileIsDirectory const&) { return true; });

  std::filesystem::remove(temp_dir / "test.bin");
}

BOOST_AUTO_TEST_CASE(join_t)
{
  std::vector<std::string> empty_vec;
  BOOST_REQUIRE_EQUAL(join(empty_vec), "");

  std::vector<std::string> test_vec{ "a", "b", "c" };
  BOOST_REQUIRE_EQUAL(join(test_vec), "a b c");
  BOOST_REQUIRE_EQUAL(join(test_vec, ", "), "a, b, c");

  std::list<std::string> test_list{ "d", "e", "f" };
  BOOST_REQUIRE_EQUAL(join(test_list), "d e f");
  BOOST_REQUIRE_EQUAL(join(test_list, ", "), "d, e, f");
}

BOOST_AUTO_TEST_CASE(get_seconds_since_epoch_t)
{
  auto time1 = get_seconds_since_epoch();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto time2 = get_seconds_since_epoch();

  BOOST_REQUIRE(time2 - time1 == 2);
}

BOOST_AUTO_TEST_CASE(format_timestamp_t)
{
  std::time_t time_input = 2;
  struct tm* time = localtime(&time_input); // NOLINT
  char time_buffer[32];

  strftime(time_buffer, sizeof time_buffer, "%a, %d %b %Y %H:%M:%S +0000", time);
  BOOST_REQUIRE_EQUAL(format_timestamp(1000000, 500000), std::string(time_buffer));
}

BOOST_AUTO_TEST_CASE(vec_fmt_t)
{
  std::vector<int> empty_vec;
  BOOST_REQUIRE_EQUAL(vec_fmt(empty_vec), "[]");

  std::vector<std::string> test_vec{ "a", "b", "c" };
  BOOST_REQUIRE_EQUAL(vec_fmt(test_vec), "[a,b,c]");
  std::vector<unsigned> test_vec2{ 0, 1, 2, 3, 4 };
  BOOST_REQUIRE_EQUAL(vec_fmt(test_vec2), "[0,1,2,3,4]");
}
BOOST_AUTO_TEST_CASE(short_vec_fmt_t)
{
  std::vector<int> empty_vec;
  BOOST_REQUIRE_EQUAL(short_vec_fmt(empty_vec), "[]");

  std::vector<int> single_vec{ 1 };
  BOOST_REQUIRE_EQUAL(short_vec_fmt(single_vec), "[1]");

  std::vector<int> test_vec{ 1, 3, 4, 5, 7, 8, 10, 35 };
  BOOST_REQUIRE_EQUAL(short_vec_fmt(test_vec), "[1,3-5,7-8,10,35]");

  std::vector<unsigned> test_vec2{ 1, 3, 5, 6, 7 };
  BOOST_REQUIRE_EQUAL(short_vec_fmt(test_vec2), "[1,3,5-7]");
}