"""
Web-based simulation demo with real-time visualization.

This demonstrates:
- Web interface setup
- Real-time visualization
- Interactive control
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.web import WebSimulation
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


class FlockingTurtle(Turtle):
    """A turtle that exhibits flocking behavior (boids-like)."""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.perception_radius = 10.0
        self.separation_distance = 3.0
    
    def decide(self, perception):
        influences = []
        
        # Simple flocking behavior
        # In a full implementation, this would:
        # 1. Separate from nearby turtles
        # 2. Align with nearby turtles' headings
        # 3. Move towards center of nearby turtles
        
        # For now, just do a random walk with occasional turns
        if random.random() < 0.1:
            influences.append(self.influence_turn(random.uniform(-0.3, 0.3)))
        
        influences.append(self.influence_move_forward(0.5))
        
        return influences


def main():
    print("=" * 60)
    print("SIMILAR2Logo Web Demo")
    print("=" * 60)
    
    # Create environment
    print("\n1. Creating environment...")
    env = Environment(width=100, height=100, toroidal=True)
    
    # Add pheromones
    print("2. Adding pheromones...")
    env.add_pheromone('food', diffusion_coef=0.1, evaporation_coef=0.01)
    env.add_pheromone('danger', diffusion_coef=0.2, evaporation_coef=0.05)
    
    # Create simulation
    print("3. Creating simulation with 100 flocking turtles...")
    sim = LogoSimulation(env, num_turtles=100, turtle_class=FlockingTurtle)
    
    # Create web interface
    print("4. Setting up web interface...")
    web_sim = WebSimulation(sim, update_rate=30)
    
    print("\n" + "=" * 60)
    print("Starting web server...")
    print("=" * 60)
    print("\nOpen your browser and navigate to:")
    print("  http://localhost:8080")
    print("\nControls:")
    print("  - Click 'Start' to begin the simulation")
    print("  - Click 'Pause' to pause")
    print("  - Click 'Reset' to restart")
    print("\nPress Ctrl+C to stop the server")
    print("=" * 60 + "\n")
    
    # Start server (this blocks)
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    main()
