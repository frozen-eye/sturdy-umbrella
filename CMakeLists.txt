cmake_minimum_required(VERSION 3.10)
project(cpplogger)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cpplogger main.cpp)

# Enable testing functionality
enable_testing()

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add test cpp file
add_executable(runUnitTests tests/test_logger.cpp) # replace with your test cpp files

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests ${GTEST_BOTH_LIBRARIES})

add_test(NAME test COMMAND runUnitTests)
