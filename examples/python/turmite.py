"""
Turmite Simulation - Simplified DSL Example

A Turmite is a simple 2D Turing machine that moves on a grid,
changing cell colors and turning based on simple rules.

This example demonstrates the Similar2Logo Python DSL.
"""

from similar2logo import LogoSimulation, Turtle
from similar2logo.tools import Point2D
import random


class TurmiteParams:
    """Simple parameter class - no inheritance needed"""
    def __init__(self):
        self.grid_width = 100
        self.grid_height = 100
        self.num_turmites = 1
        self.max_steps = 10000


class Turmite(Turtle):
    """
    A Turmite follows simple rules:
    - If on white: turn right, paint black, move forward
    - If on black: turn left, paint white, move forward
    """
    
    def __init__(self, x=None, y=None):
        super().__init__(
            position=Point2D(x or random.random() * 100, 
                           y or random.random() * 100),
            heading=random.choice([0, 90, 180, 270]),
            color="red"
        )
        self.marks = set()  # Track black cells
    
    def decide(self, perception):
        """Simple decision logic"""
        influences = []
        
        # Get current cell
        cell = (int(self.position.x), int(self.position.y))
        
        # Check if cell is black
        is_black = cell in self.marks
        
        if is_black:
            # On black: turn left, paint white, move
            influences.append(self.influence_turn(90))  # Turn left
            self.marks.discard(cell)  # Paint white (remove from black set)
        else:
            # On white: turn right, paint black, move
            influences.append(self.influence_turn(-90))  # Turn right
            self.marks.add(cell)  # Paint black
        
        # Move forward
        influences.append(self.influence_move_forward(1))
        
        return influences


def main():
    """Run Turmite simulation"""
    print("=" * 60)
    print("Turmite Simulation - DSL Example")
    print("=" * 60)
    
    # Create simulation parameters
    params = TurmiteParams()
    
    # Create simulation
    sim = LogoSimulation(
        width=params.grid_width,
        height=params.grid_height,
        toroidal=True
    )
    
    # Add turmites
    for _ in range(params.num_turmites):
        turmite = Turmite()
        turmite._environment = sim.environment
        sim.turtles.append(turmite)
    
    # Run for N steps
    print(f"Running simulation for {params.max_steps} steps...")
    for step in range(params.max_steps):
        sim.step()
        if step % 1000 == 0:
            print(f"  Step {step}/{params.max_steps}")
    
    print(f"\\nSimulation complete!")
    print(f"Final cells painted: {len(sim.turtles[0].marks)}")
    print("=" * 60)


def main_web():
    """Run Turmite simulation with web interface"""
    from similar2logo.web import WebSimulation
    
    print("=" * 60)
    print("Turmite Simulation - Web Interface")
    print("=" * 60)
    
    params = TurmiteParams()
    
    # Create simulation
    sim = LogoSimulation(
        width=params.grid_width,
        height=params.grid_height,
        toroidal=True
    )
    
    # Add turmites
    for _ in range(params.num_turmites):
        turmite = Turmite()
        turmite._environment = sim.environment
        sim.turtles.append(turmite)
    
    # Create web interface
    web_sim = WebSimulation(sim, update_rate=60)
    
    print("\\nOpen your browser to: http://localhost:8080")
    print("Watch the Turmite create emergent patterns!")
    print("=" * 60 + "\\n")
    
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
