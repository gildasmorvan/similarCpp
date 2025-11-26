"""
Forest Fire Simulation - DSL Example

A cellular automaton model of fire spreading through a forest.
Trees can be healthy, burning, or burned. Fire spreads to neighboring trees
based on a propagation probability.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.influences import SystemInfluenceRemoveAgent
from similar2logo.tools import Point2D
from dataclasses import dataclass
import random, math

@dataclass
class FireParams:
    """Simulation parameters for the Forest Fire example"""
    initial_tree_density: float = 70.0  # Percentage (matches Java implementation)
    fire_propagation_proba: float = 0.1  # Matches Java implementation
    combustion_speed: float = 0.1
    grid_size: int = 50

class Tree(Turtle):
    """A tree agent that can burn and spread fire"""
    
    def __init__(self, burning=False, **kwargs):
        super().__init__(**kwargs)
        self.burning = burning
        self.burned = 0.0  # 0.0 = healthy, 1.0 = fully burned
        self.speed = 0  # Trees don't move
        self.color = "red" if burning else "green"
    
    def decide(self, perception):
        """Decide whether to burn, spread fire, or die"""
        influences = []

        # Get parameters from environment
        params = perception['environment'].params

        if self.burning:
            # Update burn status
            self.burned += params.combustion_speed

            # Change color as burning progresses
            if self.burned < 0.5:
                self.color = "orange"
            else:
                self.color = "black"

            # Spread fire to neighbors directly (like Java implementation)
            # Check 8-connected neighbors (Moore neighborhood)
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    if dx == 0 and dy == 0:
                        continue

                    neighbor_x = int(self.position.x + dx)
                    neighbor_y = int(self.position.y + dy)

                    # Check bounds
                    if (0 <= neighbor_x < params.grid_size and
                        0 <= neighbor_y < params.grid_size):

                        # Look up neighbor directly using position mapping
                        neighbor_pos = (neighbor_x, neighbor_y)
                        neighbor = self._environment.position_to_tree.get(neighbor_pos)
                        if (neighbor is not None and
                            not neighbor.burning and
                            random.random() < params.fire_propagation_proba):
                            # Ignite neighbor directly
                            neighbor.burning = True
                            neighbor.color = "red"

            # If fully burned, remove from simulation
            if self.burned >= 1.0:
                influences.append(SystemInfluenceRemoveAgent(agent=self))

        return influences

def custom_setup(sim):
    """Setup function to initialize the forest"""
    params = FireParams()

    # Store params in environment for agents to access
    sim.environment.params = params
    # Store simulation reference for direct turtle access
    sim.environment._simulation = sim

    # Create position-to-turtle mapping for efficient neighbor lookup
    sim.environment.position_to_tree = {}
    for turtle in sim.turtles:
        if isinstance(turtle, Tree):
            pos_key = (int(turtle.position.x), int(turtle.position.y))
            sim.environment.position_to_tree[pos_key] = turtle

    # Optimize perception building for forest fire - no need for expensive spatial queries
    # Override the perception building to skip nearby turtles and pheromones
    original_build_perception = sim._build_perception
    def optimized_build_perception(turtle):
        """Optimized perception building for forest fire - only environment needed"""
        return {
            'environment': sim.environment,
            'position': turtle.position,
            'heading': turtle.heading,
            'speed': turtle.speed,
            'time': sim.current_step,
            'pheromones': {},  # Empty - no pheromones used
            'nearby_turtles': [],  # Empty - no nearby turtles needed
            'marks': []  # Empty - no marks used
        }
    sim._build_perception = optimized_build_perception
    
    # Generate trees based on density
    for x in range(params.grid_size):
        for y in range(params.grid_size):
            if random.random() * 100 < params.initial_tree_density:
                # Trees in first column start burning
                is_burning = (x == 0)
                
                tree = Tree(
                    burning=is_burning,
                    position=Point2D(x, y),
                    heading=0
                )
                tree._environment = sim.environment
                sim.turtles.append(tree)

def main():
    """Run simulation in console mode"""
    sim = Simulation("Forest Fire")
    sim.grid(50, 50, toroidal=False)
    sim.parallel(None)  # Disable parallelism for better performance with many agents
    sim.setup(custom_setup)
    sim.run(max_steps=100)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Forest Fire")
    sim.grid(50, 50, toroidal=False)
    sim.parallel(None)  # Disable parallelism for better performance with many agents
    sim.setup(custom_setup)
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
