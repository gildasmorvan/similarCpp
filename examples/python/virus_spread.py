"""
Virus Spread Simulation - SIMILAR2Logo Python DSL

A simulation of virus spread through a population of agents.
Agents can be:
- Healthy (green)
- Infected (red)  
- Recovered/Immune (blue)

Based on the Java example from SIMILAR2Logo.
"""

import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.dsl import *
import random

# Simulation parameters
GRID_SIZE = 100
NUM_AGENTS = 200
INFECTION_RADIUS = 3.0
INFECTION_PROBABILITY = 0.05
RECOVERY_TIME = 100  # steps
INITIAL_INFECTED = 5

# Agent states
STATE_HEALTHY = "healthy"
STATE_INFECTED = "infected"
STATE_RECOVERED = "recovered"

class VirusAgent(Turtle):
    """Agent that can be healthy, infected, or recovered."""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.state = STATE_HEALTHY
        self.infection_time = 0
        self.update_color()
    
    def update_color(self):
        """Update color based on state."""
        if self.state == STATE_HEALTHY:
            self.color = "green"
        elif self.state == STATE_INFECTED:
            self.color = "red"
        else:  # recovered
            self.color = "blue"
    
    def infect(self):
        """Infect this agent."""
        if self.state == STATE_HEALTHY:
            self.state = STATE_INFECTED
            self.infection_time = 0
            self.update_color()
    
    def decide(self, perception):
        """Agent behavior: random walk and potential infection/recovery."""
        influences = []
        
        # Random walk
        if random.random() < 0.3:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        influences.append(self.influence_move_forward(1.0))
        
        # Handle infection state
        if self.state == STATE_INFECTED:
            self.infection_time += 1
            
            # Try to infect nearby healthy agents
            for neighbor_info in perception['nearby_turtles']:
                neighbor = neighbor_info['turtle']
                distance = neighbor_info['distance']
                
                if (distance < INFECTION_RADIUS and 
                    neighbor.state == STATE_HEALTHY and
                    random.random() < INFECTION_PROBABILITY):
                    neighbor.infect()
            
            # Recover after infection period
            if self.infection_time >= RECOVERY_TIME:
                self.state = STATE_RECOVERED
                self.update_color()
        
        return influences


def create_virus_simulation():
    """Create and configure the virus spread simulation."""
    
    # Create environment
    env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
    
    # Create simulation
    sim = Simulation(
        environment=env,
        title="Virus Spread Simulation"
    )
    
    # Add agents
    for i in range(NUM_AGENTS):
        agent = VirusAgent(
            position=env.random_position(),
            heading=env.random_heading()
        )
        # Infect initial agents
        if i < INITIAL_INFECTED:
            agent.infect()
        sim.add_agent(agent)
    
    return sim


def main():
    """Run the virus spread simulation."""
    print("=" * 60)
    print("Virus Spread Simulation - Python DSL")
    print("=" * 60)
    print(f"Grid size: {GRID_SIZE}x{GRID_SIZE}")
    print(f"Number of agents: {NUM_AGENTS}")
    print(f"Initial infected: {INITIAL_INFECTED}")
    print(f"Infection radius: {INFECTION_RADIUS}")
    print(f"Infection probability: {INFECTION_PROBABILITY}")
    print(f"Recovery time: {RECOVERY_TIME} steps")
    print("=" * 60)
    print()
    
    sim = create_virus_simulation()
    
    # Run with web interface
    sim.run_web(
        port=8080,
        steps_per_frame=1,
        description="""
        # Virus Spread Simulation
        
        Watch how a virus spreads through a population:
        - **Green**: Healthy agents
        - **Red**: Infected agents  
        - **Blue**: Recovered/immune agents
        
        Infected agents can spread the virus to nearby healthy agents.
        After a period, infected agents recover and become immune.
        """
    )


if __name__ == "__main__":
    main()
