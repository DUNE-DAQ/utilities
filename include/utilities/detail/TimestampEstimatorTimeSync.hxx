#include "logging/Logging.hpp"

namespace dunedaq::utilities {

template<class T>
void
TimestampEstimatorTimeSync::timesync_callback(const T& tsync)
{
  ++m_received_timesync_count;
  TLOG_DEBUG(TLVL_TIME_SYNC_PROPERTIES) << "Got a TimeSync run=" << tsync.run_number << " local run=" << m_run_number
                                        << " seqno=" << tsync.sequence_number << " source_id=" << tsync.source_id;
  if (tsync.run_number == m_run_number) {
    add_timestamp_datapoint(tsync.daq_time, tsync.system_time);
  } else {
    TLOG_DEBUG(0) << "Discarded TimeSync message from run " << tsync.run_number << " during run " << m_run_number
                  << " with source_id " << tsync.source_id << " and timestamp " << tsync.daq_time;
  }
}

} // namespace dunedaq::utilities
