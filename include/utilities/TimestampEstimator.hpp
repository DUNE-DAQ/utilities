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

#include "utilities/TimeSync.hpp"
#include "daqdataformats/Types.hpp"
#include "utilities/WorkerThread.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

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
  TimestampEstimator(daqdataformats::run_number_t run_number, uint64_t clock_frequency_hz);

  explicit TimestampEstimator(uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  virtual ~TimestampEstimator();

  daqdataformats::timestamp_t get_timestamp_estimate() const override { return m_current_timestamp_estimate.load(); }

  void add_timestamp_datapoint(const utilities::TimeSync& ts);

  void timesync_callback(utilities::TimeSync& tsync);

  uint64_t get_received_timesync_count() { return m_received_timesync_count.load(); }
private:
  void estimator_thread_fn(std::atomic<bool>& running_flag);

  // The estimate of the current timestamp
  std::atomic<daqdataformats::timestamp_t> m_current_timestamp_estimate{ daqdataformats::TypeDefaults::s_invalid_timestamp };

  uint64_t m_clock_frequency_hz; // NOLINT(build/unsigned)
  utilities::TimeSync m_most_recent_timesync;
  std::mutex m_datapoint_mutex;
  daqdataformats::run_number_t m_run_number {0};
  std::atomic<uint64_t> m_received_timesync_count; // NOLINT(build/unsigned)
};

} // namespace utilities
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATOR_HPP_
