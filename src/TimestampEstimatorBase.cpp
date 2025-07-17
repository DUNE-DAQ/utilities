/**
 * @file TimestampEstimatorBase.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/TimestampEstimatorBase.hpp"

#include <thread>

namespace dunedaq {
namespace utilities {

/**
 * @brief Waits for a valid timestamp to become available.  Returns a status value that
 *        indicates whether a valid timestamp is available or not.
 * @param continue_flag whether to continue waiting until a valid timestamp is available
 *        or return immediately
 * @param last_seen_ts (Output parameter) the last timestamp seen by the method
 * @details The value of the continue_flag can be changed from true to false externally
 *          to this method, and that will cause the method to exit soon thereafter
 *          (on the order of 10 msec) and return the current status.
 * @return kFinished if a valid timestamp is available or kInterrupted if one is not
 */
TimestampEstimatorBase::WaitStatus
TimestampEstimatorBase::wait_for_valid_timestamp(std::atomic<bool>& continue_flag, uint64_t& last_seen_ts)
{
  last_seen_ts = s_invalid_ts;
  auto sleep_time = std::chrono::microseconds(1);
  // Always call get_timestamp_estimate at least once
  while ((last_seen_ts = get_timestamp_estimate()) == s_invalid_ts && continue_flag.load()) {
    std::this_thread::sleep_for(sleep_time);
    if (sleep_time < std::chrono::milliseconds(10)) {
      sleep_time *= 2;
    }
  }

  // 27-May-2025, KAB: modified this return statement so that the return code is based on whether a
  // valid timestamp is available (instead of whether the caller asked the method to wait or not)
  return (last_seen_ts != s_invalid_ts) ? TimestampEstimatorBase::kFinished
                                                    : TimestampEstimatorBase::kInterrupted;
}

TimestampEstimatorBase::WaitStatus
TimestampEstimatorBase::wait_for_requested_timestamp(uint64_t ts, std::atomic<bool>& continue_flag, uint64_t& last_seen_ts)
{
  last_seen_ts = s_invalid_ts;
  auto get_sleep_time = [this, ts]() {
    auto est = get_wait_estimate(ts);
    auto pest = static_cast<long>(est.count() * 0.8);
    if (pest < 1 && est != std::chrono::microseconds(0))
      return std::chrono::microseconds(1);
    return std::chrono::microseconds(pest);
  };
  auto sleep_time = get_sleep_time();
  // Always call get_timestamp_estimate at least once
  while (((last_seen_ts = get_timestamp_estimate()) < ts || last_seen_ts == s_invalid_ts) && continue_flag.load()) {
    std::this_thread::sleep_for(sleep_time);
    sleep_time = get_sleep_time();
  }
  return (last_seen_ts >= ts && last_seen_ts != s_invalid_ts) ? TimestampEstimatorBase::kFinished
                                                              : TimestampEstimatorBase::kInterrupted;
}

} // namespace utilities
} // namespace dunedaq
