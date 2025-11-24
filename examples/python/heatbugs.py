"""
Heatbugs Simulation - SIMILAR2Logo Python DSL

Bugs that seek optimal temperature by moving toward/away from heat.
Each bug emits heat, creating a dynamic temperature field.

Based on the classic Heatbugs model and SIMILAR2Logo Java example.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.dsl import *
import random
import math

# Simulation parameters
GRID_SIZE = 100
NUM_BUGS = 100
IDEAL_TEMPERATURE = 25.0
HEAT_OUTPUT = 10.0
EVAPORATION_RATE = 0.05
DIFFUSION_RATE = 0.1
RANDOM_MOVE_PROBABILITY = 0.1

class HeatBug(Turtle):
    """Bug that seeks its ideal temperature."""
    
    def __init__(self, ideal_temp=IDEAL_TEMPERATURE, **kwargs):
        super().__init__(**kwargs)
        self.ideal_temp = ideal_temp
        self.color = "red"
        self.speed = 1.0
    
    def decide(self, perception):
        """Move toward/away from heat to reach ideal temperature."""
        influences = []
        
        # Emit heat at current location
        influences.append(
            self.influence_emit_pheromone("heat", HEAT_OUTPUT)
        )
        
        # Get current temperature
        current_temp = perception['pheromones'].get('heat', 0.0)
        
        # Random movement sometimes
        if random.random() < RANDOM_MOVE_PROBABILITY:
            influences.append(self.influence_turn(random.uniform(-math.pi, math.pi)))
        else:
            # Move based on temperature gradient
            if current_temp < self.ideal_temp:
                # Too cold - move toward heat
                best_heading = self._find_warmest_direction(perception)
                if best_heading is not None:
                    influences.append(self.influence_turn_towards(best_heading))
            elif current_temp > self.ideal_temp:
                # Too hot - move away from heat
                best_heading = self._find_coolest_direction(perception)
                if best_heading is not None:
                    influences.append(self.influence_turn_towards(best_heading))
        
        influences.append(self.influence_move_forward(self.speed))
        
        return influences
    
    def _find_warmest_direction(self, perception):
        """Find direction toward warmest nearby location."""
        env = perception['environment']
        best_temp = -float('inf')
        best_heading = None
        
        # Sample nearby locations
        for angle in [0, math.pi/4, math.pi/2, 3*math.pi/4, math.pi, 
                      -3*math.pi/4, -math.pi/2, -math.pi/4]:
            # Look ahead in this direction
            test_x = self.position.x + 5 * math.sin(angle)
            test_y = self.position.y - 5 * math.cos(angle)
            
            # Clamp to grid
            test_x = max(0, min(test_x, env.width - 1))
            test_y = max(0, min(test_y, env.height - 1))
            
            temp = env.get_pheromone_value(int(test_x), int(test_y), 'heat')
            if temp > best_temp:
                best_temp = temp
                best_heading = angle
        
        return best_heading
    
    def _find_coolest_direction(self, perception):
        """Find direction toward coolest nearby location."""
        env = perception['environment']
        best_temp = float('inf')
        best_heading = None
        
        # Sample nearby locations
        for angle in [0, math.pi/4, math.pi/2, 3*math.pi/4, math.pi,
                      -3*math.pi/4, -math.pi/2, -math.pi/4]:
            # Look ahead in this direction
            test_x = self.position.x + 5 * math.sin(angle)
            test_y = self.position.y - 5 * math.cos(angle)
            
            # Clamp to grid
            test_x = max(0, min(test_x, env.width - 1))
            test_y = max(0, min(test_y, env.height - 1))
            
            temp = env.get_pheromone_value(int(test_x), int(test_y), 'heat')
            if temp < best_temp:
                best_temp = temp
                best_heading = angle
        
        return best_heading


def create_heatbugs_simulation():
    """Create and configure the heatbugs simulation."""
    
    # Create environment with heat pheromone
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
    env.add_pheromone(
        "heat",
        diffusion_coef=DIFFUSION_RATE,
        evaporation_coef=EVAPORATION_RATE
    )
    
    # Create simulation
    sim = Simulation(
        environment=env,
        title="Heatbugs Simulation"
    )
    
    # Add bugs with varying ideal temperatures
    for i in range(NUM_BUGS):
        # Vary ideal temperature around the mean
        ideal_temp = IDEAL_TEMPERATURE + random.uniform(-5, 5)
        bug = HeatBug(
            ideal_temp=ideal_temp,
            position=env.random_position(),
            heading=env.random_heading()
        )
        sim.add_agent(bug)
    
    return sim


def main():
    """Run the heatbugs simulation."""
    print("=" * 60)
    print("Heatbugs Simulation - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Number of bugs: {NUM_BUGS}")
    print(f"Ideal temperature: {IDEAL_TEMPERATURE}°")
    print(f"Heat output per bug: {HEAT_OUTPUT}")
    print(f"Evaporation rate: {EVAPORATION_RATE}")
    print(f"Diffusion rate: {DIFFUSION_RATE}")
    print("=" * 60)
    print()
    
    sim = create_heatbugs_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=1,
        description="""
        # Heatbugs Simulation
        
        Bugs (red dots) seek their ideal temperature by moving through a heat field.
        
        - Each bug emits heat at its location
        - Heat diffuses and evaporates over time
        - Bugs move toward heat if too cold
        - Bugs move away from heat if too hot
        
        Watch as the bugs self-organize into temperature-regulated clusters!
        """
    )


if __name__ == "__main__":
    main()
