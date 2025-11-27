"""
JamFree: Traffic simulation library with microscopic and macroscopic models

This package provides Python bindings for the JamFree C++ traffic simulation library.
"""

from ._jamfree import (
    # Core classes
    Point2D,
    Lane,
    Road,
    Vehicle,
    
    # Car-following models (microscopic)
    IDM,
    IDMPlus,
    
    # Lane-changing models
    MOBIL,
    LaneChangeDirection,
    
    # Macroscopic models
    LWR,
    CTM,
    MicroMacroBridge,
    AggregateStats,
    
    # Adaptive Hybrid Simulation
    AdaptiveSimulator,
    AdaptiveSimulatorConfig,
    AdaptiveSimulatorStatistics,
    SimulationMode,
    
    # OSM (OpenStreetMap) support
    RoadNetwork,
    OSMParser,
    
    # Utility functions
    kmh_to_ms,
    ms_to_kmh,

    # Multithreaded Engine
    ISimulationEngine,
    MultithreadedSimulationEngine,
    ISimulationModel,
    TrafficSimulationModel,
    SimulationTimeStamp,
    VehicleAgent,
    VehiclePublicLocalStateMicro,
    VehiclePrivateLocalStateMicro,
    
    # Perception and Decision Models
    VehiclePerceptionModelMicro,
    VehicleDecisionModelMicro,
    IDecisionMicroSubmodel,
    ForwardAccelerationDMS,
    LaneChangeDMS,
    ConjunctionDMS,
    SubsumptionDMS,
)

__version__ = '0.1.0'
__author__ = 'Gildas Morvan'

__all__ = [
    # Core
    'Point2D',
    'Lane',
    'Road',
    'Vehicle',
    # Microscopic
    'IDM',
    'IDMPlus',
    'MOBIL',
    'LaneChangeDirection',
    # Macroscopic
    'LWR',
    'CTM',
    'MicroMacroBridge',
    'AggregateStats',
    # Hybrid
    'AdaptiveSimulator',
    'AdaptiveSimulatorConfig',
    'AdaptiveSimulatorStatistics',
    'SimulationMode',
    # OSM
    'RoadNetwork',
    'OSMParser',
    # Utils
    'kmh_to_ms',
    'ms_to_kmh',
    # Multithreaded Engine
    'ISimulationEngine',
    'MultithreadedSimulationEngine',
    'ISimulationModel',
    'TrafficSimulationModel',
    'SimulationTimeStamp',
    'VehicleAgent',
    'VehiclePublicLocalStateMicro',
    'VehiclePrivateLocalStateMicro',
    # Perception and Decision
    'VehiclePerceptionModelMicro',
    'VehicleDecisionModelMicro',
    'IDecisionMicroSubmodel',
    'ForwardAccelerationDMS',
    'LaneChangeDMS',
    'ConjunctionDMS',
    'SubsumptionDMS',
    # Metal GPU
    'MetalCompute',
    'GPUVehicleState',
    'GPUIDMParams',
    'is_metal_available',
]

# Try to import Metal module (macOS only)
try:
    from ._metal import (
        MetalCompute,
        GPUVehicleState,
        GPUIDMParams,
        is_metal_available,
    )
except ImportError:
    # Stub for non-macOS or if build failed
    class MetalCompute:
        @staticmethod
        def is_available():
            return False
            
    class GPUVehicleState: pass
    class GPUIDMParams: pass
    
    def is_metal_available():
        return False
