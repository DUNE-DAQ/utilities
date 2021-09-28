/**
 *
 * @file FileSourceBuffer_test.cxx Utility functions Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "toolbox/FileSourceBuffer.hpp"
#include "toolbox/Issues.hpp"

#define BOOST_TEST_MODULE FileSourceBuffer_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>

using namespace dunedaq::toolbox;

BOOST_AUTO_TEST_CASE(Constructors)
{
  auto fsb = new FileSourceBuffer(16, 8);
  delete fsb;
}

BOOST_AUTO_TEST_CASE(CopyAndMoveSemantics)
{
  BOOST_REQUIRE(!std::is_copy_constructible_v<FileSourceBuffer>);
  BOOST_REQUIRE(!std::is_copy_assignable_v<FileSourceBuffer>);
  BOOST_REQUIRE(!std::is_move_constructible_v<FileSourceBuffer>);
  BOOST_REQUIRE(!std::is_move_assignable_v<FileSourceBuffer>);
}

BOOST_AUTO_TEST_CASE(SimpleFile)
{
  std::ofstream of("test.bin", std::ios::trunc | std::ios::binary);
  uint64_t first = 0x1111222233334444;
  uint64_t second = 0xFFFFDDDDCCCCBBBB;
  of.write(reinterpret_cast<char*>(&first), sizeof(first));
  of.write(reinterpret_cast<char*>(&second), sizeof(second));
  of.close();

  FileSourceBuffer buffer(16, 8);
  buffer.read("test.bin");

  BOOST_REQUIRE_EQUAL(buffer.num_elements(), 2);
  auto bytes = buffer.get();

  BOOST_REQUIRE_EQUAL(bytes.size(), 16);
  BOOST_REQUIRE_EQUAL(bytes[0], 0x44);
  BOOST_REQUIRE_EQUAL(*reinterpret_cast<uint64_t*>(bytes.data()), 0x1111222233334444);

  remove("test.bin");
}

BOOST_AUTO_TEST_CASE(NoChunkSize)
{
  std::ofstream of("test.bin", std::ios::trunc | std::ios::binary);
  uint64_t first = 0x1111222233334444;
  uint64_t second = 0xFFFFDDDDCCCCBBBB;
  of.write(reinterpret_cast<char*>(&first), sizeof(first));
  of.write(reinterpret_cast<char*>(&second), sizeof(second));
  of.close();

  FileSourceBuffer buffer(16);
  buffer.read("test.bin");

  BOOST_REQUIRE_EQUAL(buffer.num_elements(), 0);
  auto bytes = buffer.get();

  BOOST_REQUIRE_EQUAL(bytes.size(), 16);
  BOOST_REQUIRE_EQUAL(bytes[0], 0x44);
  BOOST_REQUIRE_EQUAL(*reinterpret_cast<uint64_t*>(bytes.data()), 0x1111222233334444);

  remove("test.bin");
}

BOOST_AUTO_TEST_CASE(PartialRecord)
{
  std::ofstream of("test.bin", std::ios::trunc | std::ios::binary);
  uint64_t first = 0x1111222233334444;
  uint64_t second = 0xFFFFDDDDCCCCBBBB;
  uint32_t third = 0x77778888;
  of.write(reinterpret_cast<char*>(&first), sizeof(first));
  of.write(reinterpret_cast<char*>(&second), sizeof(second));
  of.write(reinterpret_cast<char*>(&third), sizeof(third));
  of.close();

  FileSourceBuffer buffer(20, 8);
  buffer.read("test.bin");

  BOOST_REQUIRE_EQUAL(buffer.num_elements(), 2);
  auto bytes = buffer.get();

  BOOST_REQUIRE_EQUAL(bytes.size(), 20);
  BOOST_REQUIRE_EQUAL(bytes[0], 0x44);
  BOOST_REQUIRE_EQUAL(*reinterpret_cast<uint64_t*>(bytes.data()), 0x1111222233334444);

  remove("test.bin");
}

BOOST_AUTO_TEST_CASE(BadFiles)
{
  FileSourceBuffer buffer(8, 8);
  BOOST_REQUIRE_EXCEPTION(buffer.read("test.bin"), CannotOpenFile, [&](CannotOpenFile const&) {
    return true;
  });

  std::ofstream of("test.bin", std::ios::trunc | std::ios::binary);
  uint64_t first = 0x1111222233334444;
  uint64_t second = 0xFFFFDDDDCCCCBBBB;
  of.write(reinterpret_cast<char*>(&first), sizeof(first));
  of.write(reinterpret_cast<char*>(&second), sizeof(second));
  of.close();

  buffer.read("test.bin");
  auto bytes = buffer.get();
  BOOST_REQUIRE_EQUAL(bytes.size(), 16);

  remove("test.bin");
}
