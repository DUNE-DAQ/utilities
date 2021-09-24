#ifndef TOOLBOX_INCLUDE_DETAIL_UTILITYFUNCTIONS_HXX_
#define TOOLBOX_INCLUDE_DETAIL_UTILITYFUNCTIONS_HXX_

#include <logging/Logging.hpp>

#include <boost/lexical_cast.hpp>

namespace dunedaq {

//-----------------------------------------------------------------------------
template<typename T>
struct stoul
{
  BOOST_STATIC_ASSERT((boost::is_unsigned<T>::value));
  T value;

  operator T() const { return value; }

  friend std::istream& operator>>(std::istream& in, stoul& out)
  {
    std::string buf;
    in >> buf;
    out.value = strtoul(buf.c_str(), NULL, 0);
    return in;
  }
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
struct stol
{
  BOOST_STATIC_ASSERT((boost::is_signed<T>::value));
  T value;

  operator T() const { return value; }

  friend std::istream& operator>>(std::istream& in, stol& out)
  {
    std::string buf;
    in >> buf;
    out.value = strtol(buf.c_str(), NULL, 0);
    return in;
  }
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
to_string(const T& v)
{
  return boost::lexical_cast<std::string>(v);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename M>
bool
map_value_comparator(typename M::value_type& p1, typename M::value_type& p2)
{
  return p1.second < p2.second;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::vector<T>
sanitize(const std::vector<T>& vec)
{
  // Sanitise the inputs, by copying
  std::vector<uint32_t> sorted(vec);

  // ...sorting...
  std::sort(sorted.begin(), sorted.end());

  // and delete the duplicates (erase+unique require a sorted vector to delete duplicates)
  sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());

  return sorted;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename U>
T
safe_enum_cast(const U& value, const std::vector<T>& valid, const T& def)
{
  typename std::vector<T>::const_iterator it = std::find(valid.begin(), valid.end(), static_cast<T>(value));
  return (it != valid.end() ? *it : def);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename C>
std::string
join(const C& strings, const std::string& delimiter)
{

  if (strings.empty())
    return "";

  std::ostringstream string_stream;

  string_stream << *strings.begin();

  for (auto str_iter = std::next(strings.begin()); str_iter != strings.end(); ++str_iter) {
    string_stream << delimiter;
    string_stream << *str_iter;
  }

  return string_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
vec_fmt(const std::vector<T>& vec)
{
  std::ostringstream oss;
  oss << "[";

  for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); it++)
    oss << *it << ",";
  oss.seekp(oss.tellp() - 1l);
  oss << "]";

  return oss.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
short_vec_fmt(const std::vector<T>& vec)
{
  if (vec.size() == 0)
    return "[]";
  else if (vec.size() == 1)
    return "[" + boost::lexical_cast<std::string>(vec.at(0)) + "]";

  std::ostringstream oss;
  oss << "[";

  // Initial search range
  T first = vec.at(0);
  T last = first;
  for (typename std::vector<T>::const_iterator it = vec.begin() + 1; it != vec.end(); it++) {
    // if *it is contiguous to last, carry on searching
    if ((*it) == (last + 1)) {
      last = *it;
      continue;
    }

    if (first == last)
      // if first and last are the same, this is an isolated number
      oss << first << ",";
    else
      // otherwise it's a range
      oss << first << "-" << last << ",";

    // *it is the first value of the new range
    first = *it;
    last = *it;
  }

  // Process the last range
  if (first == last)
    oss << first << ",";
  else
    oss << first << "-" << last << ",";

  // Is this actually necessary?
  // Replace final "," with a "]"
  oss.seekp(oss.tellp() - 1l);
  oss << "]";

  return oss.str();
}
//-----------------------------------------------------------------------------
} // namespace dunedaq

#endif // TOOLBOX_INCLUDE_DETAIL_UTILITYFUNCTIONS_HXX_