"""
Turmite Simulation - Simplified DSL Example

A Turmite is a simple 2D Turing machine that moves on a grid,
changing cell colors and turning based on simple rules.

This example demonstrates the Similar2Logo Python DSL.
"""

from similar2logo import LogoSimulation, Turtle
from similar2logo.tools import Point2D
from similar2logo._core import SimpleMark
from similar2logo._core.influences import DropMark, RemoveMark
from similar2logo._core import SimulationTimeStamp
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
    
    Uses DropMark influences to paint cells, making marks visible in the Web UI.
    """
    
    def __init__(self, x=None, y=None, heading=None, environment=None):
        import math
        super().__init__(
            position=Point2D(x or random.random() * 100, 
                           y or random.random() * 100),
            heading=heading if heading is not None else random.choice([0, math.pi/2, math.pi, 3*math.pi/2]),  # 0, 90, 180, 270 degrees in radians
            color="red"
        )
        self._environment = environment
    
    def _check_mark_at_position(self, x, y):
        """Check if there's a mark at the given position"""
        if self._environment is None:
            return False
        
        marks_grid = self._environment.get_marks()
        if 0 <= x < len(marks_grid) and 0 <= y < len(marks_grid[0]):
            return len(marks_grid[x][y]) > 0
        return False
    
    def decide(self, perception):
        """
        Turmite decision logic matching the Jython implementation.
        
        Rules:
        - If no marks (white cell): turn RIGHT (+90°), drop mark
        - If marks present (black cell): turn LEFT (-90°), remove mark
        
        Note: Movement happens automatically via the simulation engine,
        so we don't create movement influences here.
        """
        import math
        influences = []
        
        # Get current cell
        cell_x = int(self.position.x)
        cell_y = int(self.position.y)
        
        # Check if cell has a mark (is black)
        is_black = self._check_mark_at_position(cell_x, cell_y)
        
        # Create timestamps for influences
        t_now = SimulationTimeStamp(0)
        t_next = SimulationTimeStamp(1)
        
        if is_black:
            # On black cell: turn LEFT, remove mark
            influences.append(self.influence_turn(-math.pi / 2))  # Turn left 90 degrees
            
            # Remove the mark
            if self._environment:
                marks_at_pos = self._environment.get_marks()[cell_x][cell_y]
                if marks_at_pos:
                    mark_to_remove = next(iter(marks_at_pos))
                    influences.append(RemoveMark(t_now, t_next, mark_to_remove))
        else:
            # On white cell: turn RIGHT, drop mark
            influences.append(self.influence_turn(math.pi / 2))  # Turn right 90 degrees
            
            # Drop a mark
            from similar2logo._core import Point2D as CppPoint2D
            cpp_pos = CppPoint2D(cell_x + 0.5, cell_y + 0.5)
            mark = SimpleMark(cpp_pos, "turmite_mark")
            influences.append(DropMark(t_now, t_next, mark))
        
        return influences


def main():
    """Run Turmite simulation"""
    print("=" * 60)
    print("Turmite Simulation - DSL Example")
    print("=" * 60)
    
    # Create simulation parameters
    params = TurmiteParams()
    
    # Create environment
    from similar2logo.environment import Environment
    env = Environment(
        width=params.grid_width,
        height=params.grid_height,
        toroidal=True
    )
    
    # Create simulation
    sim = LogoSimulation(environment=env)
    
    # Add turmites
    for _ in range(params.num_turmites):
        turmite = Turmite(environment=sim.environment)
        sim.turtles.append(turmite)
    
    # Run for N steps
    print(f"Running simulation for {params.max_steps} steps...")
    for step in range(params.max_steps):
        sim.step()
        if step % 1000 == 0:
            print(f"  Step {step}/{params.max_steps}")
    
    print(f"\nSimulation complete!")
    
    # Count marks in the environment
    marks_grid = sim.environment.get_marks()
    total_marks = sum(len(marks_grid[x][y]) for x in range(len(marks_grid)) 
                      for y in range(len(marks_grid[0])))
    print(f"Final cells painted: {total_marks}")
    print("=" * 60)


def main_web():
    """Run Turmite simulation with web interface"""
    from similar2logo.web import WebSimulation
    from similar2logo.environment import Environment
    
    print("=" * 60)
    print("Turmite Simulation - Web Interface")
    print("=" * 60)
    
    params = TurmiteParams()
    
    # Create environment
    env = Environment(
        width=params.grid_width,
        height=params.grid_height,
        toroidal=True
    )
    
    # Create simulation
    sim = LogoSimulation(environment=env)
    
    # Add turmites
    for _ in range(params.num_turmites):
        turmite = Turmite(environment=sim.environment)
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
