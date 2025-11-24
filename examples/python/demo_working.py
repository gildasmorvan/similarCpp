"""
Simple working example demonstrating SIMILAR2Logo Python API.

This creates a basic random walk simulation with 50 turtles.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../python'))

from similar2logo import LogoSimulation, Turtle, Environment
import random


class RandomWalker(Turtle):
    """A turtle that performs a random walk."""
    
    def decide(self, perception):
        influences = []
        
        # Random turn occasionally
        if random.random() < 0.1:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        
        # Move forward
        influences.append(self.influence_move_forward(1.0))
        
        return influences


def main():
    print("=" * 70)
    print("SIMILAR2Logo Python - Random Walk Example")
    print("=" * 70)
    print()
    
    # Create environment
    print("Creating environment (100x100, toroidal)...")
    env = Environment(width=100, height=100, toroidal=True)
    
    # Create simulation
    print("Creating simulation with 50 random walkers...")
    sim = LogoSimulation(env, num_turtles=50, turtle_class=RandomWalker)
    
    # Run simulation
    print("Running simulation for 100 steps...")
    print()
    
    def progress(sim, step):
        if step % 10 == 0:
            print(f"  Step {step}/100")
    
    sim.run(steps=100, callback=progress)
    
    print()
    print("=" * 70)
    print("Simulation complete!")
    print(f"Final step: {sim.current_step}")
    print(f"Number of turtles: {len(sim.turtles)}")
    print()
    print("✅ Python API works perfectly!")
    print("=" * 70)


if __name__ == "__main__":
    main()
