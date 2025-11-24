"""
Fast Math utilities with C++ acceleration.

This module provides optimized mathematical functions using C++ FastMath
when available, with automatic fallback to Python's math module.
"""

import math as _stdlib_math

# Try to import C++ FastMath
_use_cpp_fastmath = False
try:
    from similar2logo._core.tools import FastMath as _CppFastMath
    _use_cpp_fastmath = True
except ImportError:
    _CppFastMath = None


class FastMath:
    """
    Fast mathematical functions using C++ lookup tables when available.
    
    This class provides optimized implementations of common math functions:
    - sin/cos: Lookup table-based (C++) or standard (Python)
    - sqrt: Hardware-accelerated
    - atan2: Standard implementation
    
    Usage:
        >>> from similar2logo.fastmath import FastMath
        >>> angle = FastMath.sin(1.5)
        >>> distance = FastMath.sqrt(dx*dx + dy*dy)
    """
    
    # Constants
    PI = _stdlib_math.pi
    TWO_PI = 2 * _stdlib_math.pi
    HALF_PI = _stdlib_math.pi / 2
    
    @staticmethod
    def sin(radians: float) -> float:
        """
        Fast sine function.
        
        Uses C++ lookup table if available, otherwise Python's math.sin.
        
        Args:
            radians: Angle in radians
            
        Returns:
            Sine value
        """
        if _use_cpp_fastmath:
            return _CppFastMath.sin(radians)
        return _stdlib_math.sin(radians)
    
    @staticmethod
    def cos(radians: float) -> float:
        """
        Fast cosine function.
        
        Uses C++ lookup table if available, otherwise Python's math.cos.
        
        Args:
            radians: Angle in radians
            
        Returns:
            Cosine value
        """
        if _use_cpp_fastmath:
            return _CppFastMath.cos(radians)
        return _stdlib_math.cos(radians)
    
    @staticmethod
    def sqrt(value: float) -> float:
        """
        Fast square root.
        
        Args:
            value: Non-negative number
            
        Returns:
            Square root
        """
        if _use_cpp_fastmath:
            return _CppFastMath.sqrt(value)
        return _stdlib_math.sqrt(value)
    
    @staticmethod
    def atan2(y: float, x: float) -> float:
        """
        Arc tangent of y/x in radians.
        
        Args:
            y: Y coordinate
            x: X coordinate
            
        Returns:
            Angle in radians
        """
        return _stdlib_math.atan2(y, x)
    
    @staticmethod
    def normalize_angle(angle: float) -> float:
        """
        Normalize angle to [-PI, PI].
        
        Args:
            angle: Angle in radians
            
        Returns:
            Normalized angle
        """
        while angle > FastMath.PI:
            angle -= FastMath.TWO_PI
        while angle < -FastMath.PI:
            angle += FastMath.TWO_PI
        return angle
    
    @staticmethod
    def degrees_to_radians(degrees: float) -> float:
        """Convert degrees to radians."""
        return degrees * FastMath.PI / 180.0
    
    @staticmethod
    def radians_to_degrees(radians: float) -> float:
        """Convert radians to degrees."""
        return radians * 180.0 / FastMath.PI
    
    @staticmethod
    def is_using_cpp() -> bool:
        """Check if C++ FastMath is being used."""
        return _use_cpp_fastmath


# Convenience exports
sin = FastMath.sin
cos = FastMath.cos
sqrt = FastMath.sqrt
atan2 = FastMath.atan2
normalize_angle = FastMath.normalize_angle
PI = FastMath.PI
TWO_PI = FastMath.TWO_PI
HALF_PI = FastMath.HALF_PI


def print_status():
    """Print which math implementation is being used."""
    if _use_cpp_fastmath:
        print("✅ Using C++ FastMath (optimized lookup tables)")
    else:
        print("⚠️  Using Python math module (C++ FastMath not available)")


if __name__ == "__main__":
    print("=" * 60)
    print("FastMath Module Status")
    print("=" * 60)
    print_status()
    print()
    print("Testing functions:")
    print(f"  sin(PI/2) = {sin(PI/2)}")
    print(f"  cos(0) = {cos(0)}")
    print(f"  sqrt(16) = {sqrt(16)}")
    print(f"  atan2(1, 1) = {atan2(1, 1)}")
    print("=" * 60)
