"""
Geometric and mathematical utilities for SIMILAR2Logo.
"""

import math

# Try to use C++ implementation for performance
try:
    from ._core import tools as _cpp_tools
    Point2D = _cpp_tools.Point2D
    MathUtil = _cpp_tools.MathUtil
    HAS_CPP = True
except ImportError:
    HAS_CPP = False
    
    class Point2D:
        """
        2D point/vector class.
        
        Pure Python implementation that works without C++ backend.
        """
        
        def __init__(self, x=0.0, y=0.0):
            self.x = float(x)
            self.y = float(y)
        
        def distance(self, other):
            """Calculate Euclidean distance to another point."""
            dx = self.x - other.x
            dy = self.y - other.y
            return math.sqrt(dx * dx + dy * dy)
        
        def distance_squared(self, other):
            """
            Calculate squared distance to another point.
            Faster than distance() when you only need to compare distances.
            """
            dx = self.x - other.x
            dy = self.y - other.y
            return dx * dx + dy * dy
        
        def magnitude(self):
            """Calculate magnitude (length) of the vector."""
            return math.sqrt(self.x * self.x + self.y * self.y)
        
        def normalize(self):
            """Return a normalized (unit length) vector."""
            mag = self.magnitude()
            if mag == 0:
                return Point2D(0, 0)
            return Point2D(self.x / mag, self.y / mag)
            
        def __add__(self, other):
            return Point2D(self.x + other.x, self.y + other.y)
            
        def __sub__(self, other):
            return Point2D(self.x - other.x, self.y - other.y)
            
        def __mul__(self, scalar):
            return Point2D(self.x * scalar, self.y * scalar)
            
        def __truediv__(self, scalar):
            return Point2D(self.x / scalar, self.y / scalar)
            
        def __repr__(self):
            return f"Point2D({self.x}, {self.y})"
        
        def to_tuple(self):
            """Convert to tuple (x, y)."""
            return (self.x, self.y)
        
        def to_list(self):
            """Convert to [x, y] list."""
            return [self.x, self.y]


    class MathUtil:
        """Mathematical utilities for Logo simulations."""
        
        PI = math.pi
        TWO_PI = 2 * math.pi
        HALF_PI = math.pi / 2
        
        @staticmethod
        def normalize_angle(angle):
            """Normalize angle to [-PI, PI] range."""
            while angle > math.pi:
                angle -= 2 * math.pi
            while angle < -math.pi:
                angle += 2 * math.pi
            return angle
        
        @staticmethod
        def angle_difference(angle1, angle2):
            """Compute the smallest difference between two angles."""
            diff = angle1 - angle2
            return MathUtil.normalize_angle(diff)
        
        @staticmethod
        def to_degrees(radians):
            """Convert radians to degrees."""
            return math.degrees(radians)
        
        @staticmethod
        def to_radians(degrees):
            """Convert degrees to radians."""
            return math.radians(degrees)
