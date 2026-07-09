/**
 * @file DelayManager.cpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/DelayManager.hpp"

#include <map>
#include <memory>
#include <string>

namespace dunedaq::utilities {

std::shared_ptr<DelayManager> DelayManager::s_instance = nullptr;

void
DelayManager::configure(std::map<std::string, uint32_t> const& delay_specs) // NOLINT(build/unsigned)
{
  m_delay_map = delay_specs;
}

bool
DelayManager::is_delay_enabled(std::string const& delay_name)
{
  if (m_delay_map.contains(delay_name)) {
    if (m_delay_map[delay_name] > 0) {
      return true;
    }
  }
  return false;
}

} // namespace dunedaq::utilities
