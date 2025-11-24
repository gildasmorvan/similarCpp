"""
Spatial indexing structures for efficient neighbor queries.
"""

from collections import defaultdict
from typing import List, Set
import math


class SpatialHashGrid:
    """
    Spatial hash grid for efficient neighbor queries.
    
    Divides space into cells and indexes objects by their cell location.
    Allows O(1) average-case neighbor queries instead of O(N).
    
    Args:
        cell_size: Size of each grid cell (should be >= typical perception radius)
        width: Width of the space
        height: Height of the space
    """
    
    def __init__(self, cell_size: float, width: float, height: float):
        self.cell_size = cell_size
        self.width = width
        self.height = height
        self.grid = defaultdict(list)
        self.object_cells = {}  # Track which cell each object is in
        
    def clear(self):
        """Clear all objects from the grid."""
        self.grid.clear()
        self.object_cells.clear()
    
    def _get_cell(self, x: float, y: float) -> tuple:
        """Get the cell coordinates for a position."""
        cell_x = int(x / self.cell_size)
        cell_y = int(y / self.cell_size)
        return (cell_x, cell_y)
    
    def insert(self, obj, x: float, y: float):
        """Insert an object at the given position."""
        cell = self._get_cell(x, y)
        self.grid[cell].append(obj)
        self.object_cells[id(obj)] = cell
    
    def update(self, obj, x: float, y: float):
        """Update an object's position."""
        obj_id = id(obj)
        
        # Remove from old cell if exists
        if obj_id in self.object_cells:
            old_cell = self.object_cells[obj_id]
            if obj in self.grid[old_cell]:
                self.grid[old_cell].remove(obj)
        
        # Insert into new cell
        self.insert(obj, x, y)
    
    def query_radius(self, x: float, y: float, radius: float, exclude=None) -> List:
        """
        Query all objects within radius of the given position.
        
        Args:
            x: Query position x
            y: Query position y
            radius: Search radius
            exclude: Optional object to exclude from results
            
        Returns:
            List of (object, distance) tuples
        """
        results = []
        
        # Determine which cells to check
        cell_radius = int(math.ceil(radius / self.cell_size))
        center_cell = self._get_cell(x, y)
        
        # Check all cells within range
        for dx in range(-cell_radius, cell_radius + 1):
            for dy in range(-cell_radius, cell_radius + 1):
                cell = (center_cell[0] + dx, center_cell[1] + dy)
                
                if cell not in self.grid:
                    continue
                
                # Check objects in this cell
                for obj in self.grid[cell]:
                    if obj is exclude:
                        continue
                    
                    # Calculate actual distance
                    obj_x = obj.position.x
                    obj_y = obj.position.y
                    dx_actual = x - obj_x
                    dy_actual = y - obj_y
                    dist_sq = dx_actual * dx_actual + dy_actual * dy_actual
                    
                    if dist_sq <= radius * radius:
                        results.append((obj, math.sqrt(dist_sq)))
        
        return results
    
    def rebuild(self, objects):
        """Rebuild the entire grid from a list of objects."""
        self.clear()
        for obj in objects:
            self.insert(obj, obj.position.x, obj.position.y)
