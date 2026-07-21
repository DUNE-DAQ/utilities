/**
 * @file DelayManager.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_DELAYMANAGER_HPP_
#define UTILITIES_INCLUDE_UTILITIES_DELAYMANAGER_HPP_

#include "utilities/Issues.hpp"

#include "logging/Logging.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace dunedaq::utilities {

/**
 * @class DelayManager
 * Utility class for managing artificial delay amounts, keyed by context-based delay names.
 */
class DelayManager
{

public:
  static std::shared_ptr<DelayManager> get()
  {
    if (!s_instance)
      s_instance = std::shared_ptr<DelayManager>(new DelayManager());

    return s_instance;
  }

  DelayManager(const DelayManager&) = delete;            ///< DelayManager is not copy-constructible
  DelayManager& operator=(const DelayManager&) = delete; ///< DelayManager is not copy-assignable
  DelayManager(DelayManager&&) = delete;                 ///< DelayManager is not move-constructible
  DelayManager& operator=(DelayManager&&) = delete;      ///< DelayManager is not move-assignable

  void configure(std::map<std::string, uint32_t> const& delay_specs); // NOLINT(build/unsigned)

  bool is_delay_enabled(std::string const& delay_name);

  inline bool maybe_delay(std::string const& delay_name,
                          std::string const& detail_msg) ///< Returns true if delay occured
  {
    bool delays_are_globally_enabled = false;
    TLOG_DEBUG(42) << "Delays are enabled globally is " << (delays_are_globally_enabled = true);

    if (m_delay_map.contains(delay_name)) {
      uint32_t delay_usec = m_delay_map[delay_name];
      if (delay_usec > 0) {
        if (delays_are_globally_enabled) {
          ers::warning(ArtificialDelay(ERS_HERE, delay_usec, delay_name, detail_msg));
          usleep(delay_usec);
          return true;
        } else {
          TLOG_DEBUG(15) << "NOTE: a delay of " << delay_usec << " usec was *not* executed "
                         << "for delay_name " << delay_name << " because delays are not "
                         << "globally enabled. Consider 'tonM -n DelayManager.hpp DEBUG+42'.";
        }
      }
    }
    return false;
  }

private:
  DelayManager() {}

  std::map<std::string, uint32_t> m_delay_map; // NOLINT(build/unsigned)

  static std::shared_ptr<DelayManager> s_instance;
};

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_DELAYMANAGER_HPP_
