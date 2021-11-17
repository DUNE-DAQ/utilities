cmake_minimum_required(VERSION 3.12)
project(toolbox VERSION 1.0.0)

find_package(daq-cmake REQUIRED)
daq_setup_environment()

find_package(nlohmann_json REQUIRED)
find_package(logging REQUIRED)
find_package(Boost COMPONENTS iostreams REQUIRED)

##############################################################################
# Schema

#daq_codegen(*.jsonnet TEST TEMPLATES Structs.hpp.j2 Nljs.hpp.j2)

##############################################################################
# Main library

# We don't have a real library, but we want to create a target for
# dependents to be able to depend on
daq_add_library(UtilityFunctions.cpp LINK_LIBRARIES nlohmann_json::nlohmann_json logging::logging Boost::iostreams stdc++fs resolv)

##############################################################################

# Test applications

##############################################################################

# Unit tests

daq_add_unit_test(Resolver_test           LINK_LIBRARIES logging::logging toolbox)
daq_add_unit_test(ReusableThread_test           LINK_LIBRARIES logging::logging toolbox)
daq_add_unit_test(ThreadHelper_test           LINK_LIBRARIES logging::logging toolbox)
daq_add_unit_test(UtilityFunctions_test           LINK_LIBRARIES logging::logging toolbox stdc++fs)

daq_install()
