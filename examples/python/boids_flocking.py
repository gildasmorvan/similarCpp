"""
Boids Flocking Model - Classic emergent behavior simulation

This implements Craig Reynolds' Boids algorithm (1986) demonstrating how
complex flocking behavior emerges from three simple rules:
1. Separation - Avoid crowding neighbors
2. Alignment - Steer towards average heading of neighbors
3. Cohesion - Steer towards average position of neighbors

Reference: Reynolds, C. W. (1987). Flocks, herds and schools: A distributed
behavioral model. ACM SIGGRAPH computer graphics, 21(4), 25-32.
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D, MathUtil
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


class Boid(Turtle):
    """
    A boid (bird-oid) that exhibits flocking behavior.
    
    Attributes:
        perception_radius: How far the boid can see
        separation_distance: Minimum distance to maintain from others
        max_speed: Maximum movement speed
        max_force: Maximum steering force
    """
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        
        # Behavior parameters
        self.perception_radius = 15.0
        self.separation_distance = 5.0
        self.max_speed = 2.0
        self.max_force = 0.1
        
        # Weights for each behavior
        self.separation_weight = 1.5
        self.alignment_weight = 1.0
        self.cohesion_weight = 1.0
        
        # Velocity (derived from heading and speed)
        self.velocity = Point2D(0, 0)
        self.speed = 1.0
    
    def decide(self, perception):
        """Apply the three boids rules."""
        influences = []
        
        # Get nearby boids
        nearby_boids = self._get_nearby_boids(perception)
        
        if len(nearby_boids) == 0:
            # No neighbors, just move forward
            # influences.append(self.influence_move_forward(self.speed))
            # Actually, we should move based on current velocity
            dx = self.velocity.x
            dy = self.velocity.y
            influences.append(self.influence_move_forward(sqrt(dx*dx + dy*dy)))
            return influences
        
        # Calculate the three forces
        separation = self._separate(nearby_boids)
        alignment = self._align(nearby_boids)
        cohesion = self._cohere(nearby_boids)
        
        # Weight the forces
        separation = separation * self.separation_weight
        alignment = alignment * self.alignment_weight
        cohesion = cohesion * self.cohesion_weight
        
        # Combine forces
        total_force = separation + alignment + cohesion
        
        # Apply steering
        if total_force.magnitude() > 0:
            # Limit force
            if total_force.magnitude() > self.max_force:
                total_force = total_force.normalize() * self.max_force
            
            # Update velocity (internal state)
            self.velocity = self.velocity + total_force
            
            # Limit speed
            if self.velocity.magnitude() > self.max_speed:
                self.velocity = self.velocity.normalize() * self.max_speed
            
            # Emit influences to update physical state
            if self.velocity.magnitude() > 0:
                # Update heading to match velocity
                new_heading = atan2(self.velocity.x, -self.velocity.y)
                influences.append(self.influence_turn_towards(new_heading))
                
                # Move based on velocity
                # Note: In Logo, y is down (usually), but standard math uses y up.
                # Our Point2D and environment seem to treat y as just a coordinate.
                # If heading 0 is North (up/negative y?), then dx=sin(h), dy=-cos(h).
                # Here we already have velocity vector, so just use it.
                influences.append(self.influence_move_forward(self.velocity.magnitude()))
        else:
            # No steering needed, just move forward
            influences.append(self.influence_move_forward(self.velocity.magnitude()))
            
        return influences
    
    def _get_nearby_boids(self, perception):
        """Get list of boids within perception radius."""
        # Use the pre-calculated nearby turtles from perception
        nearby_data = perception.get('nearby_turtles', [])
        nearby_boids = []
        
        for item in nearby_data:
            # item is {'turtle': t, 'distance': d, ...}
            if item['distance'] < self.perception_radius:
                nearby_boids.append(item['turtle'])
                
        return nearby_boids
    
    def _separate(self, nearby_boids):
        """
        Separation: Steer to avoid crowding local flockmates.
        Returns a steering force away from nearby boids.
        """
        steer = Point2D(0, 0)
        count = 0
        
        for boid in nearby_boids:
            distance = self.position.distance(boid.position)
            if distance > 0 and distance < self.separation_distance:
                # Calculate vector pointing away from neighbor
                diff = self.position - boid.position
                diff = diff.normalize()
                diff = diff * (1.0 / distance)  # Weight by distance
                steer = steer + diff
                count += 1
        
        if count > 0:
            steer = steer * (1.0 / count)
        
        if steer.magnitude() > 0:
            # Implement Reynolds: Steering = Desired - Velocity
            steer = steer.normalize() * self.max_speed
            steer = steer - self.velocity
        
        return steer
    
    def _align(self, nearby_boids):
        """
        Alignment: Steer towards the average heading of local flockmates.
        Returns a steering force to match velocity with neighbors.
        """
        sum_velocity = Point2D(0, 0)
        count = 0
        
        for boid in nearby_boids:
            distance = self.position.distance(boid.position)
            if distance > 0 and distance < self.perception_radius:
                sum_velocity = sum_velocity + boid.velocity
                count += 1
        
        if count > 0:
            sum_velocity = sum_velocity * (1.0 / count)
            sum_velocity = sum_velocity.normalize() * self.max_speed
            
            # Steering = Desired - Velocity
            steer = sum_velocity - self.velocity
            return steer
        
        return Point2D(0, 0)
    
    def _cohere(self, nearby_boids):
        """
        Cohesion: Steer to move towards the average position of local flockmates.
        Returns a steering force towards the center of mass.
        """
        sum_position = Point2D(0, 0)
        count = 0
        
        for boid in nearby_boids:
            distance = self.position.distance(boid.position)
            if distance > 0 and distance < self.perception_radius:
                sum_position = sum_position + boid.position
                count += 1
        
        if count > 0:
            sum_position = sum_position * (1.0 / count)
            return self._seek(sum_position)
        
        return Point2D(0, 0)
    
    def _seek(self, target):
        """Calculate steering force to seek a target position."""
        desired = target - self.position
        desired = desired.normalize() * self.max_speed
        
        # Steering = Desired - Velocity
        steer = desired - self.velocity
        return steer


def main():
    """Run the Boids flocking simulation."""
    print("=" * 60)
    print("Boids Flocking Simulation")
    print("=" * 60)
    print("\nImplementing Craig Reynolds' Boids algorithm (1986)")
    print("Three simple rules create emergent flocking behavior:")
    print("  1. Separation - Avoid crowding")
    print("  2. Alignment - Match neighbors' heading")
    print("  3. Cohesion - Move towards center of flock")
    print("=" * 60 + "\n")
    
    # Create environment
    print("Creating environment...")
    env = Environment(width=200, height=200, toroidal=True)
    
    # Create simulation with boids
    print("Creating simulation with 100 boids...")
    sim = LogoSimulation(env, num_turtles=100, turtle_class=Boid)
    
    # Randomize initial velocities
    for boid in sim.turtles:
        angle = random.uniform(0, MathUtil.TWO_PI)
        speed = random.uniform(0.5, 2.0)
        boid.velocity = Point2D(
            speed * sin(angle),
            -speed * cos(angle)
        )
        boid.speed = speed
    
    print("\nRunning simulation...")
    print("Watch as the boids self-organize into flocks!")
    print()
    
    # Progress callback
    def progress(sim, step):
        if step % 100 == 0:
            print(f"  Step {step}/1000 - Observing emergent behavior...")
    
    # Run simulation
    sim.run(steps=1000, callback=progress)
    
    # Plot final state
    print("\nPlotting final state...")
    sim.plot()
    
    print("\n" + "=" * 60)
    print("Simulation complete!")
    print("Notice how the boids have self-organized into cohesive flocks")
    print("moving in coordinated patterns - all from simple local rules!")
    print("=" * 60)


def main_web():
    """Run the Boids simulation with web interface."""
    from similar2logo.web import WebSimulation
    
    print("=" * 60)
    print("Boids Flocking - Web Interface")
    print("=" * 60)
    
    # Create environment and simulation
    env = Environment(width=200, height=200, toroidal=True)
    sim = LogoSimulation(env, num_turtles=150, turtle_class=Boid)
    
    # Initialize velocities
    for boid in sim.turtles:
        angle = random.uniform(0, MathUtil.TWO_PI)
        speed = random.uniform(0.5, 2.0)
        boid.velocity = Point2D(
            speed * sin(angle),
            -speed * cos(angle)
        )
    
    # Create web interface
    web_sim = WebSimulation(sim, update_rate=30)
    
    print("\nOpen your browser to: http://localhost:8080")
    print("Watch the emergent flocking behavior in real-time!")
    print("=" * 60 + "\n")
    
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
