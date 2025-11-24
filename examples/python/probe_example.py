"""
Example demonstrating the RealTimeMatcherProbe for speed control.

This shows how to use probes to control simulation speed.
"""

from similar2logo import LogoSimulation, Turtle, Environment, RealTimeMatcherProbe
import random


class RandomWalker(Turtle):
    """A turtle that walks randomly."""
    
    def decide(self, perception):
        influences = []
        
        # Random walk
        if random.random() < 0.2:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        
        influences.append(self.influence_move_forward(1.0))
        
        return influences


def main():
    print("=" * 60)
    print("RealTimeMatcherProbe Example")
    print("=" * 60)
    print("\nThis demo shows how to control simulation speed using probes.")
    print()
    
    # Create environment and simulation
    env = Environment(width=100, height=100, toroidal=True)
    sim = LogoSimulation(env)
    
    # Add some random walkers
    sim.add_turtles(20, turtle_class=RandomWalker)
    
    # Add a RealTimeMatcherProbe to control speed
    # acceleration_factor = 10.0 means 10x faster than real time
    print("Adding RealTimeMatcherProbe with acceleration factor 10.0")
    print("(10x faster than real time)")
    print()
    
    sim.probe_manager.add_probe(
        "timing",
        RealTimeMatcherProbe(acceleration_factor=10.0)
    )
    
    # Run simulation
    print("Running simulation for 100 steps...")
    print("The probe will ensure it runs at the correct speed.")
    print()
    
    step_count = [0]
    
    def progress(sim, step):
        step_count[0] += 1
        if step % 10 == 0:
            print(f"  Step {step:3d} completed")
    
    import time
    start_time = time.time()
    
    sim.run(steps=100, callback=progress)
    
    elapsed = time.time() - start_time
    
    print()
    print("=" * 60)
    print(f"Simulation completed in {elapsed:.2f} seconds")
    print(f"Expected time: ~10 seconds (100 steps / 10x speed)")
    print(f"Actual speedup: {100/elapsed:.1f}x")
    print("=" * 60)


def demo_different_speeds():
    """Demonstrate different acceleration factors."""
    print("\n" + "=" * 60)
    print("Testing Different Acceleration Factors")
    print("=" * 60)
    
    env = Environment(width=50, height=50, toroidal=True)
    
    for factor in [1.0, 5.0, 20.0]:
        print(f"\nTesting acceleration factor: {factor}x")
        
        sim = LogoSimulation(env)
        sim.add_turtles(10, turtle_class=RandomWalker)
        sim.probe_manager.add_probe("timing", RealTimeMatcherProbe(factor))
        
        start = time.time()
        sim.run(steps=20)
        elapsed = time.time() - start
        
        print(f"  20 steps completed in {elapsed:.2f}s")
        print(f"  Expected: ~{20/factor:.2f}s")


if __name__ == "__main__":
    import time
    main()
    demo_different_speeds()
