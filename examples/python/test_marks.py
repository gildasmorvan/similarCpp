"""
Test script to verify marks are displayed in the web UI.
"""

from similar2logo.dsl import Simulation
from similar2logo.model import Turtle
from similar2logo.tools import Point2D
import random

class MarkTurtle(Turtle):
    """A turtle that drops marks"""

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.color = "blue"
        self.speed = 1.0

    def decide(self, perception):
        influences = []

        # Drop a mark at current position
        influences.append(self.influence_drop_mark("test_mark", "info"))

        # Move randomly
        influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        influences.append(self.influence_move_forward(2.0))

        return influences

def setup(sim):
    """Setup function to create mark turtles"""
    for i in range(5):
        turtle = MarkTurtle(
            position=Point2D(
                random.uniform(10, 90),
                random.uniform(10, 90)
            ),
            heading=random.uniform(0, 2 * 3.14159)
        )
        sim.turtles.append(turtle)

def main():
    """Run simulation in console mode"""
    sim = Simulation("Mark Test")
    sim.grid(100, 100, toroidal=True)
    sim.setup(setup)
    sim.run(max_steps=100)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Mark Test")
    sim.grid(100, 100, toroidal=True)
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
