cmake_minimum_required(VERSION 3.10)
project(JamFree VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories - add SIMILAR kernel includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../microkernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../extendedkernel/include
)

# Add library search paths for SIMILAR libraries
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
    ${CMAKE_CURRENT_SOURCE_DIR}/../microkernel/build
    ${CMAKE_CURRENT_SOURCE_DIR}/../extendedkernel/build
)

# Find SIMILAR libraries
find_library(SIMILAR_MICROKERNEL_LIB 
    NAMES similar_microkernel microkernel
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../build ${CMAKE_CURRENT_SOURCE_DIR}/../microkernel/build
    NO_DEFAULT_PATH
)

find_library(SIMILAR_EXTENDEDKERNEL_LIB 
    NAMES similar_extendedkernel extendedkernel
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../build ${CMAKE_CURRENT_SOURCE_DIR}/../extendedkernel/build
    NO_DEFAULT_PATH
)

# Print library paths for debugging
message(STATUS "SIMILAR Microkernel library: ${SIMILAR_MICROKERNEL_LIB}")
message(STATUS "SIMILAR ExtendedKernel library: ${SIMILAR_EXTENDEDKERNEL_LIB}")

# Kernel source files
set(JAMFREE_KERNEL_SOURCES
    kernel/src/model/Lane.cpp
    kernel/src/agents/VehicleAgent.cpp
    kernel/src/simulation/SimulationEngine.cpp
    kernel/src/simulation/TrafficSimulationModel.cpp
    kernel/src/simulation/TrafficLevel.cpp
    kernel/src/reaction/TrafficReactionModel.cpp
)

# Microscopic source files
set(JAMFREE_MICROSCOPIC_SOURCES
    microscopic/src/agents/VehiclePerceivedDataMicro.cpp
    microscopic/src/agents/VehiclePublicLocalStateMicro.cpp
    microscopic/src/agents/VehiclePrivateLocalStateMicro.cpp
    microscopic/src/perception/VehiclePerceptionModelMicro.cpp
    microscopic/src/decision/VehicleDecisionModelMicro.cpp
    microscopic/src/reaction/MicroscopicReactionModel.cpp
)

# Hybrid source files
set(JAMFREE_HYBRID_SOURCES
    hybrid/src/AdaptiveSimulator.cpp
)

# Realdata source files
set(JAMFREE_REALDATA_SOURCES
    realdata/src/OSMParser.cpp
)

# JamFree library
add_library(jamfree STATIC
    ${JAMFREE_KERNEL_SOURCES}
    ${JAMFREE_MICROSCOPIC_SOURCES}
    ${JAMFREE_HYBRID_SOURCES}
    ${JAMFREE_REALDATA_SOURCES}
)

# Link against SIMILAR libraries
target_link_libraries(jamfree 
    ${SIMILAR_EXTENDEDKERNEL_LIB}
    ${SIMILAR_MICROKERNEL_LIB}
)

# Example executables
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/highway_example.cpp")
    add_executable(highway_example
        examples/highway_example.cpp
    )
    target_link_libraries(highway_example jamfree ${SIMILAR_EXTENDEDKERNEL_LIB} ${SIMILAR_MICROKERNEL_LIB})
    
    set_target_properties(highway_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/lane_changing_example.cpp")
    add_executable(lane_changing_example
        examples/lane_changing_example.cpp
    )
    target_link_libraries(lane_changing_example jamfree ${SIMILAR_EXTENDEDKERNEL_LIB} ${SIMILAR_MICROKERNEL_LIB})
    
    set_target_properties(lane_changing_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Complete simulation example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/complete_simulation_example.cpp")
    add_executable(complete_simulation_example
        examples/complete_simulation_example.cpp
    )
    target_link_libraries(complete_simulation_example jamfree ${SIMILAR_EXTENDEDKERNEL_LIB} ${SIMILAR_MICROKERNEL_LIB})
    
    set_target_properties(complete_simulation_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Highway simulation example (New Refactored API)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/highway_simulation.cpp")
    add_executable(highway_simulation
        examples/highway_simulation.cpp
    )
    target_link_libraries(highway_simulation jamfree ${SIMILAR_EXTENDEDKERNEL_LIB} ${SIMILAR_MICROKERNEL_LIB})
    
    set_target_properties(highway_simulation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# ========================================================================
# Python Bindings
# ========================================================================
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

if(pybind11_FOUND)
    message(STATUS "Found pybind11: ${pybind11_VERSION}")
    
    # Create the Python extension module
    # The module name must match the one in bindings.cpp (PYBIND11_MODULE(_jamfree, m))
    pybind11_add_module(_jamfree
        python/src/bindings.cpp
    )
    
    # Link against JamFree and SIMILAR libraries
    target_link_libraries(_jamfree PRIVATE 
        jamfree
        ${SIMILAR_EXTENDEDKERNEL_LIB}
        ${SIMILAR_MICROKERNEL_LIB}
    )
    
    # Set output directory for the shared library
    # We want it in build/ so it can be imported
    set_target_properties(_jamfree PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    
    message(STATUS "Configured _jamfree Python module")
else()
    message(WARNING "pybind11 not found! Python bindings will not be built.")
endif()

# Comprehensive unit tests
add_executable(jamfree_basic_test tests/basic_jamfree_tests.cpp)

target_link_libraries(jamfree_basic_test
    jamfree_kernel
    jamfree_microscopic
    jamfree_macroscopic
    ${SIMILAR_MICROKERNEL_LIB}
    ${SIMILAR_EXTENDEDKERNEL_LIB}
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
