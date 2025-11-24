"""
Firework Simulation - SIMILAR2Logo Python DSL

Colorful firework particles that explode and fade.
Demonstrates particle systems and visual effects.

Based on the SIMILAR2Logo Java example.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.dsl import *
import random
import math

# Simulation parameters
GRID_SIZE = 150
LAUNCH_INTERVAL = 20  # steps between launches
PARTICLE_LIFETIME = 50  # steps
EXPLOSION_PARTICLES = 30
GRAVITY = 0.05

class FireworkParticle(Turtle):
    """A single firework particle."""
    
    def __init__(self, is_rocket=False, lifetime=PARTICLE_LIFETIME, **kwargs):
        super().__init__(**kwargs)
        self.is_rocket = is_rocket
        self.lifetime = lifetime
        self.age = 0
        self.velocity_x = 0
        self.velocity_y = 0
        self.exploded = False
        
        if is_rocket:
            # Rocket goes up
            self.color = "white"
            self.velocity_y = -2.0  # Upward
            self.velocity_x = random.uniform(-0.2, 0.2)
        else:
            # Explosion particle
            colors = ["red", "orange", "yellow", "blue", "purple", "green", "pink"]
            self.color = random.choice(colors)
    
    def decide(self, perception):
        """Update particle physics."""
        influences = []
        
        self.age += 1
        
        # Apply gravity
        self.velocity_y += GRAVITY
        
        # Move based on velocity
        influences.append(
            self.influence_move_forward(0)  # We'll use direct position change
        )
        
        # Update position directly (simpler than velocity-based movement)
        new_x = self.position.x + self.velocity_x
        new_y = self.position.y + self.velocity_y
        
        # Create position change influence
        from similar2logo.tools import Point2D
        influences.append(
            self.influence_move_to(Point2D(new_x, new_y))
        )
        
        # Rocket explodes at peak
        if self.is_rocket and not self.exploded and self.velocity_y > 0:
            self.exploded = True
            # Signal explosion (will be handled by simulation)
            self._should_explode = True
            self._explosion_pos = Point2D(self.position.x, self.position.y)
        
        # Fade out (could adjust color alpha if supported)
        if self.age > self.lifetime:
            self._should_remove = True
        
        return influences


class FireworkSimulation(Simulation):
    """Custom simulation that handles firework launches and explosions."""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.steps_since_launch = 0
    
    def step(self):
        """Override step to handle launches and explosions."""
        # Launch new rocket periodically
        self.steps_since_launch += 1
        if self.steps_since_launch >= LAUNCH_INTERVAL:
            self.launch_rocket()
            self.steps_since_launch = 0
        
        # Check for explosions
        particles_to_add = []
        agents_to_remove = []
        
        for agent in self.turtles:
            if hasattr(agent, '_should_explode') and agent._should_explode:
                # Create explosion
                particles_to_add.extend(
                    self.create_explosion(agent._explosion_pos, agent.color)
                )
                agents_to_remove.append(agent)
                agent._should_explode = False
            
            if hasattr(agent, '_should_remove') and agent._should_remove:
                agents_to_remove.append(agent)
        
        # Add new particles
        for particle in particles_to_add:
            self.add_agent(particle)
        
        # Remove old particles
        for agent in agents_to_remove:
            if agent in self.turtles:
                self.turtles.remove(agent)
        
        # Run normal step
        super().step()
    
    def launch_rocket(self):
        """Launch a new rocket from the bottom."""
        x = random.uniform(20, self.environment.width - 20)
        y = self.environment.height - 10
        
        from similar2logo.tools import Point2D
        rocket = FireworkParticle(
            is_rocket=True,
            position=Point2D(x, y),
            heading=0
        )
        self.add_agent(rocket)
    
    def create_explosion(self, position, base_color):
        """Create explosion particles at position."""
        particles = []
        
        for i in range(EXPLOSION_PARTICLES):
            angle = (2 * math.pi * i) / EXPLOSION_PARTICLES
            speed = random.uniform(1.0, 3.0)
            
            particle = FireworkParticle(
                is_rocket=False,
                position=position,
                heading=angle
            )
            particle.velocity_x = speed * math.sin(angle)
            particle.velocity_y = speed * math.cos(angle)
            
            particles.append(particle)
        
        return particles


def create_firework_simulation():
    """Create and configure the firework simulation."""
    
    # Create environment
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=False)
    
    # Create custom simulation
    sim = FireworkSimulation(
        environment=env,
        title="Firework Display"
    )
    
    return sim


def main():
    """Run the firework simulation."""
    print("=" * 60)
    print("Firework Simulation - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Launch interval: {LAUNCH_INTERVAL} steps")
    print(f"Particles per explosion: {EXPLOSION_PARTICLES}")
    print("=" * 60)
    print()
    
    sim = create_firework_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=1,
        description="""
        # Firework Display
        
        Watch colorful fireworks launch and explode!
        
        - White rockets launch from the bottom
        - They explode at their peak into colorful particles
        - Particles fall under gravity and fade away
        
        A beautiful demonstration of particle physics!
        """
    )


if __name__ == "__main__":
    main()
