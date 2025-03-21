/**
 * @file TimestampEstimator.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/TimestampEstimator.hpp"
#include "utilities/Issues.hpp"

#include "logging/Logging.hpp"

#include <memory>
#include <unistd.h>

#define TRACE_NAME "TimestampEstimator" // NOLINT

namespace dunedaq {
namespace utilities {
TimestampEstimator::TimestampEstimator(uint32_t run_number, uint64_t clock_frequency_hz) // NOLINT(build/unsigned)
  : TimestampEstimator(clock_frequency_hz)
{
  m_run_number = run_number;
}

TimestampEstimator::TimestampEstimator(uint64_t clock_frequency_hz) // NOLINT(build/unsigned)
  : m_current_timestamp_estimate(TimeSyncPoint{std::numeric_limits<uint64_t>::max(), std::chrono::time_point<std::chrono::steady_clock>()})
  , m_clock_frequency_hz(clock_frequency_hz)
  , m_most_recent_daq_time(0)
  , m_most_recent_system_time(0)
  , m_run_number(0)
  , m_received_timesync_count(0)
{
  m_current_process_id = static_cast<uint32_t>(getpid());
}

TimestampEstimator::~TimestampEstimator()
{
}

uint64_t
TimestampEstimator::get_timestamp_estimate() const {
  using namespace std::chrono;
  
  TimeSyncPoint estimate = m_current_timestamp_estimate.load();
  
  auto delta_time_us = duration_cast<microseconds>(steady_clock::now() - estimate.system_time).count();
      
  const uint64_t new_timestamp =
    estimate.daq_time + delta_time_us * m_clock_frequency_hz / 1000000;

  return new_timestamp;

}

std::chrono::microseconds
TimestampEstimator::get_wait_estimate(uint64_t ts) const
{
  auto now = get_timestamp_estimate();
  auto diff = ts - now;
  return std::chrono::microseconds(static_cast<long>(diff * 1000000. / m_clock_frequency_hz));
}


void
TimestampEstimator::add_timestamp_datapoint(uint64_t daq_time, uint64_t system_time)
{
    using namespace std::chrono;

  std::scoped_lock<std::mutex> lk(m_datapoint_mutex);

  // First, update the latest timestamp
  TimeSyncPoint estimate = m_current_timestamp_estimate.load();
  int64_t diff = estimate.daq_time - daq_time;
  TLOG_DEBUG(TLVL_TIME_SYNC_PROPERTIES) << "Got a TimeSync timestamp = " << daq_time
                                        << ", system time = " << system_time
                                        << " when current timestamp estimate was " << estimate.daq_time << ". diff=" << diff;

  if (m_most_recent_daq_time == std::numeric_limits<uint64_t>::max() ||
      daq_time > m_most_recent_daq_time) {
    m_most_recent_daq_time = daq_time;
    m_most_recent_system_time = system_time;
  }

  if (m_most_recent_daq_time != std::numeric_limits<uint64_t>::max()) {
    // Update the current timestamp estimate, based on the most recently-read TimeSync
    using namespace std::chrono;

    auto time_now =
      static_cast<uint64_t>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()); // NOLINT
    auto steady_time_now = steady_clock::now();

    // (PAR 2021-07-22) We only want to _increase_ our timestamp
    // estimate, not _decrease_ it, so we only attempt the update if
    // our system time is later than the latest time sync's system
    // time. We can get TimeSync messages from the "future" if
    // they're coming from another host whose clock is not exactly
    // synchronized with ours: that's fine, but if the discrepancy
    // is large, then badness could happen, so emit a warning

    if (time_now < m_most_recent_system_time - 10000) {
      ers::warning(EarlyTimeSync(ERS_HERE, m_most_recent_system_time - time_now));
    }

    if (time_now > m_most_recent_system_time) {

      auto delta_time = time_now - m_most_recent_system_time;
      TLOG_DEBUG(TLVL_TIME_SYNC_PROPERTIES)
        << "Time diff between current system and latest TimeSync system time [us]: " << delta_time;

      // Warn user if current system time is more than 1s ahead of latest TimeSync system time. This could be a sign of
      // an issue, e.g. machine times out of sync
      if (delta_time > 1e6)
        ers::warning(LateTimeSync(ERS_HERE, delta_time));

      const uint64_t new_timestamp =
        m_most_recent_daq_time + delta_time * m_clock_frequency_hz / 1000000;

      // Don't ever decrease the timestamp; just wait until enough
      // time passes that we want to increase it
      if (estimate.daq_time == std::numeric_limits<uint64_t>::max() ||
          new_timestamp >= estimate.daq_time) {
        TLOG_DEBUG(TLVL_TIME_SYNC_NEW_ESTIMATE)
          << "Storing new timestamp estimate of " << new_timestamp << " ticks (..." << std::fixed
          << std::setprecision(8)
          << (static_cast<double>(new_timestamp % (m_clock_frequency_hz * 1000)) /
              static_cast<double>(m_clock_frequency_hz))
          << " sec), mrt.daq_time is " << m_most_recent_daq_time << " ticks (..."
          << (static_cast<double>(m_most_recent_daq_time % (m_clock_frequency_hz * 1000)) /
              static_cast<double>(m_clock_frequency_hz))
          << " sec), delta_time is " << delta_time << " usec, clock_freq is " << m_clock_frequency_hz << " Hz";
        m_current_timestamp_estimate.store(TimeSyncPoint{new_timestamp, steady_time_now});
      } else {
        TLOG_DEBUG(TLVL_TIME_SYNC_NOTES) << "Not updating timestamp estimate backwards from "
                                         << m_current_timestamp_estimate.load().daq_time << " to " << new_timestamp;
      }
    }
  }
}

} // namespace utilities
} // namespace dunedaq
