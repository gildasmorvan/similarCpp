"""Environment management for SIMILAR2Logo simulations.

This module provides a thin wrapper around the C++ Environment implementation.
When C++ bindings are available, all heavy computation is done in C++.
"""

try:
    from ._core.environment import Environment as CppEnvironment
    HAS_CPP_ENV = True
except ImportError:
    HAS_CPP_ENV = False
    CppEnvironment = None

# Simple Pheromone dataclass for Python-side configuration
class Pheromone:
    """
    Pheromone definition with diffusion and evaporation properties.
    
    This is just a lightweight Python representation. The actual pheromone
    data and computation is handled by the C++ Environment.
    """
    
    def __init__(self, identifier, diffusion_coef=0.0, evaporation_coef=0.0,
                 default_value=0.0, min_value=0.0):
        self.identifier = identifier
        self.diffusion_coef = diffusion_coef
        self.evaporation_coef = evaporation_coef
        self.default_value = default_value
        self.min_value = min_value


if HAS_CPP_ENV:
    # Wrap C++ implementation to add Python-side properties
    class Environment(CppEnvironment):
        """
        Python wrapper around C++ Environment that adds convenience properties.
        """
        def __init__(self, width, height, toroidal=False):
            super().__init__(width, height, toroidal)
            # Store dimensions as Python attributes for easy access
            self.width = width
            self.height = height
            self.toroidal = toroidal
            # Track pheromones added (for Python-side bookkeeping)
            self.pheromones = {}
            self.pheromone_grids = {}  # Not used, but kept for compatibility
        
        def add_pheromone(self, identifier, diffusion_coef=0.0, evaporation_coef=0.0,
                         default_value=0.0, min_value=0.0):
            """Add a new pheromone type (delegates to C++)."""
            # Call C++ implementation (ignore return value due to type conversion issues)
            try:
                super().add_pheromone(identifier, diffusion_coef, evaporation_coef,
                                     default_value, min_value)
            except TypeError:
                # C++ returns Pheromone which may not be properly bound
                pass
            
            # Track on Python side
            pheromone = Pheromone(identifier, diffusion_coef, evaporation_coef,
                                 default_value, min_value)
            self.pheromones[identifier] = pheromone
            return pheromone

else:
    # Fallback: minimal Python implementation for when C++ is not available
    import random
    from .tools import Point2D, MathUtil
    import math
    
    class Environment:
        """
        Pure Python fallback implementation of Environment.
        This is only used when C++ bindings are not available.
        """
        
        def __init__(self, width, height, toroidal=False):
            self.width = width
            self.height = height
            self.toroidal = toroidal
            self.pheromones = {}
            self.pheromone_grids = {}
        
        def add_pheromone(self, identifier, diffusion_coef=0.0, evaporation_coef=0.0,
                         default_value=0.0, min_value=0.0):
            """Add a new pheromone type to the environment."""
            pheromone = Pheromone(
                identifier=identifier,
                diffusion_coef=diffusion_coef,
                evaporation_coef=evaporation_coef,
                default_value=default_value,
                min_value=min_value
            )
            self.pheromones[identifier] = pheromone
            
            # Initialize grid
            self.pheromone_grids[identifier] = [
                [default_value for _ in range(self.width)]
                for _ in range(self.height)
            ]
            
            return pheromone
        
        def set_pheromone(self, x, y, identifier, value):
            """Set pheromone value at a specific location."""
            x = int(round(x))
            y = int(round(y))
            
            if self.toroidal:
                x = x % self.width
                y = y % self.height
            else:
                x = max(0, min(x, self.width - 1))
                y = max(0, min(y, self.height - 1))
            
            if identifier in self.pheromone_grids:
                self.pheromone_grids[identifier][y][x] = value
        
        def get_pheromone_value(self, x, y, identifier):
            """Get pheromone value at a specific location."""
            x = int(round(x))
            y = int(round(y))
            
            if self.toroidal:
                x = x % self.width
                y = y % self.height
            else:
                x = max(0, min(x, self.width - 1))
                y = max(0, min(y, self.height - 1))
            
            if identifier in self.pheromone_grids:
                return self.pheromone_grids[identifier][y][x]
            return 0.0
        
        def random_position(self):
            """Generate a random position in the environment."""
            return Point2D(
                random.uniform(0, self.width),
                random.uniform(0, self.height)
            )
        
        def random_heading(self):
            """Generate a random heading (angle in radians)."""
            return random.uniform(0, MathUtil.TWO_PI)
        
        def get_distance(self, pos1, pos2):
            """Calculate distance between two positions."""
            dx = abs(pos1.x - pos2.x)
            dy = abs(pos1.y - pos2.y)
            
            if self.toroidal:
                dx = min(dx, self.width - dx)
                dy = min(dy, self.height - dy)
            
            return math.sqrt(dx * dx + dy * dy)
        
        def get_direction(self, from_pos, to_pos):
            """Calculate direction from one position to another."""
            dx = to_pos.x - from_pos.x
            dy = to_pos.y - from_pos.y
            
            if self.toroidal:
                if abs(dx) > self.width / 2:
                    dx = dx - self.width if dx > 0 else dx + self.width
                if abs(dy) > self.height / 2:
                    dy = dy - self.height if dy > 0 else dy + self.height
            
            return math.atan2(dx, -dy)  # Logo uses y-down, heading 0 = north


__all__ = ['Environment', 'Pheromone', 'HAS_CPP_ENV']
