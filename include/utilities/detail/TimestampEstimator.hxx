#include "logging/Logging.hpp"

namespace dunedaq {
namespace utilities {

template <class T>
void TimestampEstimator::timesync_callback(T& tsync)
{
  ++m_received_timesync_count;
  TLOG_DEBUG(TLVL_TIME_SYNC_PROPERTIES) << "Got a TimeSync run=" << tsync.run_number << " local run=" << m_run_number 
                                        << " seqno=" << tsync.sequence_number
                                        << " source_pid=" << tsync.source_pid;
  if (tsync.run_number == m_run_number) {
    add_timestamp_datapoint(tsync.daq_time, tsync.system_time);
  } else {
    TLOG_DEBUG(0) << "Discarded TimeSync message from run " << tsync.run_number << " during run "
                  << m_run_number;
  }
}

}
}