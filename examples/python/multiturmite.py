"""
Multi-Turmite Simulation - SIMILAR2Logo Python Example

Multiple Langton's Ants (turmites) creating complex emergent patterns.
Each turmite follows simple rules but creates intricate highways.

Includes custom collision handling when multiple turmites interact.

Based on the SIMILAR2Logo Jython example.
"""

import random
import math
from similar2logo import LogoSimulation, Turtle
from similar2logo.environment import Environment
from similar2logo.tools import Point2D
from similar2logo._core import SimpleMark, SimulationTimeStamp
from similar2logo._core.influences import DropMark, RemoveMark
from similar2logo.reaction import LogoReactionModel

# Import Turmite class from single turmite example
from turmite import Turmite


class MultiTurmiteParameters:
    """Parameters for multiturmite simulation collision handling"""
    
    def __init__(self):
        # If True, remove direction changes when collision detected
        self.remove_direction_change = False
        # If True, inverse mark update behavior on collision
        self.inverse_mark_update = False
        self.num_turmites = 4
        self.grid_size = 100


class TurmiteInteraction:
    """Tracks influences from multiple turmites at the same location"""
    
    def __init__(self):
        self.drop_marks = []
        self.remove_marks = []
        self.change_directions = []
    
    def is_colliding(self):
        """Check if multiple turmites are trying to affect the same cell"""
        return len(self.remove_marks) > 1 or len(self.drop_marks) > 1


class MultiTurmiteReactionModel(LogoReactionModel):
    """
    Custom reaction model that handles collisions when multiple turmites
    are at the same location.
    
    Based on the Jython MultiTurmiteReactionModel.
    """
    
    def __init__(self, parameters):
        super().__init__()
        self.parameters = parameters
    
    def make_regular_reaction(self, time_min, time_max, environment, influences_map):
        """
        Override regular reaction to handle turmite collisions.
        
        When multiple turmites are at the same location, we need to decide
        which influences to apply based on the collision parameters.
        """
        # Group influences by location
        collisions = {}
        non_specific_influences = []
        
        # Get all influences from the map
        all_influences = influences_map.get_all()
        
        for influence in all_influences:
            # Check if it's a mark-related influence
            if hasattr(influence, 'getMark'):
                # DropMark or RemoveMark
                mark = influence.getMark()
                if mark:
                    loc = mark.get_location()
                    # Use grid cell as key
                    cell_key = (int(loc.x), int(loc.y))
                    
                    if cell_key not in collisions:
                        collisions[cell_key] = TurmiteInteraction()
                    
                    # Determine influence type by checking methods
                    if type(influence).__name__ == 'DropMark':
                        collisions[cell_key].drop_marks.append(influence)
                    elif type(influence).__name__ == 'RemoveMark':
                        collisions[cell_key].remove_marks.append(influence)
            
            elif hasattr(influence, 'getTarget'):
                # ChangeDirection or other turtle-targeted influence
                target = influence.getTarget()
                if target:
                    loc = target.getLocation()
                    cell_key = (int(loc.x), int(loc.y))
                    
                    if cell_key not in collisions:
                        collisions[cell_key] = TurmiteInteraction()
                    
                    if type(influence).__name__ == 'ChangeDirection':
                        collisions[cell_key].change_directions.append(influence)
                    else:
                        non_specific_influences.append(influence)
            else:
                # Other influences (not location-specific)
                non_specific_influences.append(influence)
        
        # Process collisions
        for collision in collisions.values():
            if collision.is_colliding():
                # Collision detected - apply collision rules
                if collision.drop_marks and not self.parameters.inverse_mark_update:
                    # Keep only first drop mark
                    non_specific_influences.append(collision.drop_marks[0])
                
                if collision.remove_marks and not self.parameters.inverse_mark_update:
                    # Keep only first remove mark
                    non_specific_influences.append(collision.remove_marks[0])
                
                if not self.parameters.remove_direction_change:
                    # Keep all direction changes even on collision
                    non_specific_influences.extend(collision.change_directions)
            else:
                # No collision - apply all influences
                non_specific_influences.extend(collision.change_directions)
                if collision.drop_marks:
                    non_specific_influences.append(collision.drop_marks[0])
                if collision.remove_marks:
                    non_specific_influences.append(collision.remove_marks[0])
        
        # Call parent's regular reaction with filtered influences
        from similar2logo.reaction import InfluencesMap
        filtered_map = InfluencesMap()
        for inf in non_specific_influences:
            filtered_map.add(inf)
        
        super().make_regular_reaction(time_min, time_max, environment, filtered_map)


def main():
    """Run simulation in console mode"""
    print("=" * 60)
    print("Multi-Turmite Simulation with Collision Handling")
    print("=" * 60)
    
    # Create parameters
    params = MultiTurmiteParameters()
    params.num_turmites = 4
    params.grid_size = 100
    params.remove_direction_change = False  # Keep direction changes on collision
    params.inverse_mark_update = False  # Apply first mark influence on collision
    
    # Create environment
    env = Environment(params.grid_size, params.grid_size, toroidal=True)
    
    # Create simulation with custom reaction model
    sim = LogoSimulation(
        environment=env,
        reaction_model=MultiTurmiteReactionModel(params)
    )
    
    # Add turmites at different starting positions
    colors = ["red", "blue", "green", "yellow", "purple"]
    positions = [
        (params.grid_size // 2, params.grid_size // 2),
        (params.grid_size // 2, params.grid_size // 2 + 1),
        (params.grid_size // 2 + 10, params.grid_size // 2),
        (params.grid_size // 2 + 10, params.grid_size // 2 + 1)
    ]
    headings = [0, math.pi, 0, math.pi]  # North, South, North, South
    
    for i in range(params.num_turmites):
        x, y = positions[i]
        turmite = Turmite(
            x=float(x),
            y=float(y),
            heading=headings[i],
            environment=sim.environment,
            color=colors[i % len(colors)]
        )
        sim.turtles.append(turmite)
    
    # Run simulation
    print(f"Running simulation for 10000 steps...")
    print(f"Collision handling: remove_direction_change={params.remove_direction_change}, inverse_mark_update={params.inverse_mark_update}")
    for step in range(10000):
        sim.step()
        if step % 1000 == 0:
            state = sim.get_state()
            print(f"  Step {step}/10000")
    
    print("\nSimulation complete!")
    state = sim.get_state()
    print(f"Final marks created: {len(state['marks'])}")
    print("=" * 60)


def main_web(port=8080):
    """Run simulation with web interface"""
    from similar2logo.web.server import SimulationServer
    
    # Create parameters
    params = MultiTurmiteParameters()
    params.num_turmites = 4
    params.grid_size = 100
    params.remove_direction_change = False
    params.inverse_mark_update = False
    
    # Create environment
    env = Environment(params.grid_size, params.grid_size, toroidal=True)
    
    # Create simulation with custom reaction model
    sim = LogoSimulation(
        environment=env,
        reaction_model=MultiTurmiteReactionModel(params)
    )
    
    # Add turmites at different starting positions
    colors = ["red", "blue", "green", "yellow", "purple"]
    positions = [
        (params.grid_size // 2, params.grid_size // 2),
        (params.grid_size // 2, params.grid_size // 2 + 1),
        (params.grid_size // 2 + 10, params.grid_size // 2),
        (params.grid_size // 2 + 10, params.grid_size // 2 + 1)
    ]
    headings = [0, math.pi, 0, math.pi]  # North, South, North, South
    
    for i in range(params.num_turmites):
        x, y = positions[i]
        turmite = Turmite(
            x=float(x),
            y=float(y),
            heading=headings[i],
            environment=sim.environment,
            color=colors[i % len(colors)]
        )
        sim.turtles.append(turmite)
    
    # Start web server
    server = SimulationServer(sim, port=port)
    print(f"Starting Multi-Turmite simulation with collision handling on http://localhost:{port}")
    print(f"Collision parameters: remove_direction_change={params.remove_direction_change}, inverse_mark_update={params.inverse_mark_update}")
    print("Press Ctrl+C to stop")
    server.run()


if __name__ == "__main__":
    import sys
    if "--web" in sys.argv:
        port = 8080
        if "--port" in sys.argv:
            port_idx = sys.argv.index("--port") + 1
            if port_idx < len(sys.argv):
                port = int(sys.argv[port_idx])
        main_web(port=port)
    else:
        main()
