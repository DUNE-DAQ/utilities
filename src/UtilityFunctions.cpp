/**
 * @file toolbox.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "toolbox/UtilityFunctions.hpp"
#include "toolbox/Issues.hpp"

// C++ Headers
#include <ctime>
#include <filesystem>
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
  namespace fs = std::filesystem;

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

template std::string
vec_fmt<uint32_t>(const std::vector<uint32_t>& vec); // NOLINT(build/unsigned)
template std::string
short_vec_fmt<uint32_t>(const std::vector<uint32_t>& vec); // NOLINT(build/unsigned)

//-----------------------------------------------------------------------------

} // namespace dunedaq