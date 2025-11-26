"""
Multi-Turmite Simulation - SIMILAR2Logo Python DSL

Multiple Langton's Ants (turmites) creating complex emergent patterns.
Each turmite follows simple rules but creates intricate highways.

Based on the SIMILAR2Logo Java example.
"""

from similar2logo.dsl import Simulation
from similar2logo.model import Turtle
from similar2logo.tools import Point2D
import random
import math

# Simulation parameters
GRID_SIZE = 150
NUM_TURMITES = 5

class Turmite(Turtle):
    """Langton's Ant - follows simple rules to create complex patterns."""

    def __init__(self, position=None, heading=0.0, **kwargs):
        super().__init__(position=position, heading=heading, **kwargs)
        self.speed = 1.0
        # Each turmite gets a unique color for visualization
        colors = ["red", "blue", "green", "yellow", "purple", "orange", "pink"]
        self.color = colors[hash(id(self)) % len(colors)]

    def decide(self, perception):
        """Langton's Ant rules (matching Java implementation):
        - If no marks at current position: turn RIGHT and DROP a mark
        - If marks exist at current position: turn LEFT and REMOVE a mark
        """
        influences = []

        # Check if there are marks at current position
        marks_at_position = perception['marks']

        if not marks_at_position:
            # No marks (white cell): turn RIGHT and drop mark (make black)
            influences.append(self.influence_turn(math.pi / 2))
            influences.append(self.influence_drop_mark("black"))
        else:
            # Marks exist (black cell): turn LEFT and remove mark (make white)
            influences.append(self.influence_turn(-math.pi / 2))
            # Remove the first mark we find by its content
            if marks_at_position:
                first_mark = marks_at_position[0]
                influences.append(self.influence_remove_mark(first_mark['content']))

        # Movement happens through physics simulation, not explicit influences

        return influences


def setup(sim):
    """Setup function to create turmites"""
    # Add turmites at different starting positions
    for i in range(NUM_TURMITES):
        # Spread them out across the grid
        x = (GRID_SIZE / (NUM_TURMITES + 1)) * (i + 1)
        y = GRID_SIZE / 2

        turmite = Turmite(
            position=Point2D(x, y),
            heading=random.choice([0, math.pi/2, math.pi, -math.pi/2])
        )
        sim.turtles.append(turmite)


def main():
    """Run simulation in console mode"""
    sim = Simulation("Multi-Turmite (Langton's Ants)")
    sim.grid(GRID_SIZE, GRID_SIZE, toroidal=True)
    sim.setup(setup)
    sim.run(max_steps=1000)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Multi-Turmite (Langton's Ants)")
    sim.grid(GRID_SIZE, GRID_SIZE, toroidal=True)
    sim.setup(setup)
    sim.run_web(port=port)


if __name__ == "__main__":
    import sys
    if "--web" in sys.argv:
        port = 8080
        if "--port" in sys.argv:
            port_idx = sys.argv.index("--port") + 1
            if port_idx < len(sys.argv):
                port = int(sys.argv[port_idx])
        main_web(port=port)
    else:
        main()
