"""
Simple random walk example - turtles wander randomly in the environment.

This demonstrates the basic usage of SIMILAR2Logo Python API.
"""

from similar2logo import LogoSimulation, Turtle, Environment
import random


class RandomWalker(Turtle):
    """A turtle that performs a random walk."""
    
    def decide(self, perception):
        influences = []
        
        # Random turn occasionally
        if random.random() < 0.1:
            # Emit turn influence
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        
        # Always move forward
        # Emit move influence
        influences.append(self.influence_move_forward(1.0))
        
        return influences


def main():
    # Create environment
    print("Creating environment...")
    env = Environment(width=100, height=100, toroidal=True)
    
    # Create simulation with random walkers
    print("Creating simulation with 50 random walkers...")
    sim = LogoSimulation(env, num_turtles=50, turtle_class=RandomWalker)
    
    # Run simulation
    print("Running simulation for 1000 steps...")
    
    def progress_callback(sim, step):
        if step % 100 == 0:
            print(f"  Step {step}/1000")
    
    sim.run(steps=1000, callback=progress_callback)
    
    # Plot final state
    print("Plotting final state...")
    sim.plot()
    
    print("Done!")



def main_web(port=8080):
    """Run simulation with web interface"""
    # Create environment
    env = Environment(width=100, height=100, toroidal=True)
    
    # Create simulation with random walkers
    sim = LogoSimulation(env, num_turtles=50, turtle_class=RandomWalker)
    
    # Create web interface
    from similar2logo.web import WebSimulation
    web_sim = WebSimulation(sim)
    web_sim.start_server(port=port)


if __name__ == "__main__":
    import sys
    if "--web" in sys.argv:
        port = 8080
        if "--port" in sys.argv:
            try:
                port_idx = sys.argv.index("--port") + 1
                if port_idx < len(sys.argv):
                    port = int(sys.argv[port_idx])
            except ValueError:
                pass
        main_web(port)
    else:
        main()
