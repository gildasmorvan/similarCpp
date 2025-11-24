"""
Ant Colony Foraging - Stigmergy and pheromone-based coordination

This implements a simplified ant colony foraging model demonstrating:
- Pheromone-based indirect communication (stigmergy)
- Emergent path optimization
- Collective intelligence

Inspired by the work of Deneubourg et al. (1990) and Dorigo & Stützle (2004).
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D, MathUtil
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


class Ant(Turtle):
    """
    An ant agent that forages for food and returns to nest.
    
    States:
        - SEARCHING: Looking for food
        - RETURNING: Carrying food back to nest
    
    Behavior:
        - When searching: Follow food pheromone, wander if none
        - When returning: Follow home pheromone, deposit food pheromone
    """
    
    def __init__(self, nest_position, *args, **kwargs):
        super().__init__(*args, **kwargs)
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
        # Check for high food pheromone concentration
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
    
    def _deposit_pheromone(self, env, pheromone_id, amount):
        """Deprecated: Use influence_emit_pheromone instead."""
        pass

# ... (in main)
    # Add ants at nest
    for _ in range(50):
        ant = Ant(nest_position=nest_pos, position=nest_pos, 
                 heading=random.uniform(0, MathUtil.TWO_PI))
        ant._environment = env
        sim.turtles.append(ant)

# ... (in main_web)
    # Add ants
    for _ in range(100):
        ant = Ant(nest_position=nest_pos, position=nest_pos,
                 heading=random.uniform(0, MathUtil.TWO_PI))
        ant._environment = env
        sim.turtles.append(ant)
    
    # Place food sources
    for fx, fy in [(20, 20), (80, 20), (50, 80)]:
        for dx in range(-3, 4):
            for dy in range(-3, 4):
                x, y = fx + dx, fy + dy
                if 0 <= x < env.width and 0 <= y < env.height:
                    env.set_pheromone(x, y, 'food_source', 100.0)
    
    # Mark nest
    for dx in range(-5, 6):
        for dy in range(-5, 6):
            x, y = int(nest_pos.x) + dx, int(nest_pos.y) + dy
            if 0 <= x < env.width and 0 <= y < env.height:
                env.set_pheromone(x, y, 'home', 50.0)
    
    # Create web interface
    web_sim = WebSimulation(sim, update_rate=30)
    
    print("\nOpen your browser to: http://localhost:8080")
    print("Watch pheromone trails emerge in real-time!")
    print("=" * 60 + "\n")
    
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
