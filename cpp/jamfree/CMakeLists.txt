cmake_minimum_required(VERSION 3.10)
project(JamFree VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Source files
set(JAMFREE_SOURCES
    kernel/src/model/Lane.cpp
)

# Example executables
add_executable(highway_example
    examples/highway_example.cpp
    ${JAMFREE_SOURCES}
)

add_executable(lane_changing_example
    examples/lane_changing_example.cpp
    ${JAMFREE_SOURCES}
)

# Set output directory
set_target_properties(highway_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(lane_changing_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS highway_example lane_changing_example
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY kernel/include/
    DESTINATION include/jamfree/kernel
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY microscopic/include/
    DESTINATION include/jamfree/microscopic
    FILES_MATCHING PATTERN "*.h"
)

# Print configuration
message(STATUS "JamFree C++ Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
