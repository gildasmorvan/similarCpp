"""
Boids Simulation - DSL Example

A classic flocking model where agents (boids) exhibit alignment, cohesion, and separation.
Implemented using the SIMILAR2Logo Python DSL.
"""

from similar2logo.dsl import Simulation, SimpleTurtle
from similar2logo.influences import *  # not needed directly, but ensures influence classes are loaded
from similar2logo.tools import Point2D
from dataclasses import dataclass
import random, math
import sys

@dataclass
class BoidParams:
    """Simulation parameters for the Boids example"""
    neighbor_radius: float = 10.0
    max_speed: float = 2.0
    max_turn: float = 0.3  # radians per step
    alignment_weight: float = 1.0
    cohesion_weight: float = 0.5
    separation_weight: float = 1.5
    grid_size: int = 100
    num_boids: int = 50

class Boid(SimpleTurtle):
    """A Boid agent extending the base Turtle class"""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.speed = 1.0
        self.color = "blue"
        # Store a reference to the simulation parameters (attached to environment)
        self.params = None

    def decide(self, perception):
        # Retrieve parameters from the environment (attached in the setup callback)
        if self.params is None:
            self.params = perception['environment'].params

        # Gather nearby boids within the configured radius
        neighbors = self.nearby_turtles(perception, radius=self.params.neighbor_radius)
        influences = []
        if neighbors:
            # Alignment: turn towards average heading of neighbors
            influences += self.align_with(neighbors, weight=self.params.alignment_weight)
            # Cohesion: move towards center of mass of neighbors
            influences += self.cohere_with(neighbors, weight=self.params.cohesion_weight)
            # Separation: avoid crowding neighbors
            influences += self.separate_from(neighbors, min_distance=self.params.neighbor_radius/2, weight=self.params.separation_weight)
        # Clamp speed to max_speed
        if self.speed > self.params.max_speed:
            influences.append(self.influence_change_speed(delta_speed=self.params.max_speed - self.speed))
        return influences

def setup_simulation(sim):
    """Configure the environment and attach simulation parameters"""
    # No special pheromones needed for Boids
    params = BoidParams()
    sim.environment.params = params
    # Randomly place boids on the grid
    for _ in range(params.num_boids):
        x = random.uniform(0, params.grid_size)
        y = random.uniform(0, params.grid_size)
        heading = random.uniform(0, 2 * math.pi)
        boid = Boid(position=Point2D(x, y), heading=heading)
        boid._environment = sim.environment
        sim.turtles.append(boid)

def main():
    """Run simulation in console mode"""
    params = BoidParams()
    sim = Simulation("Boids Flocking")
    sim.grid(params.grid_size, params.grid_size, toroidal=True)
    sim.setup(setup_simulation)
    sim.run(max_steps=200)

def main_web(port=8080):
    """Run simulation with web interface"""
    params = BoidParams()
    sim = Simulation("Boids Flocking")
    sim.grid(params.grid_size, params.grid_size, toroidal=True)
    sim.setup(setup_simulation)
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
