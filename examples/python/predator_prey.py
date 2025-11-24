"""
Predator-Prey Model (Lotka-Volterra) - Population dynamics

This implements a spatial version of the classic Lotka-Volterra predator-prey
model, demonstrating:
- Population cycles
- Spatial patterns
- Energy-based interactions
- Emergent ecosystem dynamics

Reference: Lotka, A. J. (1925). Elements of physical biology.
"""

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D, MathUtil
import random


class Prey(Turtle):
    """
    Prey agent (e.g., rabbit, sheep).
    
    Behavior:
        - Wander and eat grass
        - Reproduce when energy is high
        - Die when energy reaches zero
        - Flee from predators
    """
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.energy = 50.0
        self.max_energy = 100.0
        self.reproduction_threshold = 80.0
        self.reproduction_cost = 40.0
        self.movement_cost = 0.5
        self.grass_energy = 10.0
        self.perception_radius = 10.0
        
        self.color = "green"
        self.speed = 1.0
    
    def decide(self, perception):
        """Prey decision logic."""
        influences = []
        env = perception['environment']
        
        # Check for predators nearby
        predators_nearby = self._detect_predators(perception)
        
        if predators_nearby:
            # Flee from predators
            influences.extend(self._flee_from_predators(predators_nearby))
        else:
            # Normal behavior: wander and eat
            influences.extend(self._wander_and_eat(env))
        
        # Consume energy for movement
        self.energy -= self.movement_cost
        
        # Try to reproduce if energy is high
        if self.energy >= self.reproduction_threshold:
            influences.extend(self._reproduce(perception))
        
        # Die if out of energy
        if self.energy <= 0:
            influences.extend(self._die(perception))
            
        return influences
    
    def _detect_predators(self, perception):
        """Detect nearby predators."""
        nearby_data = perception.get('nearby_turtles', [])
        predators = []
        for item in nearby_data:
            if isinstance(item['turtle'], Predator):
                predators.append(item['turtle'])
        return predators
    
    def _flee_from_predators(self, predators):
        """Flee away from predators."""
        influences = []
        if len(predators) > 0:
            avg_x = sum(p.position.x for p in predators) / len(predators)
            avg_y = sum(p.position.y for p in predators) / len(predators)
            predator_pos = Point2D(avg_x, avg_y)
            
            # Move away from predators
            flee_direction = MathUtil.normalize_angle(
                self._environment.get_direction(predator_pos, self.position)
            )
            influences.append(self.influence_turn_towards(flee_direction))
            influences.append(self.influence_move_forward(self.speed * 1.5))
        return influences
    
    def _wander_and_eat(self, env):
        """Wander randomly and eat grass."""
        influences = []
        
        # Random walk
        if random.random() < 0.1:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        
        # Eat grass (check grass pheromone)
        x, y = int(self.position.x), int(self.position.y)
        try:
            grass = env.get_pheromone_value(x, y, 'grass')
            if grass > 0:
                # Eat grass
                eat_amount = min(grass, self.grass_energy)
                self.energy = min(self.energy + eat_amount, self.max_energy)
                # Emit negative pheromone to consume grass
                influences.append(self.influence_emit_pheromone('grass', -eat_amount))
        except:
            pass
        
        influences.append(self.influence_move_forward(self.speed))
        return influences
    
    def _reproduce(self, perception):
        """Create offspring."""
        influences = []
        # Deduct energy cost
        self.energy -= self.reproduction_cost
        
        # Create new prey
        from similar2logo.influences import SystemInfluenceAddAgent
        offspring = Prey()
        offspring.position = Point2D(self.position.x, self.position.y)
        offspring.heading = random.uniform(0, MathUtil.TWO_PI)
        offspring._environment = self._environment
        
        influences.append(SystemInfluenceAddAgent(offspring))
        return influences
    
    def _die(self, perception):
        """Remove from simulation."""
        from similar2logo.influences import SystemInfluenceRemoveAgent
        return [SystemInfluenceRemoveAgent(self)]


class Predator(Turtle):
    """
    Predator agent (e.g., wolf, fox).
    """
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.energy = 100.0
        self.max_energy = 200.0
        self.reproduction_threshold = 150.0
        self.reproduction_cost = 80.0
        self.movement_cost = 1.0
        self.hunt_energy_gain = 50.0
        self.perception_radius = 15.0
        
        self.color = "red"
        self.speed = 1.2
    
    def decide(self, perception):
        """Predator decision logic."""
        influences = []
        
        # Look for prey
        prey_nearby = self._detect_prey(perception)
        
        if prey_nearby:
            # Chase nearest prey
            influences.extend(self._chase_prey(prey_nearby))
        else:
            # Wander
            influences.extend(self._wander())
        
        # Consume energy
        self.energy -= self.movement_cost
        
        # Try to reproduce
        if self.energy >= self.reproduction_threshold:
            influences.extend(self._reproduce(perception))
        
        # Die if out of energy
        if self.energy <= 0:
            influences.extend(self._die(perception))
            
        return influences
    
    def _detect_prey(self, perception):
        """Detect nearby prey."""
        nearby_data = perception.get('nearby_turtles', [])
        prey = []
        for item in nearby_data:
            if isinstance(item['turtle'], Prey):
                prey.append(item['turtle'])
        return prey
    
    def _chase_prey(self, prey_list):
        """Chase the nearest prey."""
        influences = []
        if len(prey_list) > 0:
            # Find nearest prey
            nearest = min(prey_list, key=lambda p: self.position.distance(p.position))
            
            # Move towards prey
            direction = self._environment.get_direction(self.position, nearest.position)
            influences.append(self.influence_turn_towards(direction))
            influences.append(self.influence_move_forward(self.speed))
            
            # Check if caught prey
            if self.position.distance(nearest.position) < 2.0:
                influences.extend(self._catch_prey(nearest))
        return influences
    
    def _catch_prey(self, prey):
        """Catch and eat prey."""
        influences = []
        self.energy = min(self.energy + self.hunt_energy_gain, self.max_energy)
        
        # Signal prey to die
        from similar2logo.influences import SystemInfluenceRemoveAgent
        influences.append(SystemInfluenceRemoveAgent(prey))
        return influences
    
    def _wander(self):
        """Random walk."""
        influences = []
        if random.random() < 0.1:
            influences.append(self.influence_turn(random.uniform(-0.5, 0.5)))
        influences.append(self.influence_move_forward(self.speed))
        return influences
    
    def _reproduce(self, perception):
        """Create offspring."""
        influences = []
        self.energy -= self.reproduction_cost
        
        from similar2logo.influences import SystemInfluenceAddAgent
        offspring = Predator()
        offspring.position = Point2D(self.position.x, self.position.y)
        offspring.heading = random.uniform(0, MathUtil.TWO_PI)
        offspring._environment = self._environment
        
        influences.append(SystemInfluenceAddAgent(offspring))
        return influences
    
    def _die(self, perception):
        """Remove from simulation."""
        from similar2logo.influences import SystemInfluenceRemoveAgent
        return [SystemInfluenceRemoveAgent(self)]


def main():
    """Run the predator-prey simulation."""
    print("=" * 60)
    print("Predator-Prey Model (Lotka-Volterra)")
    print("=" * 60)
    print("\nSpatial version of the classic population dynamics model.")
    print("\nComponents:")
    print("  - Prey (green): Eat grass, reproduce, flee from predators")
    print("  - Predators (red): Hunt prey, reproduce, die without food")
    print("  - Grass: Renewable resource for prey")
    print("\nExpected Dynamics:")
    print("  - Population cycles (predator-prey oscillations)")
    print("  - Spatial patterns (waves, clusters)")
    print("  - Potential extinction if imbalanced")
    print("=" * 60 + "\n")
    
    # Create environment
    print("Creating environment...")
    env = Environment(width=100, height=100, toroidal=True)
    
    # Add grass pheromone
    print("Adding grass resource...")
    env.add_pheromone('grass', diffusion_coef=0.0, evaporation_coef=0.0)
    
    # Initialize grass everywhere
    print("Planting grass...")
    for x in range(env.width):
        for y in range(env.height):
            env.set_pheromone(x, y, 'grass', 100.0)
    
    # Create simulation
    print("Creating simulation...")
    sim = LogoSimulation(env)
    
    # Add prey
    print("Adding 100 prey (green)...")
    sim.add_turtles(100, turtle_class=Prey)
    
    # Add predators
    print("Adding 20 predators (red)...")
    sim.add_turtles(20, turtle_class=Predator)
    
    print("\nRunning simulation...")
    print("Observing population dynamics...")
    print()
    
    # Track populations
    prey_history = []
    predator_history = []
    
    def progress(sim, step):
        if step % 50 == 0:
            prey_count = sum(1 for t in sim.turtles if isinstance(t, Prey))
            predator_count = sum(1 for t in sim.turtles if isinstance(t, Predator))
            prey_history.append(prey_count)
            predator_history.append(predator_count)
            print(f"  Step {step:4d}: Prey={prey_count:3d}, Predators={predator_count:3d}")
    
    # Run simulation
    sim.run(steps=1000, callback=progress)
    
    print("\n" + "=" * 60)
    print("Simulation complete!")
    print("\nFinal populations:")
    prey_count = sum(1 for t in sim.turtles if isinstance(t, Prey))
    predator_count = sum(1 for t in sim.turtles if isinstance(t, Predator))
    print(f"  Prey: {prey_count}")
    print(f"  Predators: {predator_count}")
    print("\nKey Observations:")
    print("  - Populations oscillate (predator-prey cycles)")
    print("  - Spatial clustering emerges")
    print("  - System self-regulates (usually)")
    print("=" * 60)
    
    # Plot final state
    print("\nPlotting final state...")
    sim.plot()


def main_web():
    """Run predator-prey model with web interface."""
    from similar2logo.web import WebSimulation
    
    print("=" * 60)
    print("Predator-Prey Model - Web Interface")
    print("=" * 60)
    
    # Setup
    env = Environment(width=100, height=100, toroidal=True)
    env.add_pheromone('grass', diffusion_coef=0.0, evaporation_coef=0.0)
    
    # Plant grass
    for x in range(env.width):
        for y in range(env.height):
            env.set_pheromone(x, y, 'grass', 100.0)
    
    # Create simulation
    sim = LogoSimulation(env)
    sim.add_turtles(150, turtle_class=Prey)
    sim.add_turtles(30, turtle_class=Predator)
    
    # Create web interface
    web_sim = WebSimulation(sim, update_rate=30)
    
    print("\nOpen your browser to: http://localhost:8080")
    print("Watch population dynamics unfold in real-time!")
    print("\nLook for:")
    print("  - Predator-prey cycles")
    print("  - Spatial waves and patterns")
    print("  - Clustering behavior")
    print("=" * 60 + "\n")
    
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
