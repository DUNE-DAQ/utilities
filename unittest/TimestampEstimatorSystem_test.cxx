/**
 * @file TimestampEstimatorSystem_test.cxx  TimestampEstimatorSystem class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/TimestampEstimatorSystem.hpp"

/**
 * @brief Name of this test module
 */
#define BOOST_TEST_MODULE TimestampEstimatorSystem_test // NOLINT

#include "boost/test/unit_test.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <chrono>
#include <future>

using namespace dunedaq;

BOOST_AUTO_TEST_SUITE(BOOST_TEST_MODULE)

BOOST_AUTO_TEST_CASE(Basics)
{
  const uint64_t clock_frequency_hz = 62'500'000; // NOLINT(build/unsigned)
  std::atomic<bool> continue_flag{ true };
  dunedaq::utilities::TimestampEstimatorSystem tes(clock_frequency_hz);

  BOOST_CHECK_EQUAL(tes.wait_for_valid_timestamp(continue_flag), dunedaq::utilities::TimestampEstimatorBase::kFinished);

  std::atomic<bool> do_not_continue_flag{ false };
  BOOST_CHECK_EQUAL(tes.wait_for_valid_timestamp(do_not_continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kFinished);

  auto ts_now = tes.get_timestamp_estimate();
  BOOST_CHECK_EQUAL(tes.wait_for_requested_timestamp(ts_now + clock_frequency_hz, continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kFinished);

  ts_now = tes.get_timestamp_estimate();
  BOOST_CHECK_EQUAL(tes.wait_for_requested_timestamp(ts_now + clock_frequency_hz, do_not_continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kInterrupted);

  // Check that the timestamp doesn't go backwards
  auto ts1 = tes.get_timestamp_estimate();
  auto ts2 = tes.get_timestamp_estimate();
  BOOST_CHECK_GE(ts2, ts1);
}

// 27-May-2025, KAB: this test case is intended to verify that an instance of
// the TimestampEstimatorSystem behaves as expected when it first starts up.
BOOST_AUTO_TEST_CASE(StartupBehavior)
{
  using namespace std::chrono;
  using namespace std::chrono_literals;

  const uint64_t clock_frequency_hz = 62'500'000; // NOLINT(build/unsigned)
  const double clock_frequency_Mhz = 62.5;

  utilities::TimestampEstimatorSystem te(clock_frequency_hz);

  auto system_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()); // NOLINT
  auto steady_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT
  auto daq_time_start =
    static_cast<uint64_t>((clock_frequency_Mhz) * static_cast<double>(system_time_start)); // NOLINT(build/unsigned)

  std::atomic<bool> do_not_continue_flag{ false };
  std::atomic<bool> do_continue_flag{ true };
  std::atomic<bool> continue_flag_for_thread{ true };
  std::atomic<bool> thread_has_finished{ false };
  std::atomic<utilities::TimestampEstimatorBase::WaitStatus> return_code_from_thread_wait{
    dunedaq::utilities::TimestampEstimatorBase::kInterrupted
  };

  // spawn a thread that waits until the TSE can provide a valid timestamp
  std::function<void()> valid_timestamp_wait_func = [&]() {
    return_code_from_thread_wait = te.wait_for_valid_timestamp(continue_flag_for_thread);
    thread_has_finished = true;
  };
  auto wait_ftr = std::async(std::launch::async, valid_timestamp_wait_func);

  // TimestampEstimatorSystem always provides valid timestamps
  std::this_thread::sleep_for(100ms);
  BOOST_CHECK_EQUAL(thread_has_finished, true);

  // TimestampEstimatorSystem always should return kFinished
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_not_continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kFinished);
  BOOST_CHECK_EQUAL(thread_has_finished, true);

  // verify that the wait thread has finished and it received the expected return code
  BOOST_CHECK_EQUAL(thread_has_finished, true);
  BOOST_CHECK_EQUAL(return_code_from_thread_wait, dunedaq::utilities::TimestampEstimatorBase::kFinished);
  // if the thread has not finished, tell it to finish now
  if (!thread_has_finished) {
    continue_flag_for_thread.store(false);
  }

  // verify that the TSE instance provides valid timestamps and those
  // timestamp track closely to wallclock time (computer system time)
  for (size_t i = 0; i < 10; ++i) {

    std::this_thread::sleep_for(100ms);
    auto steady_now =
      static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT
    uint64_t te_now = te.get_timestamp_estimate(); // NOLINT(build/unsigned)
    auto steady_diff = static_cast<int64_t>(steady_now - steady_time_start);
    auto te_diff = static_cast<int64_t>(te_now - daq_time_start);
    auto dd = static_cast<int64_t>(te_diff - (steady_diff * clock_frequency_hz / 1'000'000));

    BOOST_CHECK_LT(abs(dd), 1'000);
  }

  // now the TSE instance "wait" method should return immediately with a status
  // that indicates that it *does* have a valid timestamp, independent of whether
  // we tell it to wait for a valid timestamp or not
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_not_continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kFinished);
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_continue_flag),
                    dunedaq::utilities::TimestampEstimatorBase::kFinished);
}

BOOST_AUTO_TEST_CASE(AdditionalTestIdeas)
{
  // slow clock
  // fast clock
  // non-standard clock frequency
  // bursts and delays
}

BOOST_AUTO_TEST_SUITE_END()
