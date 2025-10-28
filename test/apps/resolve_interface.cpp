/**
 * @file resolve_interface.cpp Resolver test utility
 *
 * This is a simple test utility that gets the IP for a given interface
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
    std::cout << "Invalid number of arguments:\nresolve_interface ifname\n"; // NOLINT
    exit(1);
  }

  try {
    auto result = dunedaq::utilities::get_interface_ip(argv[1]); // NOLINT
    std::cout << result << std::endl;
    if (result.size() > 0)
      return 0;
  } catch (ers::Issue& expt) {
    std::cout << "Encountered ERS issue while resolving interface IP: " << expt.what();
  }
  return 1;
}
