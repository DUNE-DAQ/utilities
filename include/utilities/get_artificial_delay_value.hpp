/**
 *
 * @file get_artificial_delay_value.hpp Look up the value of the requested artificial delay
 * for a specific context.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef UTILITIES_INCLUDE_UTILITIES_GET_ARTIFICIAL_DELAY_VALUE_HPP_
#define UTILITIES_INCLUDE_UTILITIES_GET_ARTIFICIAL_DELAY_VALUE_HPP_

#include "logging/Logging.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <vector>

using nlohmann::json;

namespace dunedaq::utilities {

/**
 * This helper function looks up the requested artificial delay value from the
 * DUNEDAQ_ARTIFICIAL_DELAYS environmental variable.  If that env var is not set
 * or is empty, then a delay value of zero is returned.
 *
 * The contents of this env var are expected to be a JSON string that contains
 * various context names as keys and delay amounts, or sub-context information,
 * as values.
 * For example:
 *  - {"~ZmqSender": 10000000} would specify a delay of 10 sec (10e6 usec) for
 *    use in the ZmqSender destructor.
 *  - {"~ZmqSender": {"data_requests_for_mlt": 10000000}} would specify a delay
 *    of 10 sec (10e6 usec) for use in the ZmqSender destructor only if the sender
 *    is processing messages on the 'data_requests_for_mlt' connection.
 * These names and values are intended to be user-defined and can be context-specific.
 *
 * A caller of this function should specify the list of (possibly nested) keys as
 * the elements of the input vector.  So, for the "~ZmqSender", "data_requests_for_mlt"
 * example mentioned above, the calling code might look like the following:
 *  - std::vector<std::string> key_list = {"~ZmqSender", "data_requests_for_mlt"};
 *  - size_t delay_value = utilities::get_artificial_delay_value(key_list);
 *
 * Note that when we store strings in the DUNEDAQ_ARTIFICIAL_DELAYS env var, we
 * will likely need to escape the double quotes in the JSON string.  For example,
 * what we store in the env var entry in an OKS database might look like the following:
 *  <obj class="Variable" id="local-env-artificial-delay-settingss">
 *   <attr name="name" type="string" val="DUNEDAQ_ARTIFICIAL_DELAYS"/>
 *   <attr name="value" type="string" val="{\&quot;~ZmqSender\&quot;: {\&quot;data_requests_for_mlt\&quot;:
 * 10000000}}"/>
 *  </obj>
 */
inline size_t
get_artificial_delay_value(std::vector<std::string> keys)
{
  char* env_var_value = getenv("DUNEDAQ_ARTIFICIAL_DELAYS");
  if (env_var_value == nullptr) {
    return 0;
  }

  std::string delays_string(env_var_value);
  if (delays_string.size() < 1) {
    return 0;
  }

  size_t delay_value = 0;
  try {
    json delays_json = json::parse(delays_string);

    for (const auto& key : keys) {
      auto result = delays_json.find(key);
      if (result == delays_json.end()) {
        break;
      }

      if (result->is_number()) {
        delay_value = *result;
      }
      if (!result->is_object()) {
        break;
      }
      delays_json = *result;
    }
  } catch (json::parse_error& ex) {
  }
  return delay_value;
}

} // namespace dunedaq::utilities

#endif // UTILITIES_INCLUDE_UTILITIES_GET_ARTIFICIAL_DELAY_VALUE_HPP_
