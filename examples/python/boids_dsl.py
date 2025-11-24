"""
Boids Simulation - DSL Example

A classic flocking model where agents (boids) exhibit alignment, cohesion, and separation.
Implemented using the SIMILAR2Logo Python DSL, matching the original Java implementation.
"""

from similar2logo.dsl import Simulation, SimpleTurtle
from similar2logo.influences import *
from similar2logo.tools import Point2D
from similar2logo.fastmath import sin, cos, atan2, PI
from dataclasses import dataclass
import random
import sys

@dataclass
class BoidParams:
    """Simulation parameters matching the Java implementation"""
    # Distances
    repulsion_distance: float = 1.0
    orientation_distance: float = 2.0
    attraction_distance: float = 4.0
    
    # Weights
    repulsion_weight: float = 10.0
    orientation_weight: float = 50.0
    attraction_weight: float = 0.1
    
    # Speed and Angle limits
    max_angle: float = PI / 4  # Max turn per step
    max_initial_speed: float = 2.0
    min_initial_speed: float = 1.0
    
    # Perception
    perception_angle: float = PI  # Full 360 degrees (PI radians half-angle)
    
    # Environment
    grid_size: int = 100
    num_boids: int = 100

class Boid(SimpleTurtle):
    """A Boid agent implementing the exact logic from BoidDecisionModel.java"""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.speed = random.uniform(1.0, 2.0)
        self.color = "blue"
        self.params = None

    def decide(self, perception):
        if self.params is None:
            self.params = perception['environment'].params

        # Filter neighbors based on max distance
        max_dist = max(self.params.repulsion_distance, 
                      self.params.orientation_distance, 
                      self.params.attraction_distance)
        
        # Get neighbors within max distance
        neighbors = self.nearby_turtles(perception, radius=max_dist)
        
        if not neighbors:
            return [self.influence_move_forward(self.speed)]

        # MeanAngle calculation variables
        mean_dx = 0.0
        mean_dy = 0.0
        
        orientation_speed_sum = 0.0
        count_orientation = 0
        
        for neighbor in neighbors:
            # Filter by perception angle if not full circle
            if self.params.perception_angle < PI:
                # Calculate angle to neighbor
                dx = neighbor.position.x - self.position.x
                dy = neighbor.position.y - self.position.y
                angle_to = PI/2 - atan2(dy, dx)
                
                angle_diff = abs(angle_to - self.heading)
                while angle_diff > PI:
                    angle_diff -= 2 * PI
                angle_diff = abs(angle_diff)
                
                if angle_diff > self.params.perception_angle:
                    continue

            dist = neighbor.position.distance(self.position)
            
            # Calculate angle to neighbor (Logo heading)
            dx = neighbor.position.x - self.position.x
            dy = neighbor.position.y - self.position.y
            angle_to_neighbor = PI/2 - atan2(dy, dx)
            
            angle_contribution = 0.0
            weight = 0.0
            
            if dist <= self.params.repulsion_distance:
                # Repulsion: Turn AWAY from neighbor
                angle_contribution = self.heading - angle_to_neighbor
                weight = self.params.repulsion_weight
                
            elif dist <= self.params.orientation_distance:
                # Orientation: Align with neighbor
                angle_contribution = neighbor.heading - self.heading
                weight = self.params.orientation_weight
                
                orientation_speed_sum += (neighbor.speed - self.speed)
                count_orientation += 1
                
            elif dist <= self.params.attraction_distance:
                # Attraction: Turn TOWARDS neighbor
                angle_contribution = angle_to_neighbor - self.heading
                weight = self.params.attraction_weight
            
            # Add to mean angle vector
            if weight > 0:
                mean_dx += weight * cos(angle_contribution)
                mean_dy += weight * sin(angle_contribution)

        influences = []
        
        # 1. Change Direction
        if abs(mean_dx) > 0.001 or abs(mean_dy) > 0.001:
            # Calculate mean delta angle
            mean_delta = atan2(mean_dy, mean_dx)
            
            # Clamp to max_angle
            if mean_delta > self.params.max_angle:
                mean_delta = self.params.max_angle
            elif mean_delta < -self.params.max_angle:
                mean_delta = -self.params.max_angle
                
            influences.append(self.influence_turn(mean_delta))

        # 2. Change Speed
        if count_orientation > 0:
            avg_speed_diff = orientation_speed_sum / count_orientation
            influences.append(self.influence_change_speed(avg_speed_diff))

        # 3. Move Forward (always move)
        influences.append(self.influence_move_forward(self.speed))
        
        return influences

def setup_simulation(sim):
    params = BoidParams()
    sim.environment.params = params
    
    # Set default turtle class for dynamic addition (Web UI)
    sim._turtle_class = Boid
    
    for _ in range(params.num_boids):
        x = random.uniform(0, params.grid_size)
        y = random.uniform(0, params.grid_size)
        heading = random.uniform(0, 2 * PI)
        
        boid = Boid(position=Point2D(x, y), heading=heading)
        boid.speed = random.uniform(params.min_initial_speed, params.max_initial_speed)
        boid._environment = sim.environment
        sim.turtles.append(boid)

def main():
    params = BoidParams()
    sim = Simulation("Boids Flocking (Java Port)")
    sim.grid(params.grid_size, params.grid_size, toroidal=True)
    sim.setup(setup_simulation)
    sim.run(max_steps=200)

def main_web(port=8080):
    params = BoidParams()
    sim = Simulation("Boids Flocking (Java Port)")
    sim.grid(params.grid_size, params.grid_size, toroidal=True)
    sim.setup(setup_simulation)
    sim.run_web(port=port)

if __name__ == "__main__":
    if "--web" in sys.argv:
        port = 8080
        if "--port" in sys.argv:
            try:
                port_idx = sys.argv.index("--port") + 1
                if port_idx < len(sys.argv):
                    port = int(sys.argv[port_idx])
            except ValueError:
                pass
        main_web(port)
    else:
        main()
