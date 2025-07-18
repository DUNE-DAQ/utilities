/**
 * @file TimestampEstimatorSystem.hpp TimestampEstimatorSystem Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_
#define UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_

#include "utilities/Issues.hpp"
#include "utilities/TimestampEstimatorBase.hpp"

namespace dunedaq::utilities {

/**
 * @brief TimestampEstimatorSystem is an implementation of
 * TimestampEstimatorBase that uses the system clock to give the current timestamp
 **/
class TimestampEstimatorSystem : public TimestampEstimatorBase
{
public:
  explicit TimestampEstimatorSystem(uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  uint64_t get_timestamp_estimate() const override; // NOLINT(build/unsigned)

  std::chrono::microseconds get_wait_estimate(uint64_t ts) const override; // NOLINT(build/unsigned)

private:
  uint64_t m_clock_frequency_hz; // NOLINT(build/unsigned)
};

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_
