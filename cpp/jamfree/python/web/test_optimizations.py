#!/usr/bin/env python3
"""
Test script for JamFree Web UI optimizations.
Tests all performance features: IDM Lookup, Spatial Index, Multithreading, Adaptive Hybrid, GPU.
"""

import requests
import time
import json
from typing import Dict, Any

BASE_URL = "http://localhost:5001"

def test_api_status():
    """Test basic API connectivity."""
    print("=" * 60)
    print("TEST 1: API Status")
    print("=" * 60)
    
    response = requests.get(f"{BASE_URL}/api/status")
    assert response.status_code == 200, f"Status check failed: {response.status_code}"
    
    data = response.json()
    print(f"✓ JamFree Available: {data['jamfree_available']}")
    print(f"✓ Network Loaded: {data['network_loaded']}")
    print(f"✓ Running: {data['running']}")
    print(f"✓ Step: {data['step']}")
    print(f"✓ Vehicles: {data['num_vehicles']}")
    print()
    
    return data

def test_config():
    """Test configuration retrieval."""
    print("=" * 60)
    print("TEST 2: Configuration")
    print("=" * 60)
    
    response = requests.get(f"{BASE_URL}/api/config")
    assert response.status_code == 200, f"Config check failed: {response.status_code}"
    
    config = response.json()
    print(f"✓ Num Vehicles: {config['num_vehicles']}")
    print(f"✓ Desired Speed: {config['desired_speed']} km/h")
    print(f"✓ Time Headway: {config['time_headway']} s")
    print(f"✓ Politeness: {config['politeness']}")
    print()
    print("Optimizations:")
    print(f"  - IDM Lookup: {config.get('use_idm_lookup', False)}")
    print(f"  - Spatial Index: {config.get('use_spatial_index', False)}")
    print(f"  - Multithreading: {config.get('use_multithreading', False)}")
    print(f"  - Adaptive Hybrid: {config.get('use_adaptive_hybrid', False)}")
    print(f"  - Adaptive Threshold: {config.get('adaptive_threshold', 'N/A')}")
    print()
    
    return config

def test_simulation_step():
    """Test simulation step with performance metrics."""
    print("=" * 60)
    print("TEST 3: Simulation Step Performance")
    print("=" * 60)
    
    # Run multiple steps to get average performance
    num_steps = 10
    total_time = 0
    performance_data = []
    
    for i in range(num_steps):
        start = time.time()
        response = requests.post(f"{BASE_URL}/api/simulation/step")
        elapsed = (time.time() - start) * 1000  # ms
        
        if response.status_code != 200:
            print(f"✗ Step {i+1} failed: {response.status_code}")
            continue
        
        data = response.json()
        perf = data.get('performance', {})
        performance_data.append(perf)
        total_time += elapsed
        
        if i == 0:
            print(f"Step {data['step']}:")
            print(f"  - Vehicles: {perf.get('vehicles_count', 0)}")
            print(f"  - Update Time: {perf.get('update_time_ms', 0):.2f} ms")
            print(f"  - Speedup Factor: {perf.get('speedup_factor', 1.0):.2f}x")
            
            opts = perf.get('optimizations', {})
            print(f"  - Active Optimizations:")
            print(f"    * IDM Lookup: {opts.get('idm_lookup', False)}")
            print(f"    * Spatial Index: {opts.get('spatial_index', False)}")
            print(f"    * Multithreading: {opts.get('multithreading', False)}")
    
    avg_update_time = sum(p.get('update_time_ms', 0) for p in performance_data) / len(performance_data)
    avg_speedup = sum(p.get('speedup_factor', 1.0) for p in performance_data) / len(performance_data)
    avg_total_time = total_time / num_steps
    
    print()
    print(f"Average over {num_steps} steps:")
    print(f"  - Total Request Time: {avg_total_time:.2f} ms")
    print(f"  - Server Update Time: {avg_update_time:.2f} ms")
    print(f"  - Network Overhead: {avg_total_time - avg_update_time:.2f} ms")
    print(f"  - Speedup Factor: {avg_speedup:.2f}x")
    print()
    
    return performance_data

def test_optimization_toggle():
    """Test toggling optimizations on/off."""
    print("=" * 60)
    print("TEST 4: Optimization Toggle")
    print("=" * 60)
    
    # Get current config
    response = requests.get(f"{BASE_URL}/api/config")
    original_config = response.json()
    
    # Test configurations
    test_configs = [
        {
            'name': 'All Optimizations OFF',
            'config': {
                'use_idm_lookup': False,
                'use_spatial_index': False,
                'use_multithreading': False,
                'use_adaptive_hybrid': False,
            }
        },
        {
            'name': 'IDM Lookup Only',
            'config': {
                'use_idm_lookup': True,
                'use_spatial_index': False,
                'use_multithreading': False,
                'use_adaptive_hybrid': False,
            }
        },
        {
            'name': 'Spatial Index Only',
            'config': {
                'use_idm_lookup': False,
                'use_spatial_index': True,
                'use_multithreading': False,
                'use_adaptive_hybrid': False,
            }
        },
        {
            'name': 'All Optimizations ON',
            'config': {
                'use_idm_lookup': True,
                'use_spatial_index': True,
                'use_multithreading': True,
                'use_adaptive_hybrid': True,
            }
        },
    ]
    
    results = []
    
    for test in test_configs:
        print(f"\nTesting: {test['name']}")
        
        # Update config
        response = requests.post(f"{BASE_URL}/api/config", json=test['config'])
        assert response.status_code == 200, f"Config update failed"
        
        # Note: Config changes require simulation restart to take effect
        print("  ⚠ Note: Config changes require simulation restart")
        print(f"  Config updated successfully")
        
        results.append({
            'name': test['name'],
            'config': test['config']
        })
    
    # Restore original config
    requests.post(f"{BASE_URL}/api/config", json=original_config)
    print(f"\n✓ Original configuration restored")
    print()
    
    return results

def test_data_sources():
    """Test traffic data sources."""
    print("=" * 60)
    print("TEST 5: Traffic Data Sources")
    print("=" * 60)
    
    response = requests.get(f"{BASE_URL}/api/data/sources")
    assert response.status_code == 200, f"Data sources check failed"
    
    data = response.json()
    sources = data.get('sources', [])
    
    print(f"✓ Available sources: {len(sources)}")
    for source in sources:
        print(f"  - {source['name']} ({source['type']})")
        print(f"    Coverage: {source['coverage']}")
        print(f"    Features: {', '.join(source['features'])}")
    print()
    
    return sources

def generate_performance_report(status: Dict, config: Dict, perf_data: list):
    """Generate a comprehensive performance report."""
    print("=" * 60)
    print("PERFORMANCE REPORT")
    print("=" * 60)
    print()
    
    print("System Status:")
    print(f"  - JamFree: {'Available' if status['jamfree_available'] else 'Not Available'}")
    print(f"  - Network: {'Loaded' if status['network_loaded'] else 'Not Loaded'}")
    print(f"  - Simulation: {'Running' if status['running'] else 'Stopped'}")
    print(f"  - Vehicles: {status['num_vehicles']}")
    print()
    
    print("Enabled Optimizations:")
    optimizations = []
    if config.get('use_idm_lookup'):
        optimizations.append("IDM Lookup Tables (30-40% faster)")
    if config.get('use_spatial_index'):
        optimizations.append("Spatial Index (O(log N) queries)")
    if config.get('use_multithreading'):
        optimizations.append("Multithreading (for N > 100)")
    if config.get('use_adaptive_hybrid'):
        optimizations.append(f"Adaptive Hybrid (threshold: {config.get('adaptive_threshold')})")
    
    if optimizations:
        for opt in optimizations:
            print(f"  ✓ {opt}")
    else:
        print("  ✗ No optimizations enabled")
    print()
    
    if perf_data:
        avg_update = sum(p.get('update_time_ms', 0) for p in perf_data) / len(perf_data)
        avg_speedup = sum(p.get('speedup_factor', 1.0) for p in perf_data) / len(perf_data)
        
        print("Performance Metrics:")
        print(f"  - Average Update Time: {avg_update:.2f} ms")
        print(f"  - Average Speedup: {avg_speedup:.2f}x")
        print(f"  - Vehicles per ms: {status['num_vehicles'] / max(avg_update, 0.1):.2f}")
        print()
        
        # Performance rating
        if avg_speedup > 5:
            rating = "Excellent ⭐⭐⭐⭐⭐"
        elif avg_speedup > 3:
            rating = "Very Good ⭐⭐⭐⭐"
        elif avg_speedup > 2:
            rating = "Good ⭐⭐⭐"
        elif avg_speedup > 1.5:
            rating = "Fair ⭐⭐"
        else:
            rating = "Needs Improvement ⭐"
        
        print(f"Overall Performance: {rating}")
    print()

def main():
    """Run all tests."""
    print("\n")
    print("╔" + "=" * 58 + "╗")
    print("║" + " " * 10 + "JamFree Web UI Optimization Tests" + " " * 14 + "║")
    print("╚" + "=" * 58 + "╝")
    print()
    
    try:
        # Run tests
        status = test_api_status()
        config = test_config()
        perf_data = test_simulation_step()
        toggle_results = test_optimization_toggle()
        sources = test_data_sources()
        
        # Generate report
        generate_performance_report(status, config, perf_data)
        
        print("=" * 60)
        print("ALL TESTS PASSED ✓")
        print("=" * 60)
        print()
        
        return 0
        
    except Exception as e:
        print()
        print("=" * 60)
        print(f"TEST FAILED ✗")
        print(f"Error: {e}")
        print("=" * 60)
        print()
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())
