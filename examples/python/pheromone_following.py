"""
Pheromone following example - turtles follow pheromone gradients.

This demonstrates:
- Pheromone creation and manipulation
- Gradient-based navigation
- Environmental sensing
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D, MathUtil
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


class PheromoneFollower(Turtle):
    """A turtle that follows pheromone gradients."""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.deposit_rate = 10.0
        self.sensor_distance = 5.0
        self.sensor_angle = PI / 4  # 45 degrees
    
    def decide(self, perception):
        influences = []
        env = perception['environment']
        
        # Deposit pheromone at current location
        # Emit influence instead of modifying environment directly
        influences.append(self.influence_emit_pheromone('trail', self.deposit_rate))
        
        # Sense pheromone in three directions (left, forward, right)
        left_sensor = self._sense_direction(env, self.heading - self.sensor_angle)
        forward_sensor = self._sense_direction(env, self.heading)
        right_sensor = self._sense_direction(env, self.heading + self.sensor_angle)
        
        # Turn towards strongest pheromone
        if forward_sensor > left_sensor and forward_sensor > right_sensor:
            # Continue forward
            pass
        elif left_sensor > right_sensor:
            # Turn left
            influences.append(self.influence_turn(-0.2))
        else:
            # Turn right
            influences.append(self.influence_turn(0.2))
        
        # Add some randomness
        if random.random() < 0.05:
            influences.append(self.influence_turn(random.uniform(-0.3, 0.3)))
        
        # Move forward
        influences.append(self.influence_move_forward(1.0))
        
        return influences
    
    def _sense_direction(self, env, heading):
        """Sense pheromone in a given direction."""
        # Calculate sensor position
        dx = sin(heading) * self.sensor_distance
        dy = -cos(heading) * self.sensor_distance
        
        sensor_x = int(self.position.x + dx)
        sensor_y = int(self.position.y + dy)
        
        # Clamp to environment bounds
        sensor_x = max(0, min(env.width - 1, sensor_x))
        sensor_y = max(0, min(env.height - 1, sensor_y))
        
        try:
            return env.get_pheromone_value(sensor_x, sensor_y, 'trail')
        except:
            return 0.0


def main():
    # Create environment
    print("Creating environment with pheromone...")
    env = Environment(width=100, height=100, toroidal=True)
    
    # Add pheromone
    env.add_pheromone(
        identifier='trail',
        diffusion_coef=0.05,
        evaporation_coef=0.01,
        default_value=0.0,
        min_value=0.1
    )
    
    # Create simulation
    print("Creating simulation with 30 pheromone followers...")
    sim = LogoSimulation(env, num_turtles=30, turtle_class=PheromoneFollower)
    
    # Add some initial pheromone sources
    print("Adding initial pheromone sources...")
    for _ in range(5):
        x = random.randint(10, 90)
        y = random.randint(10, 90)
        env.set_pheromone(x, y, 'trail', 100.0)
    
    # Run simulation
    print("Running simulation for 500 steps...")
    
    def progress_callback(sim, step):
        if step % 50 == 0:
            print(f"  Step {step}/500")
    
    sim.run(steps=500, callback=progress_callback)
    
    # Plot final state
    print("Plotting final state...")
    sim.plot()
    
    print("Done!")


if __name__ == "__main__":
    main()
