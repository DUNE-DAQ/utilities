/**
 * @file TimestampEstimatorSystem.hpp TimestampEstimatorSystem Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORBASE_HPP_
#define UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORBASE_HPP_

#include <atomic>
#include <chrono>
#include <limits>

namespace dunedaq::utilities {

/**
 * @brief TimestampEstimatorBase is the base class for timestamp-based
 * logic in test systems where the current timestamp must be estimated
 * somehow (eg, because there is no hardware timing system).
 **/
class TimestampEstimatorBase
{
public:
  static constexpr uint64_t s_invalid_ts = std::numeric_limits<uint64_t>::max(); // NOLINT(build/unsigned)

  virtual ~TimestampEstimatorBase() = default;
  virtual uint64_t get_timestamp_estimate() const = 0;                        // NOLINT(build/unsigned)
  virtual std::chrono::microseconds get_wait_estimate(uint64_t ts) const = 0; // NOLINT(build/unsigned)

  enum WaitStatus
  {
    kFinished,
    kInterrupted
  };

  /**
     Wait for the current timestamp estimate to become valid, or for
     continue_flag to become false. The timestamp becomes valid once at
     least one TimeSync message has been received.

     Returns kFinished if the timestamp became valid, or kInterrupted if continue_flag became false first
  */
  WaitStatus wait_for_valid_timestamp(std::atomic<bool>& continue_flag)
  {
    uint64_t ts_discard = s_invalid_ts; // NOLINT(build/unsigned)
    return wait_for_valid_timestamp(continue_flag, ts_discard);
  }
  WaitStatus wait_for_valid_timestamp(std::atomic<bool>& continue_flag,
                                      uint64_t& last_seen_ts); // NOLINT(build/unsigned)

  /**
     Wait for the current timestamp estimate to reach ts, or for
     continue_flag to become false.

     Returns kFinished if the timestamp became valid, or kInterrupted if continue_flag became false first
  */
  WaitStatus wait_for_requested_timestamp(uint64_t ts, std::atomic<bool>& continue_flag) // NOLINT(build/unsigned)
  {
    uint64_t ts_discard = s_invalid_ts; // NOLINT(build/unsigned)
    return wait_for_requested_timestamp(ts, continue_flag, ts_discard);
  }
  WaitStatus wait_for_requested_timestamp(uint64_t ts, // NOLINT(build/unsigned)
                                          std::atomic<bool>& continue_flag,
                                          uint64_t& last_seen_ts); // NOLINT(build/unsigned)
};

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORBASE_HPP_
