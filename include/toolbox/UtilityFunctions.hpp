/**
 * @file UtilityFunctions.hpp
 *
 * Utility functions from timing which may be generally-useful
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TOOLBOX_INCLUDE_TOOLBOX_UTILITYFUNCTIONS_HPP_
#define TOOLBOX_INCLUDE_TOOLBOX_UTILITYFUNCTIONS_HPP_

// C++ Headers
#include <string>
#include <vector>

namespace dunedaq {

// Wrappers to be used by lexical_cast
template<typename T>
struct stol;
template<typename T>
struct stoul;

template<typename T>
std::string
to_string(const T&);

template<typename T, typename U>
T
safe_enum_cast(const U& value, const std::vector<T>& valid, const T& def);

/**
 * Sleeps for a given number of milliseconds
 *
 * @param      time_in_milliseconds  Number of milliseconds to sleep
 */
void
millisleep(const double& time_in_milliseconds);

/**
 * Formats a std::string in printf fashion
 *
 * @param[in]      aFmt   Format string
 * @param[in]      ...   List of arguments
 *
 * @return     A formatted string
 */
std::string
strprintf(const char* fmt, ...); // NOLINT

/**
 * Expand the path
 * @param path path to expand
 * @return vector of expanded paths
 */
std::vector<std::string>
shell_expand_paths(const std::string& path);

/**
 * Performs variable subsitutition on path
 * @param path: Path to expand
 * @return Expanded path
 */
std::string
shellExpandPath(const std::string& path);

/**
 * Checks that the input path corresponds to an existing filesystem item which
 * is a file
 * @param path: Path to file
 */
void
throw_if_not_file(const std::string& path);

/**
 * @brief      Converts a vector of strings in a delimiter-separated string.
 *
 * @param[in]  aStrings    Collection of strings
 * @param[in]  aDelimiter  A delimiter
 *
 * @tparam     C           { description }
 *
 * @return     { description_of_the_return_value }
 */
template<typename C>
std::string
join(const C& strings, const std::string& delimiter = " ");

/**
 * ""
 * @return
 */
int64_t
get_seconds_since_epoch();

/**
 * ""
 * @return
 */
std::string
format_timestamp(uint64_t raw_timestamp, uint32_t clock_frequency_hz); // NOLINT(build/unsigned)

template<typename T>
std::string
vec_fmt(const std::vector<T>& vec);

template<typename T>
std::string
short_vec_fmt(const std::vector<T>& sorted_vec);

} // namespace dunedaq

#include "detail/UtilityFunctions.hxx"

#endif // TOOLBOX_INCLUDE_TOOLBOX_UTILITYFUNCTIONS_HPP_
