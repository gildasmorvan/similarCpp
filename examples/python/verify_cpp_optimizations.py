"""
Verification script to ensure all Python examples are using optimized C++ code.

This script checks:
1. If C++ bindings are available
2. Which components are using C++ vs pure Python
3. Performance comparison between C++ and Python implementations
"""

import sys
import time

print("=" * 70)
print("SIMILAR2Logo C++ Optimization Verification")
print("=" * 70)

# Check if C++ bindings are available
print("\n1. Checking C++ Bindings Availability...")
print("-" * 70)

try:
    from similar2logo._core import tools as cpp_tools
    print("✓ C++ tools module loaded successfully")
    has_cpp_tools = True
except ImportError as e:
    print(f"✗ C++ tools module not available: {e}")
    has_cpp_tools = False

try:
    from similar2logo._core import microkernel
    print("✓ C++ microkernel module loaded successfully")
    has_cpp_microkernel = True
except ImportError as e:
    print(f"✗ C++ microkernel module not available: {e}")
    has_cpp_microkernel = False

try:
    from similar2logo._core import engine
    print("✓ C++ engine module loaded successfully")
    has_cpp_engine = True
except ImportError as e:
    print(f"✗ C++ engine module not available: {e}")
    has_cpp_engine = False

# Check what the Python library is using
print("\n2. Checking Python Library Configuration...")
print("-" * 70)

from similar2logo.tools import Point2D, MathUtil, HAS_CPP

if HAS_CPP:
    print("✓ similar2logo.tools is using C++ implementation")
    print(f"  - Point2D type: {type(Point2D)}")
    print(f"  - MathUtil type: {type(MathUtil)}")
else:
    print("✗ similar2logo.tools is using pure Python fallback")

# Check available optimizations
print("\n3. Checking Available Optimizations...")
print("-" * 70)

if has_cpp_tools:
    print("C++ Tools Features:")
    print(f"  - Point2D: {hasattr(cpp_tools, 'Point2D')}")
    print(f"  - MathUtil: {hasattr(cpp_tools, 'MathUtil')}")
    print(f"  - FastMath: {hasattr(cpp_tools, 'FastMath')}")
    
    if hasattr(cpp_tools, 'MathUtil'):
        math_util = cpp_tools.MathUtil
        print(f"\n  MathUtil methods:")
        print(f"    - distance: {hasattr(math_util, 'distance')}")
        print(f"    - distance_squared: {hasattr(math_util, 'distance_squared')}")
        print(f"    - toroidal_distance: {hasattr(math_util, 'toroidal_distance')}")
        print(f"    - normalize_angle: {hasattr(math_util, 'normalize_angle')}")
    
    if hasattr(cpp_tools, 'FastMath'):
        fast_math = cpp_tools.FastMath
        print(f"\n  FastMath methods:")
        print(f"    - sin: {hasattr(fast_math, 'sin')}")
        print(f"    - cos: {hasattr(fast_math, 'cos')}")
        print(f"    - sqrt: {hasattr(fast_math, 'sqrt')}")
        print(f"    - fast_inv_sqrt: {hasattr(fast_math, 'fast_inv_sqrt')}")

if has_cpp_engine:
    print("\nC++ Engine Features:")
    from similar2logo._core import engine as cpp_engine
    print(f"  - ISimulationModel: {hasattr(cpp_engine, 'ISimulationModel')}")
    print(f"  - MultiThreadedSimulationEngine: {hasattr(cpp_engine, 'MultiThreadedSimulationEngine')}")

# Performance comparison
print("\n4. Performance Comparison...")
print("-" * 70)

if HAS_CPP:
    # Test Point2D operations
    print("\nPoint2D Distance Calculation (1,000,000 iterations):")
    
    p1 = Point2D(0, 0)
    p2 = Point2D(3, 4)
    
    start = time.time()
    for _ in range(1000000):
        d = p1.distance(p2)
    cpp_time = time.time() - start
    
    print(f"  C++ implementation: {cpp_time:.4f} seconds")
    
    # Test MathUtil operations
    if hasattr(MathUtil, 'toroidal_distance'):
        print("\nMathUtil Toroidal Distance (1,000,000 iterations):")
        
        start = time.time()
        for _ in range(1000000):
            d = MathUtil.toroidal_distance(p1, p2, 100, 100, True, True)
        cpp_time = time.time() - start
        
        print(f"  C++ implementation: {cpp_time:.4f} seconds")
    
    # Test FastMath if available
    if has_cpp_tools and hasattr(cpp_tools, 'FastMath'):
        import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI
        print("\nTrigonometric Functions (1,000,000 iterations):")
        
        # Python math.sin
        start = time.time()
        for i in range(1000000):
            s = sin(i * 0.001)
        py_time = time.time() - start
        print(f"  Python math.sin: {py_time:.4f} seconds")
        
        # C++ FastMath.sin
        fast_math = cpp_tools.FastMath
        start = time.time()
        for i in range(1000000):
            s = fast_sin(i * 0.001)
        cpp_time = time.time() - start
        print(f"  C++ FastMath.sin: {cpp_time:.4f} seconds")
        print(f"  Speedup: {py_time/cpp_time:.2f}x")

else:
    print("✗ C++ not available, skipping performance tests")

# Summary
print("\n" + "=" * 70)
print("SUMMARY")
print("=" * 70)

if HAS_CPP and has_cpp_tools and has_cpp_engine:
    print("✓ All C++ optimizations are active!")
    print("  Your Python examples are using optimized C++ code for:")
    print("  - Point2D operations (distance, rotation, etc.)")
    print("  - MathUtil functions (toroidal distance, angle normalization)")
    print("  - FastMath trigonometric functions (sin, cos)")
    print("  - MultiThreadedSimulationEngine (available for advanced use)")
elif HAS_CPP:
    print("⚠ Partial C++ optimization active")
    print("  Some components are using C++, but not all features are available")
else:
    print("✗ No C++ optimizations active")
    print("  Running in pure Python mode (slower)")
    print("\nTo enable C++ optimizations:")
    print("  1. Build the C++ bindings: cd cpp/build && cmake -DBUILD_PYTHON_BINDINGS=ON .. && make")
    print("  2. Copy the module: cp cpp/build/python/similar2logo/_core.*.so python/similar2logo/")

print("=" * 70)
