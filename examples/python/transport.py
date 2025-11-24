"""
Transport Simulation - SIMILAR2Logo Python DSL

Vehicles following road networks and avoiding collisions.
Demonstrates path following and traffic simulation.

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
NUM_VEHICLES = 30
VEHICLE_SPEED = 2.0
COLLISION_DISTANCE = 3.0
ROAD_WIDTH = 5

class Road:
    """Represents a road segment."""
    def __init__(self, start_x, start_y, end_x, end_y):
        self.start_x = start_x
        self.start_y = start_y
        self.end_x = end_x
        self.end_y = end_y
        self.angle = math.atan2(end_x - start_x, -(end_y - start_y))
    
    def distance_to_point(self, x, y):
        """Calculate perpendicular distance from point to road."""
        # Vector from start to point
        dx = x - self.start_x
        dy = y - self.start_y
        
        # Vector along road
        road_dx = self.end_x - self.start_x
        road_dy = self.end_y - self.start_y
        road_length = math.sqrt(road_dx**2 + road_dy**2)
        
        if road_length == 0:
            return math.sqrt(dx**2 + dy**2)
        
        # Normalize road vector
        road_dx /= road_length
        road_dy /= road_length
        
        # Project point onto road
        projection = dx * road_dx + dy * road_dy
        projection = max(0, min(road_length, projection))
        
        # Find closest point on road
        closest_x = self.start_x + projection * road_dx
        closest_y = self.start_y + projection * road_dy
        
        # Distance to closest point
        return math.sqrt((x - closest_x)**2 + (y - closest_y)**2)


class Vehicle(Turtle):
    """Vehicle that follows roads and avoids collisions."""
    
    def __init__(self, roads, **kwargs):
        super().__init__(**kwargs)
        self.roads = roads
        self.color = "blue"
        self.speed = VEHICLE_SPEED
        self.target_road = random.choice(roads) if roads else None
    
    def decide(self, perception):
        """Follow roads and avoid collisions."""
        influences = []
        
        # Find nearest road
        nearest_road = self._find_nearest_road()
        if nearest_road:
            self.target_road = nearest_road
        
        # Check for vehicles ahead
        collision_ahead = False
        for neighbor in perception['nearby_turtles']:
            if neighbor['distance'] < COLLISION_DISTANCE:
                # Check if neighbor is ahead
                angle_diff = abs(self.heading - neighbor['heading'])
                if angle_diff < math.pi / 4 or angle_diff > 7 * math.pi / 4:
                    collision_ahead = True
                    break
        
        # Adjust speed based on traffic
        if collision_ahead:
            # Slow down or stop
            influences.append(self.influence_set_speed(0.5))
        else:
            # Normal speed
            influences.append(self.influence_set_speed(VEHICLE_SPEED))
        
        # Steer toward road
        if self.target_road:
            target_heading = self.target_road.angle
            heading_diff = target_heading - self.heading
            
            # Normalize angle difference
            while heading_diff > math.pi:
                heading_diff -= 2 * math.pi
            while heading_diff < -math.pi:
                heading_diff += 2 * math.pi
            
            # Gradual steering
            if abs(heading_diff) > 0.1:
                turn_amount = max(-0.2, min(0.2, heading_diff * 0.3))
                influences.append(self.influence_turn(turn_amount))
        
        # Move forward
        influences.append(self.influence_move_forward(self.speed))
        
        return influences
    
    def _find_nearest_road(self):
        """Find the nearest road to current position."""
        if not self.roads:
            return None
        
        nearest = None
        min_dist = float('inf')
        
        for road in self.roads:
            dist = road.distance_to_point(self.position.x, self.position.y)
            if dist < min_dist:
                min_dist = dist
                nearest = road
        
        return nearest


def create_transport_simulation():
    """Create and configure the transport simulation."""
    
    # Create environment
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
    
    # Create road network (simple grid)
    roads = []
    
    # Horizontal roads
    for y in [30, 60, 90, 120]:
        roads.append(Road(10, y, GRID_SIZE - 10, y))
    
    # Vertical roads
    for x in [30, 60, 90, 120]:
        roads.append(Road(x, 10, x, GRID_SIZE - 10))
    
    # Create simulation
    sim = Simulation(
        environment=env,
        title="Transport Simulation"
    )
    
    # Store roads for visualization
    sim.roads = roads
    
    # Add vehicles on roads
    for _ in range(NUM_VEHICLES):
        road = random.choice(roads)
        # Start somewhere along the road
        t = random.random()
        x = road.start_x + t * (road.end_x - road.start_x)
        y = road.start_y + t * (road.end_y - road.start_y)
        
        from similar2logo.tools import Point2D
        vehicle = Vehicle(
            roads=roads,
            position=Point2D(x, y),
            heading=road.angle
        )
        sim.add_agent(vehicle)
    
    return sim


def main():
    """Run the transport simulation."""
    print("=" * 60)
    print("Transport Simulation - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Number of vehicles: {NUM_VEHICLES}")
    print(f"Vehicle speed: {VEHICLE_SPEED}")
    print("=" * 60)
    print()
    
    sim = create_transport_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=1,
        description="""
        # Transport Simulation
        
        Vehicles navigate a road network!
        
        - **Blue dots**: Vehicles following roads
        - **Gray lines**: Road network
        
        Vehicles:
        - Follow the nearest road
        - Slow down when traffic ahead
        - Maintain safe distances
        
        Watch as traffic patterns emerge from simple rules!
        """
    )


if __name__ == "__main__":
    main()
