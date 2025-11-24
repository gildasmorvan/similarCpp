cmake_minimum_required(VERSION 3.15)
project(extendedkernel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../microkernel/include
)

# Find all source files
file(GLOB_RECURSE EXTKERNEL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Create library
add_library(extendedkernel STATIC ${EXTKERNEL_SOURCES})

# Link with microkernel
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../microkernel/build)
target_link_libraries(extendedkernel PUBLIC microkernel)

# Installation
install(TARGETS extendedkernel
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
