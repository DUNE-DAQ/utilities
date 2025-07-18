/**
 * @file TimestampEstimator.hpp TimestampEstimator Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 *
 * This is an implementation of TimestampEstimatorBase that uses timestamps
 * from the TimeSync messages received by the application
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORTIMESYNC_HPP_
#define UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORTIMESYNC_HPP_

#include "utilities/Issues.hpp"
#include "utilities/TimestampEstimatorBase.hpp"

#include <atomic>
#include <memory>
#include <mutex>

namespace dunedaq::utilities {

enum
{
  TLVL_TIME_SYNC_PROPERTIES = 17,
  TLVL_TIME_SYNC_NOTES = 18,
  TLVL_TIME_SYNC_NEW_ESTIMATE = 19,
};

/**
 * @brief TimestampEstimatorTimeSync is an implementation of
 * TimestampEstimatorBase that uses TimeSync messages from an input
 * queue to estimate the current timestamp
 **/
class TimestampEstimatorTimeSync : public TimestampEstimatorBase
{
public:
  TimestampEstimatorTimeSync(uint32_t run_number, uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  explicit TimestampEstimatorTimeSync(uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  virtual ~TimestampEstimatorTimeSync();

  uint64_t get_timestamp_estimate() const override; // NOLINT(build/unsigned)

  std::chrono::microseconds get_wait_estimate(uint64_t ts) const override; // NOLINT(build/unsigned)

  void add_timestamp_datapoint(uint64_t daq_time, uint64_t system_time); // NOLINT(build/unsigned)

  template<class T>
  void timesync_callback(const T& tsync);

  uint64_t get_received_timesync_count() const { return m_received_timesync_count.load(); } // NOLINT(build/unsigned)

private:
  struct TimeSyncPoint
  {
    uint64_t daq_time{ s_invalid_ts }; // NOLINT(build/unsigned)
    std::chrono::time_point<std::chrono::steady_clock> system_time;
  };

  std::atomic<TimeSyncPoint> m_current_timestamp_estimate;

  uint64_t m_clock_frequency_hz;                   // NOLINT(build/unsigned)
  uint64_t m_most_recent_daq_time{ s_invalid_ts }; // NOLINT(build/unsigned)
  uint64_t m_most_recent_system_time;              // NOLINT(build/unsigned)
  std::mutex m_datapoint_mutex;
  uint32_t m_run_number{ 0 };                      // NOLINT(build/unsigned)
  std::atomic<uint64_t> m_received_timesync_count; // NOLINT(build/unsigned)
  uint32_t m_current_process_id;                   // NOLINT(build/unsigned)
};

} // namespace dunedaq::utilities

#include "detail/TimestampEstimatorTimeSync.hxx"

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORTIMESYNC_HPP_
