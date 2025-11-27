#!/usr/bin/env python3
"""
Test script for JamFree engine manager.

Tests the simplified engine initialization without full vehicle simulation.
"""

import sys
sys.path.insert(0, '/Users/morvan/Antigravity/similar/cpp/jamfree/python')

import jamfree
from web.engine_manager import SimulationEngineManager

def test_engine_initialization():
    """Test basic engine initialization."""
    print("=" * 60)
    print("JamFree Engine Manager Test")
    print("=" * 60)
    
    # Create minimal config
    config = {
        'num_threads': 2,
        'num_vehicles': 10,
    }
    
    # Create dummy network (simplified)
    # In real usage, this would be loaded from OSM
    class DummyNetwork:
        min_lat = 48.8
        max_lat = 48.9
        min_lon = 2.3
        max_lon = 2.4
    
    network = DummyNetwork()
    
    print("\n1. Creating SimulationEngineManager...")
    manager = SimulationEngineManager(network, config)
    print(f"   ✓ Manager created")
    print(f"   Center: ({manager.center_lat:.4f}, {manager.center_lon:.4f})")
    
    print("\n2. Initializing engine...")
    success = manager.initialize()
    print(f"   {'✓' if success else '✗'} Initialization {'succeeded' if success else 'failed'}")
    print(f"   Engine initialized: {manager.is_initialized()}")
    
    if success:
        print("\n3. Testing simulation step...")
        result = manager.step()
        if 'error' in result:
            print(f"   ✗ Step failed: {result['error']}")
        else:
            print(f"   ✓ Step {result['step']} completed")
            print(f"   Vehicles: {len(result['vehicles'])}")
            print(f"   Update time: {result['performance']['update_time_ms']:.2f}ms")
    
    print("\n4. Cleaning up...")
    manager.stop()
    print("   ✓ Cleanup complete")
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Engine initialization: {'✓ PASS' if success else '✗ FAIL'}")
    print("\nNote: Full vehicle simulation requires:")
    print("  - VehiclePerceptionModelMicro bindings")
    print("  - VehicleDecisionModelMicro bindings")
    print("  - TrafficSimulationModel integration")
    print("  - Probe registration API")
    print("=" * 60)

if __name__ == '__main__':
    test_engine_initialization()
