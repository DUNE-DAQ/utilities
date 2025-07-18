/**
 * @file TimestampEstimatorTimeSync_test.cxx  TimestampEstimatorTimeSync class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

// #include "iomanager/IOManager.hpp"
// #include "iomanager/Sender.hpp"
// #include "iomanager/Receiver.hpp"
#include "utilities/TimestampEstimatorTimeSync.hpp"

/**
 * @brief Name of this test module
 */
#define BOOST_TEST_MODULE TimestampEstimatorTimeSync_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <map>
#include <memory>
#include <string>

struct DummyTimeSync
{
  uint64_t daq_time{ dunedaq::utilities::TimestampEstimatorBase::s_invalid_ts }; // NOLINT(build/unsigned)
  /// The current system time
  uint64_t system_time{ 0 }; // NOLINT(build/unsigned)
  /// Sequence Number of this message, for debugging
  uint64_t sequence_number{ 0 }; // NOLINT(build/unsigned)
  /// Run number at time of creation
  uint32_t run_number{ 0 }; // NOLINT(build/unsigned)
  /// PID of the creating process, for debugging
  uint32_t source_pid{ 0 }; // NOLINT(build/unsigned)
};

using namespace dunedaq;

BOOST_AUTO_TEST_SUITE(BOOST_TEST_MODULE)

// /**
//  * @brief Initializes the QueueRegistry
//  */
// struct DAQSinkDAQSourceTestFixture
// {
//   DAQSinkDAQSourceTestFixture() {}

//   void setup()
//   {
//     iomanager::ConnectionIds_t connections;
//     connections.emplace_back(
//       iomanager::ConnectionId{ "dummy", iomanager::ServiceType::kQueue, "TimeSync", "queue://kFollyMPMCQueue:100" });

//     get_iomanager()->configure(connections);
//   }

//   void teardown() {
//       get_iomanager()->reset();
//   }
// };

// BOOST_TEST_GLOBAL_FIXTURE(DAQSinkDAQSourceTestFixture);

BOOST_AUTO_TEST_CASE(Basics)
{
  using namespace std::chrono;
  using namespace std::chrono_literals;

  const uint64_t clock_frequency_hz = 62'500'000; // NOLINT(build/unsigned)
  const double clock_frequency_Mhz = 62.5;

  const uint32_t run_num = 5; // NOLINT(build/unsigned)
  utilities::TimestampEstimatorTimeSync te(run_num, clock_frequency_hz);

  uint64_t daq_time_start = 1'000'000; // NOLINT(build/unsigned)
  auto system_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()); // NOLINT
  auto steady_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT

  DummyTimeSync ts;
  ts.daq_time = daq_time_start;
  ts.system_time = system_time_start;
  ts.sequence_number = 1;
  ts.run_number = run_num;
  ts.source_pid = 12345;

  te.timesync_callback(ts);

  for (size_t i = 0; i < 100; ++i) {

    std::this_thread::sleep_for(10ms);
    auto steady_now =
      static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT
    uint64_t te_now = te.get_timestamp_estimate(); // NOLINT(build/unsigned)
    auto steady_diff = static_cast<double>(steady_now - steady_time_start);
    auto te_diff = static_cast<double>(te_now - daq_time_start);
    auto dd = static_cast<int64_t>(te_diff - (steady_diff * clock_frequency_Mhz));

    BOOST_CHECK_LT(abs(dd), 1'000);
  }
}

// 27-May-2025, KAB: this test case is intended to verify that an instance of
// the TimestampEstimatorTimeSync behaves as expected when it first starts up.
BOOST_AUTO_TEST_CASE(StartupBehavior)
{
  using namespace std::chrono;
  using namespace std::chrono_literals;

  const uint64_t clock_frequency_hz = 62'500'000; // NOLINT(build/unsigned)
  const double clock_frequency_Mhz = 62.5;

  const uint32_t run_num = 5; // NOLINT(build/unsigned)
  utilities::TimestampEstimatorTimeSync te(run_num, clock_frequency_hz);

  uint64_t daq_time_start = 1'000'000; // NOLINT(build/unsigned)
  auto system_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()); // NOLINT
  auto steady_time_start =
    static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT

  // create a dummy TimeSync message for later use
  DummyTimeSync ts;
  ts.daq_time = daq_time_start;
  ts.system_time = system_time_start;
  ts.sequence_number = 1;
  ts.run_number = run_num;
  ts.source_pid = 12345;

  std::atomic<bool> do_not_continue_flag{ false };
  std::atomic<bool> do_continue_flag{ true };
  std::atomic<bool> continue_flag_for_thread{ true };
  std::atomic<bool> thread_has_finished{ false };
  std::atomic<utilities::TimestampEstimatorBase::WaitStatus> return_code_from_thread_wait{
    utilities::TimestampEstimatorBase::kInterrupted
  };

  // spawn a thread that waits until the TSE can provide a valid timestamp
  std::function<void()> valid_timestamp_wait_func = [&]() {
    return_code_from_thread_wait = te.wait_for_valid_timestamp(continue_flag_for_thread);
    thread_has_finished = true;
  };
  auto wait_ftr = std::async(std::launch::async, valid_timestamp_wait_func);

  // verify that a TSE instance that hasn't received any TimeSync messages returns
  // a special value that indicates that it can't yet provide a valid timestamp
  // when we ask it for the current timestamp estimate.
  // Also, verify that the wait thread is still waiting.
  for (size_t i = 0; i < 10; ++i) {

    std::this_thread::sleep_for(100ms);
    uint64_t te_now = te.get_timestamp_estimate(); // NOLINT(build/unsigned)
    BOOST_CHECK_EQUAL(te_now, utilities::TimestampEstimatorBase::s_invalid_ts);

    BOOST_CHECK_EQUAL(thread_has_finished, false);
  }

  // if we ask the TSE if it has a valid timestamp, and we tell it that it doesn't
  // need to wait until it gets one, AND the TSE instance has not yet received
  // a TimeSync message, it should return immediately and tell us that it is returning
  // without being able to provide a valid timestamp (kInterrupted).
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_not_continue_flag), utilities::TimestampEstimatorBase::kInterrupted);
  BOOST_CHECK_EQUAL(thread_has_finished, false);

  // pass a TimeSync message into the TSE instance
  te.timesync_callback(ts);

  // verify that the wait thread has finished and it received the expected return code
  std::this_thread::sleep_for(std::chrono::milliseconds(25));
  BOOST_CHECK_EQUAL(thread_has_finished, true);
  BOOST_CHECK_EQUAL(return_code_from_thread_wait, utilities::TimestampEstimatorBase::kFinished);
  // if the thread has not finished, tell it to finish now
  if (!thread_has_finished) {
    continue_flag_for_thread.store(false);
  }

  // verify that the TSE instance now provides valid timestamps and those
  // timestamp track closely to wallclock time (computer system time)
  for (size_t i = 0; i < 10; ++i) {

    std::this_thread::sleep_for(100ms);
    auto steady_now =
      static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count()); // NOLINT
    auto te_now = te.get_timestamp_estimate();
    auto steady_diff = static_cast<double>(steady_now - steady_time_start);
    auto te_diff = static_cast<double>(te_now - daq_time_start);
    auto dd = static_cast<int64_t>(te_diff - (steady_diff * clock_frequency_Mhz));

    BOOST_CHECK_LT(abs(dd), 1'000);
  }

  // now the TSE instance "wait" method should return immediately with a status
  // that indicates that it *does* have a valid timestamp, independent of whether
  // we tell it to wait for a valid timestamp or not
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_not_continue_flag), utilities::TimestampEstimatorBase::kFinished);
  BOOST_CHECK_EQUAL(te.wait_for_valid_timestamp(do_continue_flag), utilities::TimestampEstimatorBase::kFinished);
}

BOOST_AUTO_TEST_CASE(AdditionalTestIdeas)
{
  // slow clock
  // fast clock
  // non-standard clock frequency
  // bursts and delays
}

BOOST_AUTO_TEST_SUITE_END()
