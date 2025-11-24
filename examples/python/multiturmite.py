"""
Multi-Turmite Simulation - SIMILAR2Logo Python DSL

Multiple Langton's Ants (turmites) creating complex emergent patterns.
Each turmite follows simple rules but creates intricate highways.

Based on the SIMILAR2Logo Java example.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.dsl import *
import random

# Simulation parameters
GRID_SIZE = 150
NUM_TURMITES = 5

# Colors for the grid
COLOR_WHITE = 0
COLOR_BLACK = 1

class Turmite(Turtle):
    """Langton's Ant - follows simple rules to create complex patterns."""
    
    def __init__(self, grid, **kwargs):
        super().__init__(**kwargs)
        self.grid = grid  # Reference to shared grid
        self.color = "red"
        self.speed = 1.0
        # Each turmite gets a unique color for visualization
        colors = ["red", "blue", "green", "yellow", "purple", "orange", "pink"]
        self.color = colors[hash(id(self)) % len(colors)]
    
    def decide(self, perception):
        """Langton's Ant rules:
        - At a white square, turn 90° right, flip color, move forward
        - At a black square, turn 90° left, flip color, move forward
        """
        influences = []
        
        # Get current cell
        x = int(self.position.x) % self.grid.width
        y = int(self.position.y) % self.grid.height
        
        # Get cell state
        cell_state = self.grid.get_cell(x, y)
        
        # Apply Langton's Ant rules
        import math
        if cell_state == COLOR_WHITE:
            # Turn right 90 degrees
            influences.append(self.influence_turn(math.pi / 2))
            # Flip cell to black
            self.grid.set_cell(x, y, COLOR_BLACK)
        else:
            # Turn left 90 degrees
            influences.append(self.influence_turn(-math.pi / 2))
            # Flip cell to white
            self.grid.set_cell(x, y, COLOR_WHITE)
        
        # Move forward one cell
        influences.append(self.influence_move_forward(1.0))
        
        return influences


class TurmiteGrid:
    """Grid to track cell states for turmites."""
    
    def __init__(self, width, height):
        self.width = width
        self.height = height
        # Initialize all cells to white
        self.cells = [[COLOR_WHITE for _ in range(width)] for _ in range(height)]
    
    def get_cell(self, x, y):
        """Get cell state at (x, y)."""
        x = int(x) % self.width
        y = int(y) % self.height
        return self.cells[y][x]
    
    def set_cell(self, x, y, value):
        """Set cell state at (x, y)."""
        x = int(x) % self.width
        y = int(y) % self.height
        self.cells[y][x] = value


def create_multiturmite_simulation():
    """Create and configure the multi-turmite simulation."""
    
    # Create environment
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
    
    # Create shared grid
    grid = TurmiteGrid(GRID_SIZE, GRID_SIZE)
    
    # Create simulation
    sim = Simulation(
        environment=env,
        title="Multi-Turmite (Langton's Ants)"
    )
    
    # Store grid for visualization
    sim.turmite_grid = grid
    
    # Add turmites at different starting positions
    for i in range(NUM_TURMITES):
        # Spread them out across the grid
        x = (GRID_SIZE / (NUM_TURMITES + 1)) * (i + 1)
        y = GRID_SIZE / 2
        
        from similar2logo.tools import Point2D
        import math
        turmite = Turmite(
            grid=grid,
            position=Point2D(x, y),
            heading=random.choice([0, math.pi/2, math.pi, -math.pi/2])
        )
        sim.add_agent(turmite)
    
    return sim


def main():
    """Run the multi-turmite simulation."""
    print("=" * 60)
    print("Multi-Turmite Simulation - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Number of turmites: {NUM_TURMITES}")
    print()
    print("Langton's Ant Rules:")
    print("  - On white: turn right, flip to black, move")
    print("  - On black: turn left, flip to white, move")
    print("=" * 60)
    print()
    
    sim = create_multiturmite_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=5,  # Speed up to see patterns emerge
        description="""
        # Multi-Turmite (Langton's Ants)
        
        Multiple Langton's Ants creating emergent patterns!
        
        Each colored dot is a turmite following simple rules:
        - On white cell: turn right, flip to black, move
        - On black cell: turn left, flip to white, move
        
        Despite the simplicity, they create complex "highways" after ~10,000 steps.
        
        **Fun fact**: A single ant creates chaos for ~10,000 steps, then builds a highway!
        With multiple ants, the patterns become even more intricate.
        """
    )


if __name__ == "__main__":
    main()
