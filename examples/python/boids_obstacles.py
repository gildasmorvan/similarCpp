"""
Boids with Obstacles - SIMILAR2Logo Python DSL

Flocking boids that avoid obstacles while maintaining cohesion.
Demonstrates obstacle avoidance combined with flocking behavior.

Based on the SIMILAR2Logo Java example.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.dsl import *
import random
import math

# Simulation parameters
GRID_SIZE = 150
NUM_BOIDS = 100
NUM_OBSTACLES = 10
OBSTACLE_RADIUS = 8

# Boid behavior parameters
PERCEPTION_RADIUS = 15.0
SEPARATION_DISTANCE = 5.0
COHESION_WEIGHT = 0.01
ALIGNMENT_WEIGHT = 0.05
SEPARATION_WEIGHT = 0.1
OBSTACLE_AVOIDANCE_WEIGHT = 0.3
MAX_SPEED = 2.0

class Obstacle:
    """Static obstacle in the environment."""
    def __init__(self, x, y, radius):
        self.x = x
        self.y = y
        self.radius = radius


class ObstacleAvoidingBoid(Turtle):
    """Boid that flocks and avoids obstacles."""
    
    def __init__(self, obstacles=None, **kwargs):
        super().__init__(**kwargs)
        self.obstacles = obstacles or []
        self.color = "blue"
        self.speed = 1.5
    
    def decide(self, perception):
        """Flocking behavior with obstacle avoidance."""
        influences = []
        
        # Get nearby boids
        neighbors = perception['nearby_turtles']
        
        if not neighbors:
            # No neighbors - just move forward
            influences.append(self.influence_move_forward(self.speed))
            return influences
        
        # Calculate flocking forces
        cohesion = self._calculate_cohesion(neighbors)
        alignment = self._calculate_alignment(neighbors)
        separation = self._calculate_separation(neighbors)
        obstacle_avoidance = self._calculate_obstacle_avoidance()
        
        # Combine forces
        total_dx = (cohesion[0] * COHESION_WEIGHT +
                   alignment[0] * ALIGNMENT_WEIGHT +
                   separation[0] * SEPARATION_WEIGHT +
                   obstacle_avoidance[0] * OBSTACLE_AVOIDANCE_WEIGHT)
        
        total_dy = (cohesion[1] * COHESION_WEIGHT +
                   alignment[1] * ALIGNMENT_WEIGHT +
                   separation[1] * SEPARATION_WEIGHT +
                   obstacle_avoidance[1] * OBSTACLE_AVOIDANCE_WEIGHT)
        
        # Calculate desired heading
        if total_dx != 0 or total_dy != 0:
            desired_heading = math.atan2(total_dx, -total_dy)
            influences.append(self.influence_turn_towards(desired_heading))
        
        # Move forward
        influences.append(self.influence_move_forward(self.speed))
        
        return influences
    
    def _calculate_cohesion(self, neighbors):
        """Move toward center of mass of neighbors."""
        if not neighbors:
            return (0, 0)
        
        center_x = sum(n['position'].x for n in neighbors) / len(neighbors)
        center_y = sum(n['position'].y for n in neighbors) / len(neighbors)
        
        return (center_x - self.position.x, center_y - self.position.y)
    
    def _calculate_alignment(self, neighbors):
        """Align with average heading of neighbors."""
        if not neighbors:
            return (0, 0)
        
        avg_dx = sum(math.sin(n['heading']) for n in neighbors) / len(neighbors)
        avg_dy = sum(-math.cos(n['heading']) for n in neighbors) / len(neighbors)
        
        return (avg_dx, avg_dy)
    
    def _calculate_separation(self, neighbors):
        """Avoid crowding neighbors."""
        dx, dy = 0, 0
        
        for neighbor in neighbors:
            if neighbor['distance'] < SEPARATION_DISTANCE and neighbor['distance'] > 0:
                # Push away from close neighbors
                diff_x = self.position.x - neighbor['position'].x
                diff_y = self.position.y - neighbor['position'].y
                # Weight by inverse distance
                weight = 1.0 / neighbor['distance']
                dx += diff_x * weight
                dy += diff_y * weight
        
        return (dx, dy)
    
    def _calculate_obstacle_avoidance(self):
        """Avoid nearby obstacles."""
        dx, dy = 0, 0
        
        for obstacle in self.obstacles:
            # Distance to obstacle
            dist_x = self.position.x - obstacle.x
            dist_y = self.position.y - obstacle.y
            distance = math.sqrt(dist_x**2 + dist_y**2)
            
            # If within danger zone
            danger_distance = obstacle.radius + 15
            if distance < danger_distance and distance > 0:
                # Push away from obstacle
                weight = (danger_distance - distance) / danger_distance
                dx += (dist_x / distance) * weight * 10
                dy += (dist_y / distance) * weight * 10
        
        return (dx, dy)


def create_boids_obstacles_simulation():
    """Create and configure the boids with obstacles simulation."""
    
    # Create environment
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
    
    # Create obstacles
    obstacles = []
    for _ in range(NUM_OBSTACLES):
        x = random.uniform(20, GRID_SIZE - 20)
        y = random.uniform(20, GRID_SIZE - 20)
        obstacles.append(Obstacle(x, y, OBSTACLE_RADIUS))
    
    # Create simulation
    sim = Simulation(
        environment=env,
        title="Boids with Obstacles"
    )
    
    # Store obstacles in simulation for visualization
    sim.obstacles = obstacles
    
    # Add boids
    for _ in range(NUM_BOIDS):
        # Ensure boids don't start inside obstacles
        while True:
            pos = env.random_position()
            valid = True
            for obs in obstacles:
                dist = math.sqrt((pos.x - obs.x)**2 + (pos.y - obs.y)**2)
                if dist < obs.radius + 5:
                    valid = False
                    break
            if valid:
                break
        
        boid = ObstacleAvoidingBoid(
            obstacles=obstacles,
            position=pos,
            heading=env.random_heading()
        )
        sim.add_agent(boid)
    
    return sim


def main():
    """Run the boids with obstacles simulation."""
    print("=" * 60)
    print("Boids with Obstacles - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Number of boids: {NUM_BOIDS}")
    print(f"Number of obstacles: {NUM_OBSTACLES}")
    print(f"Perception radius: {PERCEPTION_RADIUS}")
    print("=" * 60)
    print()
    
    sim = create_boids_obstacles_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=1,
        description="""
        # Boids with Obstacles
        
        Flocking boids navigate around obstacles!
        
        - **Blue dots**: Boids that flock together
        - **Gray circles**: Static obstacles
        
        Watch as the boids maintain cohesion while avoiding obstacles.
        They demonstrate:
        - Cohesion (move toward flock center)
        - Alignment (match neighbors' direction)
        - Separation (avoid crowding)
        - Obstacle avoidance (steer clear of barriers)
        """
    )


if __name__ == "__main__":
    main()
