"""
Passive Turtle Simulation - DSL Example

A turtle that moves with initial velocity and acceleration but makes no decisions.
Demonstrates the physics engine handling movement without agent intervention.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.tools import Point2D
import math

class PassiveTurtle(Turtle):
    """A turtle that makes no decisions - purely passive movement"""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.color = "green"
    
    def decide(self, perception):
        """Does nothing - turtle is passive"""
        return []

def setup(sim):
    """Setup function to create the passive turtle"""
    turtle = PassiveTurtle(
        position=Point2D(50, 50),
        heading=math.pi / 4,  # 45 degrees
    )
    turtle.speed = 1.0
    turtle.acceleration = 0.01
    turtle._environment = sim.environment
    sim.turtles.append(turtle)

def main():
    """Run simulation in console mode"""
    sim = Simulation("Passive Turtle")
    sim.grid(100, 100, toroidal=True)
    sim.setup(setup)
    sim.run(max_steps=1000)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Passive Turtle")
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
