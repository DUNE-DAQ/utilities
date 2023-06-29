cmake_minimum_required(VERSION 3.12)
project(utilities VERSION 2.2.1)

find_package(daq-cmake REQUIRED)
daq_setup_environment()

find_package(nlohmann_json REQUIRED)
find_package(logging REQUIRED)
find_package(Boost COMPONENTS iostreams unit_test_framework program_options REQUIRED)
find_package(ers REQUIRED)
find_package(daqdataformats REQUIRED)
find_package(serialization REQUIRED)
##############################################################################
# Schema

#daq_codegen(*.jsonnet TEST TEMPLATES Structs.hpp.j2 Nljs.hpp.j2)

##############################################################################
# Main library

# We don't have a real library, but we want to create a target for
# dependents to be able to depend on
daq_add_library(*.cpp LINK_LIBRARIES nlohmann_json::nlohmann_json logging::logging resolv daqdataformats::daqdataformats serialization::serialization)

##############################################################################

# Test applications

##############################################################################

# Unit tests

daq_add_unit_test(Resolver_test           LINK_LIBRARIES logging::logging utilities)
daq_add_unit_test(ReusableThread_test           LINK_LIBRARIES logging::logging utilities)
daq_add_unit_test(WorkerThread_test           LINK_LIBRARIES logging::logging utilities)
daq_add_unit_test(NamedObject_test        )
daq_add_unit_test(TimeSync_test LINK_LIBRARIES utilities daqdataformats::daqdataformats serialization::serialization)
#daq_add_unit_test(TimestampEstimatorSystem_test  LINK_LIBRARIES utilities)
#daq_add_unit_test(TimestampEstimator_test        LINK_LIBRARIES utilities)

daq_add_application(resolve_hostname resolve_hostname.cpp TEST LINK_LIBRARIES utilities)

daq_install()
