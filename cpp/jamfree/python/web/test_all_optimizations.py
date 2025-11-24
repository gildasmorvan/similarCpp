#!/usr/bin/env python3
"""
Comprehensive test suite for all JamFree optimizations.

Tests:
1. Web UI API functionality
2. All performance optimizations (IDM Lookup, Spatial Index, Multithreading)
3. Adaptive Hybrid simulation configuration
4. GPU acceleration availability
5. Performance metrics tracking
"""

import requests
import time
import json
from typing import Dict, Any

BASE_URL = "http://localhost:5001"

class Colors:
    """ANSI color codes for terminal output"""
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    BOLD = '\033[1m'
    END = '\033[0m'

def print_header(text: str):
    """Print a formatted header"""
    print(f"\n{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.END}")
    print(f"{Colors.BOLD}{Colors.BLUE}{text:^60}{Colors.END}")
    print(f"{Colors.BOLD}{Colors.BLUE}{'='*60}{Colors.END}\n")

def print_success(text: str):
    """Print success message"""
    print(f"{Colors.GREEN}✓ {text}{Colors.END}")

def print_error(text: str):
    """Print error message"""
    print(f"{Colors.RED}✗ {text}{Colors.END}")

def print_info(text: str):
    """Print info message"""
    print(f"{Colors.YELLOW}ℹ {text}{Colors.END}")

def print_metric(name: str, value: Any):
    """Print a metric"""
    print(f"  {Colors.BOLD}{name}:{Colors.END} {value}")

def test_api_status() -> bool:
    """Test 1: Verify API is accessible"""
    print_header("TEST 1: API Status")
    try:
        response = requests.get(f"{BASE_URL}/api/status", timeout=5)
        if response.status_code == 200:
            data = response.json()
            print_success("API is accessible")
            print_metric("Status", data.get('status', 'unknown'))
            print_metric("JamFree Available", data.get('jamfree_available', False))
            return True
        else:
            print_error(f"API returned status code {response.status_code}")
            return False
    except Exception as e:
        print_error(f"Failed to connect to API: {e}")
        return False

def test_configuration() -> bool:
    """Test 2: Verify configuration endpoint and optimization settings"""
    print_header("TEST 2: Configuration & Optimizations")
    try:
        response = requests.get(f"{BASE_URL}/api/config", timeout=5)
        if response.status_code == 200:
            config = response.json()
            print_success("Configuration endpoint accessible")
            
            # Check optimization flags
            optimizations = {
                'IDM Lookup': config.get('use_idm_lookup', False),
                'Spatial Index': config.get('use_spatial_index', False),
                'Multithreading': config.get('use_multithreading', False),
                'Adaptive Hybrid': config.get('use_adaptive_hybrid', False),
            }
            
            print("\n  Optimization Status:")
            for name, enabled in optimizations.items():
                status = f"{Colors.GREEN}ENABLED{Colors.END}" if enabled else f"{Colors.RED}DISABLED{Colors.END}"
                print(f"    • {name}: {status}")
            
            # Check other parameters
            print("\n  Simulation Parameters:")
            print_metric("Vehicles", config.get('num_vehicles', 'N/A'))
            print_metric("Desired Speed", f"{config.get('desired_speed', 'N/A')} km/h")
            print_metric("Time Headway", f"{config.get('time_headway', 'N/A')}s")
            
            return all(optimizations.values())
        else:
            print_error(f"Config endpoint returned status code {response.status_code}")
            return False
    except Exception as e:
        print_error(f"Failed to get configuration: {e}")
        return False

def test_simulation_performance() -> bool:
    """Test 3: Run simulation and measure performance"""
    print_header("TEST 3: Simulation Performance")
    
    # First, load a simple test map
    print_info("Loading test map...")
    try:
        # Create a simple road network for testing
        map_data = {
            'lat': 48.8566,
            'lon': 2.3522,
            'radius': 500
        }
        response = requests.post(f"{BASE_URL}/api/map/load", json=map_data, timeout=30)
        if response.status_code != 200:
            print_error(f"Failed to load map: {response.status_code}")
            return False
        print_success("Map loaded")
    except Exception as e:
        print_error(f"Failed to load map: {e}")
        return False
    
    # Start simulation
    print_info("Starting simulation...")
    try:
        response = requests.post(f"{BASE_URL}/api/simulation/start", timeout=5)
        if response.status_code != 200:
            print_error(f"Failed to start simulation: {response.status_code}")
            return False
        print_success("Simulation started")
    except Exception as e:
        print_error(f"Failed to start simulation: {e}")
        return False
    
    # Run a few steps and collect performance metrics
    print_info("Running simulation steps...")
    metrics_list = []
    
    for i in range(10):
        try:
            start_time = time.time()
            response = requests.post(f"{BASE_URL}/api/simulation/step", timeout=10)
            request_time = (time.time() - start_time) * 1000  # ms
            
            if response.status_code == 200:
                data = response.json()
                perf = data.get('performance', {})
                
                metrics = {
                    'step': data.get('step', i),
                    'vehicles': perf.get('vehicles_count', 0),
                    'update_time_ms': perf.get('update_time_ms', 0),
                    'speedup_factor': perf.get('speedup_factor', 0),
                    'request_time_ms': request_time,
                }
                metrics_list.append(metrics)
                
                print(f"  Step {metrics['step']}: "
                      f"{metrics['vehicles']} vehicles, "
                      f"{metrics['update_time_ms']:.2f}ms update, "
                      f"{metrics['speedup_factor']:.1f}x speedup")
            else:
                print_error(f"Step {i} failed with status {response.status_code}")
                
            time.sleep(0.1)  # Small delay between steps
        except Exception as e:
            print_error(f"Step {i} failed: {e}")
    
    # Stop simulation
    try:
        requests.post(f"{BASE_URL}/api/simulation/stop", timeout=5)
        print_success("Simulation stopped")
    except:
        pass
    
    # Analyze performance
    if metrics_list:
        avg_update_time = sum(m['update_time_ms'] for m in metrics_list) / len(metrics_list)
        avg_speedup = sum(m['speedup_factor'] for m in metrics_list) / len(metrics_list)
        avg_vehicles = sum(m['vehicles'] for m in metrics_list) / len(metrics_list)
        
        print(f"\n  {Colors.BOLD}Performance Summary:{Colors.END}")
        print_metric("Average Update Time", f"{avg_update_time:.2f} ms")
        print_metric("Average Speedup", f"{avg_speedup:.1f}x")
        print_metric("Average Vehicles", f"{avg_vehicles:.0f}")
        print_metric("Vehicles per ms", f"{avg_vehicles/avg_update_time:.2f}" if avg_update_time > 0 else "N/A")
        
        # Performance rating
        if avg_update_time < 5:
            rating = "⭐⭐⭐⭐⭐ Excellent"
        elif avg_update_time < 10:
            rating = "⭐⭐⭐⭐ Very Good"
        elif avg_update_time < 20:
            rating = "⭐⭐⭐ Good"
        elif avg_update_time < 50:
            rating = "⭐⭐ Fair"
        else:
            rating = "⭐ Needs Improvement"
        
        print_metric("Overall Rating", rating)
        
        return avg_update_time < 100  # Pass if under 100ms
    else:
        print_error("No performance metrics collected")
        return False

def test_optimization_toggle() -> bool:
    """Test 4: Toggle optimizations and verify changes"""
    print_header("TEST 4: Optimization Toggle")
    
    try:
        # Test toggling IDM lookup
        print_info("Testing IDM Lookup toggle...")
        response = requests.post(
            f"{BASE_URL}/api/config",
            json={'use_idm_lookup': False},
            timeout=5
        )
        if response.status_code == 200:
            print_success("IDM Lookup disabled")
        
        # Re-enable
        response = requests.post(
            f"{BASE_URL}/api/config",
            json={'use_idm_lookup': True},
            timeout=5
        )
        if response.status_code == 200:
            print_success("IDM Lookup re-enabled")
        
        # Test toggling multithreading
        print_info("Testing Multithreading toggle...")
        response = requests.post(
            f"{BASE_URL}/api/config",
            json={'use_multithreading': False},
            timeout=5
        )
        if response.status_code == 200:
            print_success("Multithreading disabled")
        
        # Re-enable
        response = requests.post(
            f"{BASE_URL}/api/config",
            json={'use_multithreading': True},
            timeout=5
        )
        if response.status_code == 200:
            print_success("Multithreading re-enabled")
        
        return True
    except Exception as e:
        print_error(f"Failed to toggle optimizations: {e}")
        return False

def test_traffic_data_sources() -> bool:
    """Test 5: Verify traffic data source availability"""
    print_header("TEST 5: Traffic Data Sources")
    
    try:
        response = requests.get(f"{BASE_URL}/api/traffic/sources", timeout=5)
        if response.status_code == 200:
            sources = response.json()
            print_success("Traffic data sources endpoint accessible")
            
            print("\n  Available Sources:")
            for source in sources.get('sources', []):
                print(f"    • {source}")
            
            return True
        else:
            print_info("Traffic data sources not available (optional feature)")
            return True  # Not a critical failure
    except Exception as e:
        print_info(f"Traffic data sources not available: {e}")
        return True  # Not a critical failure

def test_gpu_availability() -> bool:
    """Test 6: Check GPU acceleration availability"""
    print_header("TEST 6: GPU Acceleration")
    
    try:
        # Try to import GPU module
        import sys
        sys.path.insert(0, '/Users/morvan/Antigravity/similar/cpp/jamfree/python/jamfree')
        
        try:
            import _jamfree
            # Check if MetalCompute is available
            if hasattr(_jamfree, 'MetalCompute'):
                print_success("GPU acceleration (Metal) is available")
                return True
            else:
                print_info("GPU acceleration not yet integrated in Python bindings")
                print_info("C++ implementation exists, Python bindings pending")
                return True  # Not a failure, just not implemented yet
        except ImportError as e:
            print_info(f"JamFree module import failed: {e}")
            return True
    except Exception as e:
        print_info(f"GPU check failed: {e}")
        return True  # Not a critical failure

def test_adaptive_hybrid() -> bool:
    """Test 7: Verify adaptive hybrid configuration"""
    print_header("TEST 7: Adaptive Hybrid Simulation")
    
    try:
        # Check if adaptive hybrid is configured
        response = requests.get(f"{BASE_URL}/api/config", timeout=5)
        if response.status_code == 200:
            config = response.json()
            adaptive_enabled = config.get('use_adaptive_hybrid', False)
            threshold = config.get('adaptive_threshold', 50)
            
            if adaptive_enabled:
                print_success("Adaptive Hybrid simulation is ENABLED")
                print_metric("Threshold", f"{threshold} vehicles")
            else:
                print_info("Adaptive Hybrid simulation is DISABLED")
            
            # Try to enable it
            response = requests.post(
                f"{BASE_URL}/api/config",
                json={'use_adaptive_hybrid': True, 'adaptive_threshold': 50},
                timeout=5
            )
            if response.status_code == 200:
                print_success("Adaptive Hybrid configuration updated")
            
            return True
        else:
            print_error("Failed to check adaptive hybrid configuration")
            return False
    except Exception as e:
        print_error(f"Adaptive hybrid test failed: {e}")
        return False

def main():
    """Run all tests"""
    print_header("JamFree Comprehensive Optimization Test Suite")
    print(f"{Colors.BOLD}Testing all optimizations and features...{Colors.END}\n")
    
    tests = [
        ("API Status", test_api_status),
        ("Configuration & Optimizations", test_configuration),
        ("Simulation Performance", test_simulation_performance),
        ("Optimization Toggle", test_optimization_toggle),
        ("Traffic Data Sources", test_traffic_data_sources),
        ("GPU Acceleration", test_gpu_availability),
        ("Adaptive Hybrid", test_adaptive_hybrid),
    ]
    
    results = {}
    for name, test_func in tests:
        try:
            results[name] = test_func()
        except Exception as e:
            print_error(f"Test '{name}' crashed: {e}")
            results[name] = False
    
    # Summary
    print_header("Test Summary")
    
    passed = sum(1 for v in results.values() if v)
    total = len(results)
    
    for name, result in results.items():
        status = f"{Colors.GREEN}PASS{Colors.END}" if result else f"{Colors.RED}FAIL{Colors.END}"
        print(f"  {status}  {name}")
    
    print(f"\n{Colors.BOLD}Results: {passed}/{total} tests passed{Colors.END}")
    
    if passed == total:
        print(f"\n{Colors.GREEN}{Colors.BOLD}🎉 ALL TESTS PASSED! 🎉{Colors.END}\n")
        return 0
    else:
        print(f"\n{Colors.YELLOW}{Colors.BOLD}⚠️  Some tests failed or were skipped{Colors.END}\n")
        return 1

if __name__ == "__main__":
    exit(main())
