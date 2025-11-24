#!/usr/bin/env python3
"""
Boids Flocking Simulation - DSL - Multiprocessing Backend

This example demonstrates using the multiprocessing backend for true parallelism.
The 'process' backend bypasses Python's GIL, providing genuine parallel execution
of agent decisions across multiple CPU cores.

Usage:
    python boids_multiprocessing.py
"""

import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../python'))

from dataclasses import dataclass
from similar2logo.dsl import Simulation, Turtle
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


@dataclass
class BoidParams:
    """Simulation parameters - clean and simple"""
    repulsion_distance: float = 1.0
    orientation_distance: float = 2.0
    attraction_distance: float = 5.0
    repulsion_weight: float = 1.0
    orientation_weight: float = 1.0
    attraction_weight: float = 0.1
    max_speed: float = 2.0
    num_agents: int = 1000
    grid_size: int = 100


class Boid(Turtle):
    """A boid agent with flocking behavior"""
    
    def __init__(self, position=None, heading=0.0, params=None, **kwargs):
        super().__init__(position, heading, color="blue", **kwargs)
        self.params = params or BoidParams()
        self.speed = 1.0
    
    def decide(self, perception):
        """Implement Reynolds' boids flocking rules"""
        influences = []
        nearby = perception['nearby_turtles']
        
        if not nearby:
            # Random walk if alone
            influences.extend(self.random_walk_influences(turn_amount=0.1))
            return influences
        
        # Separation: Avoid crowding neighbors
        separation_force = [0.0, 0.0]
        separation_count = 0
        
        # Alignment: Steer towards average heading
        avg_heading_x = 0.0
        avg_heading_y = 0.0
        alignment_count = 0
        
        # Cohesion: Steer towards average position
        avg_position = [0.0, 0.0]
        cohesion_count = 0
        
        for neighbor_info in nearby:
            distance = neighbor_info['distance']
            neighbor = neighbor_info['turtle']
            
            # Separation
            if distance < self.params.repulsion_distance and distance > 0:
                dx = self.position.x - neighbor.position.x
                dy = self.position.y - neighbor.position.y
                separation_force[0] += dx / distance
                separation_force[1] += dy / distance
                separation_count += 1
            
            # Alignment
            if distance < self.params.orientation_distance:
                import math
                avg_heading_x += sin(neighbor.heading)
                avg_heading_y += -cos(neighbor.heading)
                alignment_count += 1
            
            # Cohesion
            if distance < self.params.attraction_distance:
                avg_position[0] += neighbor.position.x
                avg_position[1] += neighbor.position.y
                cohesion_count += 1
        
        # Apply separation
        if separation_count > 0:
            import math
            angle = atan2(separation_force[0], -separation_force[1])
            influences.append(
                self.influence_turn_towards(angle)
            )
        
        # Apply alignment
        if alignment_count > 0:
            import math
            avg_heading_x /= alignment_count
            avg_heading_y /= alignment_count
            target_heading = atan2(avg_heading_x, avg_heading_y)
            
            # Turn towards average heading
            angle_diff = target_heading - self.heading
            # Normalize to [-pi, pi]
            while angle_diff > PI:
                angle_diff -= TWO_PI
            while angle_diff < -PI:
                angle_diff += TWO_PI
            
            influences.append(
                self.influence_turn(angle_diff * self.params.orientation_weight * 0.1)
            )
        
        # Apply cohesion
        if cohesion_count > 0:
            import math
            avg_position[0] /= cohesion_count
            avg_position[1] /= cohesion_count
            
            dx = avg_position[0] - self.position.x
            dy = avg_position[1] - self.position.y
            target_heading = atan2(dx, -dy)
            
            angle_diff = target_heading - self.heading
            while angle_diff > PI:
                angle_diff -= TWO_PI
            while angle_diff < -PI:
                angle_diff += TWO_PI
            
            influences.append(
                self.influence_turn(angle_diff * self.params.attraction_weight * 0.1)
            )
        
        # Move forward
        influences.append(self.influence_move_forward(self.speed))
        
        return influences


if __name__ == "__main__":
    print("=" * 60)
    print("Boids Flocking - DSL - Multiprocessing Backend")
    print("=" * 60)
    print()
    print("This simulation uses multiprocessing for true parallelism.")
    print("Agent decisions are executed across multiple CPU cores,")
    print("bypassing Python's Global Interpreter Lock (GIL).")
    print()
    print("Open your browser to: http://localhost:8080")
    print("=" * 60)
    print()
    
    params = BoidParams()
    
    # Create and run simulation with multiprocessing backend
    (Simulation("Boids Flocking - Multiprocessing")
     .grid(params.grid_size, params.grid_size, toroidal=True)
     .parallel(backend='process', num_workers=4)  # Use 4 processes
     .agents(Boid, params.num_agents, params=params)
     .run_web(port=8080))
