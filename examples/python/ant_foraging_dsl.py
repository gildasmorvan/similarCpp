"""
Ant Colony Foraging - DSL Example

Demonstrates the DSL with custom environment setup and pheromones.
"""

from similar2logo.dsl import Simulation, Turtle
from similar2logo.tools import Point2D, MathUtil
from similar2logo.influences import DropMark
from similar2logo.fastmath import sin, cos, atan2, PI
import random


class Ant(Turtle):
    """
    An ant agent that forages for food and returns to nest.
    """
    
    def __init__(self, nest_position=None, **kwargs):
        # Handle nest_position being passed in kwargs or explicitly
        if nest_position is None:
            nest_position = Point2D(50, 50)
            
        super().__init__(**kwargs)
        self.nest_position = nest_position
        self.has_food = False
        self.state = "SEARCHING"
        
        # Behavior parameters
        self.pheromone_deposit_amount = 10.0
        self.sensor_distance = 3.0
        self.sensor_angle = PI / 4  # 45 degrees
        self.random_walk_prob = 0.1
        
        self.color = "black"
    
    def decide(self, perception):
        """Main decision logic based on current state."""
        if self.state == "SEARCHING":
            return self._search_for_food(perception)
        elif self.state == "RETURNING":
            return self._return_to_nest(perception)
        return []
    
    def _search_for_food(self, perception):
        """Search for food by following food pheromone or wandering."""
        env = perception['environment']
        influences = []
        
        # Check if at food source
        if self._at_food_source(env):
            self.has_food = True
            self.state = "RETURNING"
            self.color = "brown"  # Carrying food
            # Turn around
            influences.append(self.influence_turn(PI))
            return influences
        
        # Follow food pheromone gradient
        food_gradient = self._sense_pheromone_gradient(env, 'food')
        
        if food_gradient > 0.1:
            # Strong signal, follow it
            direction = self._get_gradient_direction(env, 'food')
            influences.append(self.influence_turn_towards(direction))
        else:
            # Weak signal, random walk
            if random.random() < self.random_walk_prob:
                influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        
        influences.append(self.influence_move_forward(1.0))
        return influences
    
    def _return_to_nest(self, perception):
        """Return to nest, depositing food pheromone trail."""
        env = perception['environment']
        influences = []
        
        # Deposit food pheromone
        influences.append(self.influence_emit_pheromone('food', self.pheromone_deposit_amount))
        
        # Check if at nest
        if self._at_nest():
            self.has_food = False
            self.state = "SEARCHING"
            self.color = "black"
            # Turn around
            influences.append(self.influence_turn(PI))
            return influences
        
        # Follow home pheromone or head towards nest
        home_gradient = self._sense_pheromone_gradient(env, 'home')
        
        if home_gradient > 0.1:
            # Follow home pheromone
            direction = self._get_gradient_direction(env, 'home')
            influences.append(self.influence_turn_towards(direction))
        else:
            # Head directly towards nest
            direction = env.get_direction(self.position, self.nest_position)
            influences.append(self.influence_turn_towards(direction))
        
        influences.append(self.influence_move_forward(1.0))
        return influences
    
    def _at_food_source(self, env):
        """Check if ant is at a food source."""
        x, y = int(self.position.x), int(self.position.y)
        try:
            food_level = env.get_pheromone_value(x, y, 'food_source')
            return food_level > 50.0
        except:
            return False
    
    def _at_nest(self):
        """Check if ant is at the nest."""
        distance = self.position.distance(self.nest_position)
        return distance < 5.0
    
    def _sense_pheromone_gradient(self, env, pheromone_id):
        """Sense pheromone gradient strength."""
        x, y = int(self.position.x), int(self.position.y)
        try:
            return env.get_pheromone_value(x, y, pheromone_id)
        except:
            return 0.0
    
    def _get_gradient_direction(self, env, pheromone_id):
        """Get direction of strongest pheromone gradient."""
        # Sample in three directions
        left = self._sense_direction(env, pheromone_id, self.heading - self.sensor_angle)
        forward = self._sense_direction(env, pheromone_id, self.heading)
        right = self._sense_direction(env, pheromone_id, self.heading + self.sensor_angle)
        
        # Return direction of strongest signal
        if forward >= left and forward >= right:
            return self.heading
        elif left > right:
            return self.heading - self.sensor_angle
        else:
            return self.heading + self.sensor_angle
    
    def _sense_direction(self, env, pheromone_id, direction):
        """Sense pheromone in a specific direction."""
        dx = sin(direction) * self.sensor_distance
        dy = -cos(direction) * self.sensor_distance
        
        x = int(self.position.x + dx)
        y = int(self.position.y + dy)
        
        # Clamp to bounds
        x = max(0, min(env.width - 1, x))
        y = max(0, min(env.height - 1, y))
        
        try:
            return env.get_pheromone_value(x, y, pheromone_id)
        except:
            return 0.0


def setup_simulation(sim):
    """Configure environment and pheromones."""
    env = sim.environment
    
    # Add pheromones
    env.add_pheromone('food', diffusion_coef=0.5, evaporation_coef=0.01)
    env.add_pheromone('home', diffusion_coef=0.5, evaporation_coef=0.01)
    env.add_pheromone('food_source', diffusion_coef=0.0, evaporation_coef=0.0)
    
    nest_pos = Point2D(50, 50)
    
    # Place food sources
    for fx, fy in [(20, 20), (80, 20), (50, 80)]:
        for dx in range(-3, 4):
            for dy in range(-3, 4):
                x, y = fx + dx, fy + dy
                if 0 <= x < env.width and 0 <= y < env.height:
                    env.set_pheromone(x, y, 'food_source', 100.0)
    
    # Mark nest with home pheromone (permanent gradient)
    for dx in range(-5, 6):
        for dy in range(-5, 6):
            x, y = int(nest_pos.x) + dx, int(nest_pos.y) + dy
            if 0 <= x < env.width and 0 <= y < env.height:
                env.set_pheromone(x, y, 'home', 50.0)


def main():
    """Run ant foraging simulation in console mode"""
    print("=" * 60)
    print("Ant Foraging - DSL Example (Console)")
    print("=" * 60)
    
    nest_pos = Point2D(50, 50)
    
    (Simulation("Ant Foraging")
     .grid(100, 100, toroidal=False)
     .agents(Ant, count=50, nest_position=nest_pos, position=nest_pos)
     .setup(setup_simulation)
     .run(max_steps=1000))


def main_web():
    """Run ant foraging simulation with web interface"""
    nest_pos = Point2D(50, 50)
    
    (Simulation("Ant Foraging - DSL")
     .grid(100, 100, toroidal=False)
     .agents(Ant, count=100, nest_position=nest_pos, position=nest_pos)
     .setup(setup_simulation)
     .run_web(port=8080))


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
