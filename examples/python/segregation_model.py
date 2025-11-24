"""
Schelling's Segregation Model - Emergent segregation from mild preferences

This implementation follows the structure of the original SIMILAR2Logo examples.
"""

import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI
import random
from typing import List, Optional

from similar2logo import LogoSimulation, Turtle, Environment
from similar2logo.tools import Point2D
from similar2logo.influences import RegularInfluence, InfluencesMap
from similar2logo.reaction import LogoReactionModel


class SegregationParameters:
    """Parameters for the segregation model."""
    
    def __init__(self):
        self.similarity_rate = 3.0/8  # Threshold for satisfaction (37.5%)
        self.vacancy_rate = 0.05      # Percentage of empty cells
        self.perception_distance = sqrt(2) * 1.5 # Radius to look for neighbors
        self.grid_width = 50
        self.grid_height = 50


class Move(RegularInfluence):
    """Influence to request a move to a vacant spot."""
    
    CATEGORY = 'move'
    
    def __init__(self, agent):
        super().__init__(agent)
        self.agent = agent
    
    def get_category(self) -> str:
        return self.CATEGORY
    
    def __repr__(self):
        return f"Move(agent={self.agent})"


class SegregationTurtle(Turtle):
    """Resident agent in the segregation model."""
    
    def __init__(self, group, parameters, **kwargs):
        super().__init__(**kwargs)
        self.group = group
        self.parameters = parameters
        self.satisfied = False
        
        # Set color based on group
        self.color = "red" if group == 'A' else "blue"
    
    def decide(self, perception) -> List[RegularInfluence]:
        """
        Check satisfaction. If unsatisfied, request to move.
        """
        influences = []
        
        # Get neighbors from perception
        neighbors = self._get_neighbors(perception)
        
        if not neighbors:
            # No neighbors, satisfied by default? Or unsatisfied?
            # Schelling model usually implies satisfied if isolated or ratio met
            self.satisfied = True
            return influences
        
        # Count similar neighbors
        similar_count = sum(1 for n in neighbors if hasattr(n, 'group') and n.group == self.group)
        similarity_ratio = similar_count / len(neighbors)
        
        # Check if satisfied
        if similarity_ratio >= self.parameters.similarity_rate:
            self.satisfied = True
        else:
            # Not satisfied, request to move
            self.satisfied = False
            influences.append(Move(self))
            
        return influences
    
    def _get_neighbors(self, perception):
        """Get all neighbors within perception radius."""
        nearby_data = perception.get('nearby_turtles', [])
        neighbors = []
        
        for item in nearby_data:
            if item['distance'] <= self.parameters.perception_distance:
                neighbors.append(item['turtle'])
                
        return neighbors


class SegregationReactionModel(LogoReactionModel):
    """Reaction model that handles Move influences by finding vacant spots."""
    
    def __init__(self, parameters):
        super().__init__()
        self.parameters = parameters
    
    def make_regular_reaction(self, time_min: int, time_max: int,
                              environment: Environment,
                              influences: InfluencesMap):
        """
        Process influences. Handle Move influences specially.
        """
        # This is called from the custom step() method which also calls process_moves
        # So we just call the parent for standard influence processing
        super().make_regular_reaction(time_min, time_max, environment, influences)

    def process_moves(self, environment, move_influences, all_turtles):
        """
        Process move influences knowing all turtle positions.
        """
        # Build occupied set
        occupied = set()
        for t in all_turtles:
            occupied.add((int(t.position.x), int(t.position.y)))
            
        # Find vacant places
        vacant_places = []
        for x in range(self.parameters.grid_width):
            for y in range(self.parameters.grid_height):
                if (x, y) not in occupied:
                    vacant_places.append(Point2D(x, y))
        
        if not vacant_places:
            return # No space to move
            
        random.shuffle(vacant_places)
        random.shuffle(move_influences)
        
        # Move agents
        for i, influence in enumerate(move_influences):
            if i >= len(vacant_places):
                break
            
            target_pos = vacant_places[i]
            agent = influence.agent
            
            # Update occupied set (old pos becomes free, new pos becomes occupied)
            # But we process all at once, so we just assign from the vacant list
            agent.position = target_pos
            # Note: We modify agent position directly here, which is what the reaction model does.


class SegregationSimulation(LogoSimulation):
    """Segregation simulation controller."""
    
    def __init__(self, parameters):
        env = Environment(parameters.grid_width, parameters.grid_height, toroidal=False)
        super().__init__(env)
        self.parameters = parameters
        self.reaction_model = SegregationReactionModel(parameters)
        
        self.generate_agents()
        
    def generate_agents(self):
        """Generate agents based on parameters."""
        for x in range(self.parameters.grid_width):
            for y in range(self.parameters.grid_height):
                if random.random() >= self.parameters.vacancy_rate:
                    group = 'A' if random.random() < 0.5 else 'B'
                    
                    turtle = SegregationTurtle(
                        group=group,
                        parameters=self.parameters,
                        position=Point2D(x, y)
                    )
                    turtle._environment = self.environment
                    self.turtles.append(turtle)

    def step(self):
        """Override step to pass turtles to reaction model."""
        # Phase 0: Rebuild spatial index
        self.spatial_index.rebuild(self.turtles)
        
        # Notify probes before step
        self.probe_manager.notify_step(self.current_step, self)
        
        # Phase 1: Perception
        perceptions = {}
        for turtle in self.turtles:
            perceptions[turtle] = self._build_perception(turtle)
        
        # Phase 2: Decision
        influences_map = InfluencesMap()
        for turtle in self.turtles:
            turtle_influences = turtle.decide(perceptions[turtle])
            if turtle_influences:
                influences_map.add_all(turtle_influences)
        
        # Phase 3: Reaction
        # Custom processing for moves
        move_influences = [inf for inf in influences_map.get_all_regular() 
                          if isinstance(inf, Move)]
        
        if move_influences:
            self.reaction_model.process_moves(self.environment, move_influences, self.turtles)
            
        # Standard processing for other influences (if any)
        self.reaction_model.make_regular_reaction(
            self.current_step,
            self.current_step + 1,
            self.environment,
            influences_map
        )
        
        self.current_step += 1


def main():
    print("=" * 60)
    print("Schelling's Segregation Model (Refactored)")
    print("=" * 60)
    
    params = SegregationParameters()
    sim = SegregationSimulation(params)
    
    print(f"Grid: {params.grid_width}x{params.grid_height}")
    print(f"Agents: {len(sim.turtles)}")
    print(f"Similarity Threshold: {params.similarity_rate:.2%}")
    print("\nRunning simulation...")
    
    def progress(sim, step):
        if step % 10 == 0:
            satisfied = sum(1 for t in sim.turtles if t.satisfied)
            pct = (satisfied / len(sim.turtles)) * 100
            print(f"Step {step:3d}: {pct:.1f}% satisfied")
            
    sim.run(steps=100, callback=progress)
    
    print("\nFinal State:")
    satisfied = sum(1 for t in sim.turtles if t.satisfied)
    print(f"Satisfied: {satisfied}/{len(sim.turtles)} ({satisfied/len(sim.turtles)*100:.1f}%)")
    
    sim.plot()


def main_web():
    from similar2logo.web import WebSimulation
    
    print("=" * 60)
    print("Schelling's Segregation Model - Web Interface")
    print("=" * 60)
    print("\nInitializing simulation...")
    
    params = SegregationParameters()
    sim = SegregationSimulation(params)
    
    print(f"Grid: {params.grid_width}x{params.grid_height}")
    print(f"Agents: {len(sim.turtles)}")
    print(f"Similarity Threshold: {params.similarity_rate:.2%}")
    print("\nStarting web server...")
    print("Open your browser to: http://localhost:8080")
    print("=" * 60)
    print()
    
    web_sim = WebSimulation(sim, update_rate=10, initial_speed=10.0)
    web_sim.start_server(port=8080)


if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "--web":
        main_web()
    else:
        main()
