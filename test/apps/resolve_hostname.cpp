/**
 * @file resolve_hostname.cpp Resolver test utility
 *
 * This is a simple test utility that gets the IPs from the given hostname
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/get_ips.hpp"

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    std::cout << "Invalid number of arguments:\nresolve_hostname hostname\n"; // NOLINT
    exit(1);
  }

  auto results = dunedaq::utilities::get_hostname_ips(argv[1]); // NOLINT
  for (auto const& r : results)
    std::cout << r << "\n"; // NOLINT

  if (results.size() > 1)
    return 0;
  else
    return 1;
}
