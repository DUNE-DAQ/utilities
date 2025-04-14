/**
 * @file TimestampEstimator.hpp TimestampEstimator Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATOR_HPP_
#define UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATOR_HPP_

#include "utilities/TimestampEstimatorBase.hpp"
#include "utilities/Issues.hpp"

#include <atomic>
#include <memory>
#include <mutex>

namespace dunedaq {
namespace utilities {

/**
 * @brief TimestampEstimator is an implementation of
 * TimestampEstimatorBase that uses TimeSync messages from an input
 * queue to estimate the current timestamp
 **/
class TimestampEstimator : public TimestampEstimatorBase
{
public:
  TimestampEstimator(uint32_t run_number, uint64_t clock_frequency_hz);

  explicit TimestampEstimator(uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  virtual ~TimestampEstimator();

  uint64_t get_timestamp_estimate() const override;

  std::chrono::microseconds get_wait_estimate(uint64_t ts) const override;

  void add_timestamp_datapoint(uint64_t daq_time, uint64_t system_time);

  template <class T>
  void timesync_callback(const T& tsync);

  uint64_t get_received_timesync_count() const { return m_received_timesync_count.load(); }
private:

  struct TimeSyncPoint {
    uint64_t daq_time;
    std::chrono::time_point<std::chrono::steady_clock> system_time;
  };
  
  std::atomic<TimeSyncPoint> m_current_timestamp_estimate;


  uint64_t m_clock_frequency_hz; // NOLINT(build/unsigned)
  uint64_t m_most_recent_daq_time;
  uint64_t m_most_recent_system_time;
  std::mutex m_datapoint_mutex;
  uint32_t m_run_number {0};
  std::atomic<uint64_t> m_received_timesync_count; // NOLINT(build/unsigned)
  uint32_t m_current_process_id;
};

} // namespace utilities
} // namespace dunedaq

#include "detail/TimestampEstimator.hxx"

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATOR_HPP_
