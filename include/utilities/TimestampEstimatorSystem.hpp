/**
 * @file TimestampEstimatorSystem.hpp TimestampEstimatorSystem Class
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_
#define UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_

#include "utilities/TimestampEstimatorBase.hpp"
#include "utilities/Issues.hpp"

namespace dunedaq {
namespace utilities {

/**
 * @brief TimestampEstimatorSystem is an implementation of
 * TimestampEstimatorBase that uses the system clock to give the current timestamp
 **/
class TimestampEstimatorSystem : public TimestampEstimatorBase
{
public:
  explicit TimestampEstimatorSystem(uint64_t clock_frequency_hz); // NOLINT(build/unsigned)

  uint64_t get_timestamp_estimate() const override;

private:
  uint64_t m_clock_frequency_hz; // NOLINT(build/unsigned)
};

} // namespace utilities
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_TIMESTAMPESTIMATORSYSTEM_HPP_