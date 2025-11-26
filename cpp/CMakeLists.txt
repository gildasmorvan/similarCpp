cmake_minimum_required(VERSION 3.10)
project(SIMILAR_CPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set version
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# Build type configuration
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Compiler flags for different build types
if(MSVC)
  add_compile_options(/W4)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/O2 /DNDEBUG)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /DDEBUG)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG -march=native)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -DDEBUG)
  endif()
endif()

# Include directories
include_directories(microkernel/include)
include_directories(extendedkernel/include)
include_directories(extendedkernel/third_party)  # For cpp-httplib and nlohmann/json

# Microkernel library
file(GLOB_RECURSE MICROKERNEL_SOURCES "microkernel/src/*.cpp")
add_library(similar_microkernel ${MICROKERNEL_SOURCES})

# Extended Kernel library (includes extendedlibs)
file(GLOB_RECURSE EXTENDEDKERNEL_SOURCES 
    "extendedkernel/src/*.cpp"
)
# Exclude example files from library
list(FILTER EXTENDEDKERNEL_SOURCES EXCLUDE REGEX ".*examples/.*")
add_library(similar_extendedkernel STATIC ${EXTENDEDKERNEL_SOURCES})
target_link_libraries(similar_extendedkernel similar_microkernel)

# Link pthread for threading support (required by cpp-httplib)
find_package(Threads REQUIRED)
target_link_libraries(similar_extendedkernel Threads::Threads)

# Similar2Logo library
file(GLOB_RECURSE SIMILAR2LOGO_SOURCES "similar2logo/src/*.cpp")
# Also include headers for now to ensure they are found if we add source files later
# But GLOB_RECURSE only finds files matching the pattern.
# Since similar2logo is currently header-only (mostly), we might not have .cpp files yet?
# Let's check if there are .cpp files.
# If not, we can't add a library with no sources.
# But we can add an INTERFACE library or just add the headers to a target.

# Let's assume we will have sources.
add_library(similar2logo ${SIMILAR2LOGO_SOURCES})
target_link_libraries(similar2logo similar_microkernel)
target_include_directories(similar2logo PUBLIC similar2logo/include)


# Example executables
add_executable(similar_example extendedkernel/examples/simple_example.cpp)
target_link_libraries(similar_example similar_extendedkernel similar_microkernel)

# Predator-Prey example
add_executable(predator_prey extendedkernel/examples/predator_prey_example.cpp)
target_link_libraries(predator_prey similar_extendedkernel similar_microkernel)

# Improved Ecosystem example (using extendedlibs)
add_executable(improved_ecosystem extendedkernel/examples/improved_ecosystem.cpp)
target_link_libraries(improved_ecosystem similar_extendedkernel similar_microkernel)

# ExtendedLibs Complete Demo
add_executable(extendedlibs_demo extendedkernel/examples/extendedlibs_demo.cpp)
target_link_libraries(extendedlibs_demo similar_extendedkernel similar_microkernel)

# Random & End Criterion Demo
add_executable(random_demo extendedkernel/examples/random_demo.cpp)
target_link_libraries(random_demo similar_extendedkernel similar_microkernel)

# Web-Enabled Predator-Prey Example
add_executable(web_predator_prey extendedkernel/examples/web_predator_prey.cpp)
target_link_libraries(web_predator_prey similar_extendedkernel similar_microkernel Threads::Threads)

# Minimal Web Server Demo
add_executable(minimal_web_server extendedkernel/examples/minimal_web_server.cpp)
target_link_libraries(minimal_web_server Threads::Threads)

# Similar2Logo Tests
add_executable(similar2logo_influences_test similar2logo/tests/influences_test.cpp)
target_link_libraries(similar2logo_influences_test similar2logo similar_microkernel)

add_executable(similar2logo_core_test similar2logo/tests/core_tests.cpp)
target_link_libraries(similar2logo_core_test similar2logo similar_microkernel)

add_executable(similar2logo_extended_test similar2logo/tests/extended_kernel_tests.cpp)
target_link_libraries(similar2logo_extended_test similar_extendedkernel similar_microkernel)

# Python bindings (optional)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(similar2logo/python)
endif()

# JamFree traffic simulation (optional)
option(BUILD_JAMFREE "Build JamFree traffic simulation" ON)
if(BUILD_JAMFREE)
    add_subdirectory(jamfree)
endif()
