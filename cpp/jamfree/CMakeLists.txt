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

# Kernel source files
set(JAMFREE_KERNEL_SOURCES
    kernel/src/model/Lane.cpp
    kernel/src/agents/VehicleAgent.cpp
    kernel/src/simulation/SimulationEngine.cpp
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

# JamFree library
add_library(jamfree STATIC
    ${JAMFREE_KERNEL_SOURCES}
    ${JAMFREE_MICROSCOPIC_SOURCES}
)

# Link against SIMILAR libraries
target_link_libraries(jamfree 
    similar_extendedkernel 
    similar_microkernel
)

# Example executables
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/highway_example.cpp")
    add_executable(highway_example
        examples/highway_example.cpp
    )
    target_link_libraries(highway_example jamfree similar_extendedkernel similar_microkernel)
    
    set_target_properties(highway_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/lane_changing_example.cpp")
    add_executable(lane_changing_example
        examples/lane_changing_example.cpp
    )
    target_link_libraries(lane_changing_example jamfree similar_extendedkernel similar_microkernel)
    
    set_target_properties(lane_changing_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Complete simulation example
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/complete_simulation_example.cpp")
    add_executable(complete_simulation_example
        examples/complete_simulation_example.cpp
    )
    target_link_libraries(complete_simulation_example jamfree similar_extendedkernel similar_microkernel)
    
    set_target_properties(complete_simulation_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Highway simulation example (New Refactored API)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/highway_simulation.cpp")
    add_executable(highway_simulation
        examples/highway_simulation.cpp
    )
    target_link_libraries(highway_simulation jamfree similar_extendedkernel similar_microkernel)
    
    set_target_properties(highway_simulation PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

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
