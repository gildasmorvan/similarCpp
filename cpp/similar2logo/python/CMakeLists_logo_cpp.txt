cmake_minimum_required(VERSION 3.15)
project(similar2logo_cpp_bindings)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find Python and pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../microkernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../extendedkernel/include
)

# Source files for Logo C++ implementation
set(LOGO_CPP_SOURCES
    ../src/kernel/agents/LogoAgent.cpp
    ../src/kernel/model/LogoSimulationModel.cpp
    ../src/kernel/tools/FastMath.cpp
    ../src/kernel/environment/Environment.cpp
    ../src/kernel/reaction/Reaction.cpp
    ../src/kernel/influences/ChangePosition.cpp
    ../src/kernel/influences/ChangeDirection.cpp
    ../src/kernel/influences/ChangeSpeed.cpp
    ../src/kernel/influences/Stop.cpp
    ../src/kernel/influences/EmitPheromone.cpp
)

# Link with microkernel and extendedkernel libraries
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../microkernel/build
    ${CMAKE_CURRENT_SOURCE_DIR}/../../extendedkernel/build
)

# Create the Python module for C++ Logo engine
pybind11_add_module(_core bindings_logo_cpp.cpp ${LOGO_CPP_SOURCES})

target_link_libraries(_core PRIVATE
    microkernel
    extendedkernel
)

# Installation - install to the python package directory
install(TARGETS _core
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../python/similar2logo
)
