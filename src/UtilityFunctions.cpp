/**
 * @file toolbox.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "toolbox/UtilityFunctions.hpp"
#include "toolbox/Issues.hpp"

// Boost Headers
#include "boost/foreach.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// C++ Headers
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <wordexp.h>

using namespace std;

namespace dunedaq {

//-----------------------------------------------------------------------------
void
millisleep(const double& time_in_milliseconds)
{
  double time_in_seconds(time_in_milliseconds / 1e3);
  int integer_part(static_cast<int>(time_in_seconds));
  double fractional_part(time_in_seconds - static_cast<double>(integer_part));
  struct timespec sleep_time, return_time;
  sleep_time.tv_sec = integer_part;
  sleep_time.tv_nsec = static_cast<long>(fractional_part * 1e9); // NOLINT
  nanosleep(&sleep_time, &return_time);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
strprintf(const char* fmt, ...) // NOLINT
{
  char* ret;
  va_list ap;
  va_start(ap, fmt);
  vasprintf(&ret, fmt, ap); // NOLINT
  va_end(ap);
  std::string str(ret);
  free(ret);
  return str;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<std::string>
shell_expand_paths(const std::string& path)
{

  std::vector<std::string> paths;
  wordexp_t substituted_path;
  int code = wordexp(path.c_str(), &substituted_path, WRDE_NOCMD);
  if (code)
    throw runtime_error("Failed expanding path: " + path);

  for (std::size_t i = 0; i != substituted_path.we_wordc; i++)
    paths.push_back(substituted_path.we_wordv[i]);

  wordfree(&substituted_path);

  return paths;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
shellExpandPath(const std::string& path)
{
  std::vector<std::string> paths = shell_expand_paths(path);

  if (paths.size() > 1)
    throw runtime_error("Failed to expand: multiple matches found");

  return paths[0];
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
throw_if_not_file(const std::string& path)
{

  // FIXME: Review the implementation. The function never returns
  namespace fs = boost::filesystem;

  // Check that the path exists and that it's not a directory
  fs::path cfgFile(path);
  if (!fs::exists(cfgFile)) {
    throw toolbox::FileNotFound(ERS_HERE, path);
  } else if (fs::is_directory(cfgFile)) {
    throw toolbox::FileIsDirectory(ERS_HERE, path);
  }

  //    return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int64_t
get_seconds_since_epoch()
{
  // get the current time
  const auto now = std::chrono::system_clock::now();

  // transform the time into a duration since the epoch
  const auto epoch = now.time_since_epoch();

  // cast the duration into seconds
  const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

  // return the number of seconds
  return seconds.count();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
format_timestamp(uint64_t raw_timestamp, uint32_t clock_frequency_hz) // NOLINT(build/unsigned)
{
  std::time_t sec_from_epoch = raw_timestamp / clock_frequency_hz;

  struct tm* time = localtime(&sec_from_epoch); // NOLINT
  char time_buffer[32];

  strftime(time_buffer, sizeof time_buffer, "%a, %d %b %Y %H:%M:%S +0000", time);
  return time_buffer;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
convert_bits_to_float(uint64_t bits, bool is_double_precision) // NOLINT(build/unsigned)
{
  uint32_t mantissa_shift = is_double_precision ? 52 : 23;                        // NOLINT(build/unsigned)
  uint64_t exponent_mask = is_double_precision ? 0x7FF0000000000000 : 0x7f800000; // NOLINT(build/unsigned)
  uint32_t bias = is_double_precision ? 1023 : 127;                               // NOLINT(build/unsigned)
  uint32_t sign_shift = is_double_precision ? 63 : 31;                            // NOLINT(build/unsigned)

  int32_t sign = (bits >> sign_shift) & 0x01;
  uint32_t exponent_biased = ((bits & exponent_mask) >> mantissa_shift); // NOLINT(build/unsigned)
  int32_t exponent = exponent_biased - bias;

  int32_t power = -1;
  double mantissa = 0.0;
  for (uint32_t i = 0; i < mantissa_shift; ++i) {                      // NOLINT(build/unsigned)
    uint64_t mantissa_bit = (bits >> (mantissa_shift - i - 1)) & 0x01; // NOLINT(build/unsigned)
    mantissa += mantissa_bit * pow(2.0, power);
    --power;
  }

  if (exponent_biased == 0) {
    ++exponent;
    if (mantissa == 0)
      return 0;
  } else {
    mantissa += 1.0;
  }
  return (sign ? -1 : 1) * pow(2.0, exponent) * mantissa;
}
//-----------------------------------------------------------------------------

template std::string
vec_fmt<uint32_t>(const std::vector<uint32_t>& vec); // NOLINT(build/unsigned)
template std::string
short_vec_fmt<uint32_t>(const std::vector<uint32_t>& vec); // NOLINT(build/unsigned)

//-----------------------------------------------------------------------------

} // namespace dunedaq