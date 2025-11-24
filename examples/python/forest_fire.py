"""
Forest Fire Simulation - DSL Example

A cellular automaton model of fire spreading through a forest.
Trees can be healthy, burning, or burned. Fire spreads to neighboring trees
based on a propagation probability.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.influences import EmitPheromone, SystemInfluenceRemoveAgent
from similar2logo.tools import Point2D
from dataclasses import dataclass
import random, math

@dataclass
class FireParams:
    """Simulation parameters for the Forest Fire example"""
    initial_tree_density: float = 50.0  # Percentage (reduced from 70)
    fire_propagation_proba: float = 0.58
    combustion_speed: float = 0.1
    grid_size: int = 50  # Reduced from 100 for better performance

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
            
            # Spread fire to neighbors
            # Check 8-connected neighbors (Moore neighborhood)
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    if dx == 0 and dy == 0:
                        continue
                    
                    # Probabilistic fire spread
                    if random.random() < params.fire_propagation_proba:
                        neighbor_x = self.position.x + dx
                        neighbor_y = self.position.y + dy
                        
                        # Emit fire pheromone at neighbor location
                        influences.append(
                            EmitPheromone(
                                agent=self,
                                pheromone_id="fire",
                                position=Point2D(neighbor_x, neighbor_y),
                                amount=1.0
                            )
                        )
            
            # If fully burned, remove from simulation
            if self.burned >= 1.0:
                influences.append(SystemInfluenceRemoveAgent(agent=self))
        
        else:
            # Check if there's fire nearby (via pheromone)
            fire_level = perception['pheromones'].get('fire', 0.0)
            
            if fire_level > 0:
                # Ignite!
                self.burning = True
                self.color = "red"
        
        return influences

def custom_setup(sim):
    """Setup function to initialize the forest"""
    params = FireParams()
    
    # Add fire pheromone (fast evaporation, no diffusion)
    sim.environment.add_pheromone("fire", evaporation_coef=1.0, diffusion_coef=0.0)
    
    # Store params in environment for agents to access
    sim.environment.params = params
    
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
