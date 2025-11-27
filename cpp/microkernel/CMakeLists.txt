cmake_minimum_required(VERSION 3.10)
project(similar_microkernel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*/main.cpp$")

add_library(microkernel ${SOURCES})

add_executable(similar_test src/main.cpp)
target_link_libraries(similar_test microkernel)
