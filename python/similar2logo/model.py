"""
Core simulation model with Turtle base class and LogoSimulation controller.

This implements the proper SIMILAR influence/reaction architecture.
"""

import random
import math
from typing import List
from .tools import Point2D, MathUtil
from .environment import Environment
from .influences import *
from .reaction import LogoReactionModel

try:
    from ._core.environment import TurtlePLS
    from ._core.influences import (
        ChangePosition as CppChangePosition,
        ChangeDirection as CppChangeDirection,
        ChangeSpeed as CppChangeSpeed,
        Stop as CppStop,
        EmitPheromone as CppEmitPheromone
    )
    from ._core import SimulationTimeStamp, LevelIdentifier
    HAS_CPP_CORE = True
except ImportError:
    HAS_CPP_CORE = False


class Turtle:
    """
    Base class for Logo turtles (agents).
    
    Subclass this and override the `decide` method to define custom behavior.
    The decide method should return a list of influences, NOT modify the turtle directly.
    
    Attributes:
        position: Current position (Point2D)
        heading: Current heading in radians (0 = north)
        color: Visual color
        speed: Movement speed
        pen_down: Whether turtle draws when moving
    
    Examples:
        >>> class MyTurtle(Turtle):
        ...     def decide(self, perception):
        ...         influences = []
        ...         influences.append(self.influence_move_forward(1.0))
        ...         influences.append(self.influence_turn(0.1))
        ...         return influences
    """
    
    def __init__(self, position=None, heading=0.0, color="black", **kwargs):
        self._position = position if position else Point2D(0, 0)
        self._heading = heading
        self._color = color
        self._speed = 1.0
        self._pen_down = False
        self.acceleration = 0.0
        self._environment = None
        self._id = id(self)  # Unique identifier
        
        if HAS_CPP_CORE:
            from ._core import Point2D as CppPoint2D
            cpp_pos = CppPoint2D(self._position.x, self._position.y)
            self._pls = TurtlePLS(cpp_pos, self._heading, self._pen_down, self._color)
            self._pls.set_speed(self._speed)
    
    @property
    def position(self):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            p = self._pls.get_location()
            return Point2D(p.x, p.y)
        return self._position
    
    @position.setter
    def position(self, value):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            from ._core import Point2D as CppPoint2D
            self._pls.set_location(CppPoint2D(value.x, value.y))
        self._position = value

    @property
    def heading(self):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            return self._pls.get_heading()
        return self._heading
    
    @heading.setter
    def heading(self, value):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            self._pls.set_heading(value)
        self._heading = value

    @property
    def speed(self):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            return self._pls.get_speed()
        return self._speed
    
    @speed.setter
    def speed(self, value):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            self._pls.set_speed(value)
        self._speed = value

    @property
    def color(self):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            return self._pls.get_color()
        return self._color
    
    @color.setter
    def color(self, value):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            self._pls.set_color(value)
        self._color = value

    @property
    def pen_down(self):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            return self._pls.is_pen_down()
        return self._pen_down
    
    @pen_down.setter
    def pen_down(self, value):
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            self._pls.set_pen_down(value)
        self._pen_down = value

    def decide(self, perception) -> List[IInfluence]:
        """
        Override this method to define turtle behavior.
        
        IMPORTANT: Do NOT modify self directly! Return influences instead.
        
        Args:
            perception: Dictionary containing:
                - 'environment': The Environment object
                - 'position': Current position
                - 'heading': Current heading
                - 'time': Current simulation time
                - 'pheromones': Dict of pheromone values at current location
                - 'nearby_turtles': List of nearby turtles
                - 'marks': Marks at current location
        
        Returns:
            List of IInfluence objects representing intended actions
        """
        return []
    
    # ========================================================================
    # Influence Creation Methods
    # ========================================================================
    
    def influence_move_forward(self, distance: float):
        """
        Create an influence to move forward by the given distance.
        
        Args:
            distance: Distance to move
        
        Returns:
            ChangePosition influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            dx = math.sin(self.heading) * distance
            dy = -math.cos(self.heading) * distance
            t = SimulationTimeStamp(0)
            return CppChangePosition(t, t, dx, dy, self._pls)
            
        dx = math.sin(self.heading) * distance
        dy = -math.cos(self.heading) * distance
        return ChangePosition(self, dx=dx, dy=dy)
    
    def influence_move_to(self, target: Point2D):
        """
        Create an influence to move to a specific position.
        
        Args:
            target: Target position
        
        Returns:
            ChangePosition influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            dx = target.x - self.position.x
            dy = target.y - self.position.y
            t = SimulationTimeStamp(0)
            return CppChangePosition(t, t, dx, dy, self._pls)
            
        return ChangePosition(self, target_position=target)
    
    def influence_turn(self, angle: float):
        """
        Create an influence to turn by the given angle.
        
        Args:
            angle: Angle to turn (radians)
        
        Returns:
            ChangeDirection influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            t = SimulationTimeStamp(0)
            return CppChangeDirection(t, t, angle, self._pls)
            
        return ChangeDirection(self, delta_heading=angle)
    
    def influence_turn_towards(self, target_heading: float):
        """
        Create an influence to turn towards a target heading.
        
        Args:
            target_heading: Target heading (radians)
        
        Returns:
            ChangeDirection influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            dd = MathUtil.normalize_angle(target_heading - self.heading)
            t = SimulationTimeStamp(0)
            return CppChangeDirection(t, t, dd, self._pls)
            
        return ChangeDirection(self, target_heading=target_heading)
    
    def influence_set_speed(self, speed: float):
        """
        Create an influence to set speed to a specific value.
        
        Args:
            speed: Target speed
        
        Returns:
            ChangeSpeed influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            ds = speed - self.speed
            t = SimulationTimeStamp(0)
            return CppChangeSpeed(t, t, ds, self._pls)
            
        return ChangeSpeed(self, target_speed=speed)
    
    def influence_change_speed(self, delta_speed: float):
        """
        Create an influence to change speed by a delta.
        
        Args:
            delta_speed: Change in speed
        
        Returns:
            ChangeSpeed influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            t = SimulationTimeStamp(0)
            return CppChangeSpeed(t, t, delta_speed, self._pls)
            
        return ChangeSpeed(self, delta_speed=delta_speed)
    
    def influence_stop(self):
        """
        Create an influence to stop moving.
        
        Returns:
            Stop influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            t = SimulationTimeStamp(0)
            return CppStop(t, t, self._pls)
            
        return Stop(self)
    
    def influence_emit_pheromone(self, pheromone_id: str, 
                                 amount: float):
        """
        Create an influence to emit pheromone.
        
        Args:
            pheromone_id: Identifier of pheromone type
            amount: Amount to emit
        
        Returns:
            EmitPheromone influence
        """
        if HAS_CPP_CORE and hasattr(self, '_pls'):
            t = SimulationTimeStamp(0)
            return CppEmitPheromone(t, t, self._pls.get_location(), pheromone_id, amount)
            
        return EmitPheromone(self, self.position, pheromone_id, amount)
    
    def influence_drop_mark(self, content, category: str = "default") -> DropMark:
        """
        Create an influence to drop a mark.
        
        Args:
            content: Content of the mark
            category: Category of the mark
        
        Returns:
            DropMark influence
        """
        return DropMark(self, self.position, content, category)
    
    # ========================================================================
    # Helper Methods for Common Behaviors
    # ========================================================================
    
    def random_walk_influences(self, turn_amount: float = 0.3) -> List[IInfluence]:
        """
        Generate influences for a random walk behavior.
        
        Args:
            turn_amount: Maximum turn angle
        
        Returns:
            List of influences
        """
        influences = []
        
        if random.random() < 0.1:
            influences.append(self.influence_turn(
                random.uniform(-turn_amount, turn_amount)
            ))
        
        influences.append(self.influence_move_forward(self.speed))
        
        return influences
    
    def __repr__(self):
        return f"Turtle(pos={self.position.to_tuple()}, heading={self.heading:.2f})"




class LogoSimulation:
    """
    Main simulation controller using proper influence/reaction architecture.
    
    Args:
        environment: Environment instance
        num_turtles: Number of turtles to create
        turtle_class: Turtle class to instantiate (default: Turtle)
        parallel_backend: 'thread', 'process', or None (default: 'thread')
        num_workers: Number of parallel workers (None = auto-detect)
    
    Examples:
        >>> env = Environment(100, 100)
        >>> sim = LogoSimulation(env, num_turtles=50)
        >>> sim.run(steps=1000)
    """
    
    def __init__(self, environment, num_turtles=0, turtle_class=None, 
                 parallel_backend='thread', num_workers=None, **turtle_kwargs):
        self.environment = environment
        self.turtles = []
        self.current_step = 0
        self.reaction_model = LogoReactionModel()
        
        # Parallel execution configuration
        self.parallel_backend = parallel_backend
        self.num_workers = num_workers
        self._executor = None
        
        if parallel_backend:
            from .parallel import create_executor
            self._executor = create_executor(parallel_backend, num_workers)
        
        # Store for later use (e.g. adding more agents dynamically)
        self._turtle_class = turtle_class or Turtle
        self._turtle_kwargs = turtle_kwargs
        
        # Spatial indexing for efficient neighbor queries
        # Cell size should be >= typical perception radius
        from .spatial import SpatialHashGrid
        self.spatial_index = SpatialHashGrid(
            cell_size=20.0,  # Adjust based on typical perception radius
            width=environment.width,
            height=environment.height
        )
        
        # Probe system for observation and timing control
        from .probes import ProbeManager
        self.probe_manager = ProbeManager()
        
        if num_turtles > 0:
            self.add_turtles(num_turtles, self._turtle_class, **self._turtle_kwargs)
    
    def add_turtles(self, count, turtle_class=None, **kwargs):
        """Add turtles to the simulation."""
        turtle_class = turtle_class or self._turtle_class
        # Merge stored kwargs with provided kwargs, provided take precedence
        final_kwargs = self._turtle_kwargs.copy()
        final_kwargs.update(kwargs)
        
        for _ in range(count):
            # Extract position/heading if present to avoid duplicates, otherwise generate random
            pos = final_kwargs.pop('position', self.environment.random_position())
            head = final_kwargs.pop('heading', self.environment.random_heading())
            
            turtle = turtle_class(
                position=pos,
                heading=head,
                **final_kwargs
            )
            turtle._environment = self.environment
            self.turtles.append(turtle)

    def step(self):
        """
        Execute one simulation step using influence/reaction architecture.
        
        Steps:
        1. Rebuild spatial index for efficient neighbor queries
        2. Build perceptions for all turtles
        3. Let turtles decide and emit influences (Parallelized)
        4. Process influences through reaction model
        5. Update environment state
        """
        # Phase 0: Rebuild spatial index for this step
        self.spatial_index.rebuild(self.turtles)
        
        # Notify probes before step
        self.probe_manager.notify_step(self.current_step, self)
        
        # Phase 1: Perception - Build perceptions for all turtles
        perceptions = {}
        for turtle in self.turtles:
            perceptions[turtle] = self._build_perception(turtle)
        
        # Phase 2: Decision - Collect influences from all turtles
        # We parallelize this phase as decisions should be independent
        influences_map = InfluencesMap()
        
        if self._executor and self.turtles:
            # Use parallel executor
            all_results = self._executor.map_decisions(self.turtles, perceptions)
            for turtle_influences in all_results:
                if turtle_influences:
                    influences_map.add_all(turtle_influences)
        else:
            # Sequential fallback
            for turtle in self.turtles:
                turtle_influences = turtle.decide(perceptions[turtle])
                if turtle_influences:
                    influences_map.add_all(turtle_influences)
        
        # Phase 3: Reaction - Process influences
        
        # 3a. Regular Reactions (Movement, Pheromones, etc.)
        self.reaction_model.make_regular_reaction(
            self.current_step,
            self.current_step + 1,
            self.environment,
            influences_map
        )
        
        # 3b. System Reactions (Add/Remove Agents)
        # Extract system influences
        system_influences = influences_map.get_all_system()
        
        if system_influences:
            self.reaction_model.make_system_reaction(
                self.current_step,
                self.current_step + 1,
                self.environment,
                system_influences,
                self.turtles
            )
        
        self.current_step += 1
    
    def run(self, steps, callback=None):
        """
        Run simulation for a number of steps.
        
        Args:
            steps: Number of steps to run
            callback: Optional callback function(sim, step)
        """
        # Notify probes of simulation start
        self.probe_manager.notify_initial_time(self.current_step, self)
        
        for step in range(steps):
            self.step()
            if callback:
                callback(self, step)
        
        # Notify probes of simulation end
        self.probe_manager.notify_final_time(self.current_step, self)
    
    def _build_perception(self, turtle):
        """Build perception dictionary for a turtle."""
        # Get pheromone values at current location
        pheromones = {}
        x, y = int(turtle.position.x), int(turtle.position.y)
        for pheromone_id in self.environment.pheromones:
            pheromones[pheromone_id] = self.environment.get_pheromone_value(x, y, pheromone_id)
        
        # Get nearby marks
        marks = []
        if hasattr(self.environment, 'marks'):
            for mark_id, mark in self.environment.marks.items():
                distance = turtle.position.distance(mark['position'])
                if distance < 5.0:  # Within perception radius
                    marks.append(mark)
        
        # Get nearby turtles using spatial index (O(1) instead of O(N))
        nearby_turtles = []
        perception_radius = 10.0
        
        # Query spatial index for neighbors
        neighbors = self.spatial_index.query_radius(
            turtle.position.x,
            turtle.position.y,
            perception_radius,
            exclude=turtle
        )
        
        for other, distance in neighbors:
            nearby_turtles.append({
                'turtle': other,
                'distance': distance,
                'position': other.position,
                'heading': other.heading
            })
        
        return {
            'environment': self.environment,
            'position': turtle.position,
            'heading': turtle.heading,
            'speed': turtle.speed,
            'time': self.current_step,
            'pheromones': pheromones,
            'nearby_turtles': nearby_turtles,
            'marks': marks
        }

    def get_state(self):
        """Get current simulation state as a dictionary."""
        # Extract pheromone grids
        pheromones = {}
        if hasattr(self.environment, 'pheromone_grids'):
            for pid, grid in self.environment.pheromone_grids.items():
                # Convert grid to simple list of lists or similar for JSON
                # To save bandwidth, we could only send non-zero values, but for local dev full grid is fine
                pheromones[pid] = grid
        
        return {
            'step': self.current_step,
            'num_turtles': len(self.turtles),
            'turtles': [
                {
                    'position': [t.position.x, t.position.y],
                    'heading': t.heading,
                    'color': t.color,
                    'speed': t.speed
                }
                for t in self.turtles
            ],
            'environment': {
                'width': self.environment.width,
                'height': self.environment.height
            },
            'pheromones': pheromones
        }
    
    def reset(self):
        """Reset the simulation."""
        self.current_step = 0
        for turtle in self.turtles:
            turtle.position = self.environment.random_position()
            turtle.heading = self.environment.random_heading()
    
    def plot(self):
        """Plot the current state using matplotlib."""
        try:
            import matplotlib.pyplot as plt
            
            fig, ax = plt.subplots(figsize=(10, 10))
            
            # Plot turtles
            for turtle in self.turtles:
                ax.plot(turtle.position.x, turtle.position.y, 'o', 
                       color=turtle.color, markersize=5)
            
            # Plot marks if any
            if hasattr(self.environment, 'marks'):
                for mark in self.environment.marks.values():
                    ax.plot(mark['position'].x, mark['position'].y, 'x',
                           color='gray', markersize=3)
            
            ax.set_xlim(0, self.environment.width)
            ax.set_ylim(0, self.environment.height)
            ax.set_aspect('equal')
            ax.set_title(f'SIMILAR2Logo Simulation (Step {self.current_step})')
            ax.grid(True, alpha=0.3)
            
            plt.show()
        except ImportError:
            print("Matplotlib not available. Install with: pip install matplotlib")
