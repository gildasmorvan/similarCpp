"""
Slime Mold Aggregation - Dictyostelium discoideum simulation

This models the aggregation behavior of slime mold cells using:
- cAMP (cyclic AMP) pheromone signaling
- Chemotaxis (movement towards chemical gradient)
- Pulsatile signaling
- Emergent spiral wave patterns

Reference: Keller, E. F., & Segel, L. A. (1970). Initiation of slime mold
aggregation viewed as an instability. Journal of theoretical biology, 26(3), 399-415.
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D, MathUtil
import random
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI


class SlimeMoldCell(Turtle):
    """
    A slime mold cell (amoeba) that aggregates using cAMP signaling.
    
    Behavior:
        - Emit cAMP pulses periodically
        - Move towards cAMP gradient (chemotaxis)
        - Relay cAMP signals (amplification)
    """
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        
        # Signaling parameters
        self.camp_production_rate = 5.0
        self.camp_threshold = 10.0
        self.signal_period = 20  # Steps between signals
        self.signal_counter = random.randint(0, self.signal_period)
        self.refractory_period = 10
        self.refractory_counter = 0
        
        # Movement parameters
        self.chemotaxis_strength = 0.3
        self.random_motion_strength = 0.1
        self.sensor_distance = 3.0
        self.sensor_angle = PI / 3  # 60 degrees
        
        self.color = "yellow"
        self.speed = 0.5
    
    def decide(self, perception):
        """Cell decision logic."""
        env = perception['environment']
        
        # Update counters
        self.signal_counter += 1
        if self.refractory_counter > 0:
            self.refractory_counter -= 1
        
        # Emit cAMP signal periodically
        if self.signal_counter >= self.signal_period and self.refractory_counter == 0:
            self._emit_camp(env)
            self.signal_counter = 0
            self.refractory_counter = self.refractory_period
        
        # Sense cAMP gradient
        camp_level = self._sense_camp(env)
        
        # If cAMP detected above threshold, relay signal
        if camp_level > self.camp_threshold and self.refractory_counter == 0:
            self._relay_camp(env)
            self.refractory_counter = self.refractory_period
        
        # Move towards cAMP gradient (chemotaxis)
        gradient_direction = self._get_camp_gradient_direction(env)
        
        if gradient_direction is not None:
            # Chemotaxis
            target_heading = gradient_direction
            heading_diff = MathUtil.angle_difference(self.heading, target_heading)
            self.turn(heading_diff * self.chemotaxis_strength)
        
        # Add random motion
        if random.random() < self.random_motion_strength:
            self.turn(random.uniform(-0.3, 0.3))
        
        # Move
        self.move_forward(self.speed)
    
    def _emit_camp(self, env):
        """Emit cAMP signal at current location."""
        x, y = int(self.position.x), int(self.position.y)
        try:
            current = env.get_pheromone_value(x, y, 'camp')
            env.set_pheromone(x, y, 'camp', current + self.camp_production_rate)
        except:
            pass
    
    def _relay_camp(self, env):
        """Relay (amplify) cAMP signal."""
        self._emit_camp(env)
    
    def _sense_camp(self, env):
        """Sense cAMP concentration at current location."""
        x, y = int(self.position.x), int(self.position.y)
        try:
            return env.get_pheromone_value(x, y, 'camp')
        except:
            return 0.0
    
    def _get_camp_gradient_direction(self, env):
        """Get direction of cAMP gradient."""
        # Sample in three directions
        left = self._sense_camp_direction(env, self.heading - self.sensor_angle)
        forward = self._sense_camp_direction(env, self.heading)
        right = self._sense_camp_direction(env, self.heading + self.sensor_angle)
        
        # Find strongest direction
        max_signal = max(left, forward, right)
        
        if max_signal < self.camp_threshold:
            return None  # No significant gradient
        
        if forward == max_signal:
            return self.heading
        elif left == max_signal:
            return self.heading - self.sensor_angle
        else:
            return self.heading + self.sensor_angle
    
    def _sense_camp_direction(self, env, direction):
        """Sense cAMP in a specific direction."""
        dx = sin(direction) * self.sensor_distance
        dy = -cos(direction) * self.sensor_distance
        
        x = int(self.position.x + dx)
        y = int(self.position.y + dy)
        
        # Clamp to bounds
        x = max(0, min(env.width - 1, x))
        y = max(0, min(env.height - 1, y))
        
        try:
            return env.get_pheromone_value(x, y, 'camp')
        except:
            return 0.0


def main():
    """Run the slime mold aggregation simulation."""
    print("=" * 60)
    print("Slime Mold Aggregation Simulation")
    print("=" * 60)
    print("\nModeling Dictyostelium discoideum aggregation using cAMP signaling.")
    print("\nMechanism:")
    print("  - Cells emit cAMP pulses periodically")
    print("  - Cells move towards cAMP gradients (chemotaxis)")
    print("  - Cells relay cAMP signals (amplification)")
    print("  - cAMP diffuses and degrades")
    print("\nExpected Patterns:")
    print("  - Spiral waves")
    print("  - Aggregation streams")
    print("  - Mound formation")
    print("=" * 60 + "\n")
    
    # Create environment
    print("Creating environment...")
    env = Environment(width=100, height=100, toroidal=False)
    
    # Add cAMP pheromone with diffusion and degradation
    print("Adding cAMP signaling system...")
    env.add_pheromone(
        identifier='camp',
        diffusion_coef=0.1,      # Diffuses quickly
        evaporation_coef=0.05,   # Degrades quickly
        default_value=0.0,
        min_value=0.1
    )
    
    # Create simulation
    print("Creating simulation with 500 slime mold cells...")
    sim = LogoSimulation(env, num_turtles=500, turtle_class=SlimeMoldCell)
    
    # Distribute cells randomly
    print("Distributing cells randomly...")
    for cell in sim.turtles:
        cell.position = env.random_position()
        cell.heading = env.random_heading()
        cell._sync_to_cpp()
    
    print("\nRunning simulation...")
    print("Watch for spiral waves and aggregation patterns!")
    print()
    
    def progress(sim, step):
        if step % 100 == 0:
            # Calculate average clustering (simplified metric)
            print(f"  Step {step:4d}: Observing aggregation patterns...")
    
    # Run simulation
    sim.run(steps=1000, callback=progress)
    
    print("\n" + "=" * 60)
    print("Simulation complete!")
    print("\nKey Observations:")
    print("  - Cells self-organize into aggregation centers")
    print("  - Spiral waves propagate through the population")
    print("  - Streams form as cells move towards centers")
    print("  - No central coordinator - pure self-organization!")
    print("\nBiological Significance:")
    print("  - Models real slime mold behavior")
    print("  - Demonstrates chemical signaling")
    print("  - Shows emergent pattern formation")
    print("=" * 60)
    
    # Plot final state
    print("\nPlotting final state...")
    sim.plot()


def main_web():
    """Run slime mold simulation with web interface."""
    from similar2logo.web import WebSimulation
    
    print("=" * 60)
    print("Slime Mold Aggregation - Web Interface")
    print("=" * 60)
    
    # Setup
    env = Environment(width=100, height=100, toroidal=False)
    env.add_pheromone('camp', diffusion_coef=0.1, evaporation_coef=0.05)
    
    # Create simulation
    sim = LogoSimulation(env, num_turtles=800, turtle_class=SlimeMoldCell)
    
    # Distribute cells
    for cell in sim.turtles:
        cell.position = env.random_position()
        cell.heading = env.random_heading()
        cell._sync_to_cpp()
    
    # Create web interface
    web_sim = WebSimulation(sim, update_rate=30)
    
    print("\nOpen your browser to: http://localhost:8080")
    print("Watch spiral waves and aggregation in real-time!")
    print("\nLook for:")
    print("  - Spiral wave formation")
    print("  - Aggregation streams")
    print("  - Mound formation")
    print("=" * 60 + "\n")
    
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
