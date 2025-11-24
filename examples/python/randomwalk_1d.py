"""
Random Walk 1D Simulation - DSL Example

A simple random walk along the X axis.
The turtle randomly moves left or right at each step.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.tools import Point2D
import random

class RandomWalker1D(Turtle):
    """A turtle that performs a 1D random walk along the X axis"""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.color = "red"
        self.speed = 0  # Not used for discrete steps
    
    def decide(self, perception):
        """Randomly move left or right by 1 unit"""
        influences = []
        
        # Random choice: move left (-1) or right (+1)
        dx = -1 if random.random() < 0.5 else 1
        dy = 0  # No vertical movement
        
        # Use change position influence
        new_pos = Point2D(self.position.x + dx, self.position.y + dy)
        influences.append(self.influence_move_to(new_pos))
        
        return influences

def setup(sim):
    """Setup function to create the walker"""
    walker = RandomWalker1D(
        position=Point2D(10, 10),
        heading=0
    )
    walker._environment = sim.environment
    sim.turtles.append(walker)

def main():
    """Run simulation in console mode"""
    sim = Simulation("Random Walk 1D")
    sim.grid(20, 20, toroidal=True)
    sim.setup(setup)
    sim.run(max_steps=3000)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Random Walk 1D")
    sim.grid(20, 20, toroidal=True)
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
