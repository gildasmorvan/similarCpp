"""
Reaction model for processing influences in SIMILAR2Logo.

The reaction model is responsible for:
1. Processing agent influences (ChangePosition, EmitPheromone, etc.)
2. Updating pheromone dynamics (diffusion, evaporation)
3. Handling system influences (add/remove agents)
4. Resolving conflicts between influences
"""

import math
from typing import List
from .influences import *
from .environment import Environment
from .tools import MathUtil, Point2D

try:
    from ._core.reaction import Reaction as CppReaction
    HAS_CPP_REACTION = True
except ImportError:
    HAS_CPP_REACTION = False


class LogoReactionModel:
    """
    Default reaction model for Logo simulations.
    
    Processes influences and updates the environment state according
    to the SIMILAR influence/reaction architecture.
    """
    
    def __init__(self):
        self.marks_counter = 0  # For generating unique mark IDs
        self._cpp_fallback_warned = False  # Track if we've warned about Python fallback
        self._cpp_success_shown = False  # Track if we've shown C++ success message
        if HAS_CPP_REACTION:
            self._cpp_reaction = CppReaction()
    
    def make_regular_reaction(self, time_min: int, time_max: int,
                              environment: Environment,
                              influences: InfluencesMap):
        """
        Process regular influences and update environment.
        
        Args:
            time_min: Start of time step
            time_max: End of time step
            environment: The simulation environment
            influences: Map of influences to process
        """
        # Try C++ reaction first when appropriate.
        # Check if we have C++ reaction and if environment is C++ compatible
        # (Environment inherits from CppEnvironment when available)
        cpp_used = False
        has_cpp_candidates = False
        if HAS_CPP_REACTION:
            try:
                from ._core.environment import Environment as CppEnvironment
                is_cpp_env = isinstance(environment, CppEnvironment)
            except ImportError:
                is_cpp_env = False
            
            if is_cpp_env:
                cpp_influences = []
                for influence in influences.get_all_regular():
                    # Low-level C++ influences (e.g., ChangePosition from _core)
                    # expose a getTarget method; high-level DSL influences do not.
                    if hasattr(influence, 'getTarget'):
                        cpp_influences.append(influence)
                has_cpp_candidates = bool(cpp_influences)
                
                if cpp_influences:
                    dt = float(time_max - time_min)
                    # Pass environment directly - it already IS a C++ environment
                    self._cpp_reaction.apply(cpp_influences, environment, dt)
                    cpp_used = True
                    
                    # Show success message once (if verbose mode enabled)
                    if not self._cpp_success_shown:
                        import os
                        if os.environ.get('SIMILAR_VERBOSE', '').lower() in ('1', 'true', 'yes'):
                            print("✓ Using C++ reaction engine for optimal performance")
                        self._cpp_success_shown = True

        # Warn if falling back to Python (only once), but only when we actually
        # had C++-compatible influences that could not be processed.
        # DSL-only simulations that never emit low-level C++ influences should
        # quietly use the Python reaction model.
        if has_cpp_candidates and not cpp_used and not self._cpp_fallback_warned:
            import warnings
            if not HAS_CPP_REACTION:
                warnings.warn(
                    "⚠️  Using Python reaction model (C++ not available). "
                    "Performance will be significantly slower. "
                    "Build C++ engine with: ./build_cpp_engine.sh",
                    RuntimeWarning,
                    stacklevel=2
                )
            else:
                warnings.warn(
                    "⚠️  Using Python reaction fallback. "
                    "C++ reaction is available but could not be used for some "
                    "low-level influences.",
                    RuntimeWarning,
                    stacklevel=2
                )
            self._cpp_fallback_warned = True

        # Process each type of influence (Python legacy or non-C++ types)
        self._process_change_position_influences(influences, environment)
        self._process_change_direction_influences(influences, environment)
        self._process_change_speed_influences(influences, environment)
        self._process_stop_influences(influences, environment)
        self._process_drop_mark_influences(influences, environment)
        self._process_remove_mark_influences(influences, environment)
        self._process_emit_pheromone_influences(influences, environment)
        
        # Update pheromone dynamics (only for Python environment)
        # C++ environment handles this internally or doesn't support it yet
        dt = time_max - time_min
        if dt > 0 and hasattr(environment, 'pheromone_grids') and environment.pheromone_grids:
            self._pheromone_diffusion(environment, dt)
            self._pheromone_evaporation(environment, dt)

    def make_natural_reaction(self, time_min: int, time_max: int,
                             environment: Environment,
                             influences: List):
        """
        Process natural influences (physics, environment dynamics).

        Args:
            time_min: Start of time step
            time_max: End of time step
            environment: The simulation environment
            influences: List of natural influences
        """
        for influence in influences:
            if isinstance(influence, AgentPositionUpdate):
                # Update agent position based on dynamics
                influence.agent.position = influence.new_position

    def make_system_reaction(self, time_min: int, time_max: int,
                            environment: Environment,
                            influences: List,
                            turtles: List):
        """
        Process system influences.
        
        Args:
            time_min: Start of time step
            time_max: End of time step
            environment: The simulation environment
            influences: List of system influences
            turtles: List of turtles in simulation
        """
        for influence in influences:
            if isinstance(influence, SystemInfluenceAddAgent):
                if influence.agent not in turtles:
                    turtles.append(influence.agent)

            elif isinstance(influence, SystemInfluenceRemoveAgent):
                if influence.agent in turtles:
                    turtles.remove(influence.agent)
    
    # ========================================================================
    # Regular Influence Processing
    # ========================================================================
    
    def _process_change_position_influences(self, influences: InfluencesMap,
                                           environment: Environment):
        """Process ChangePosition influences."""
        for influence in influences.get_by_type(ChangePosition):
            agent = influence.agent
            
            if influence.target_position:
                # Absolute position
                new_x = influence.target_position.x
                new_y = influence.target_position.y
            else:
                # Relative position
                new_x = agent.position.x + influence.dx
                new_y = agent.position.y + influence.dy
            
            # Apply toroidal topology if enabled
            if environment.toroidal:
                new_x = new_x % environment.width
                new_y = new_y % environment.height
            else:
                # Clamp to boundaries
                new_x = max(0, min(new_x, environment.width - 1))
                new_y = max(0, min(new_y, environment.height - 1))
            
            # Check for obstacles/conflicts here if needed
            # For now, accept all position changes
            agent.position.x = new_x
            agent.position.y = new_y
    
    def _process_change_direction_influences(self, influences: InfluencesMap,
                                            environment: Environment):
        """Process ChangeDirection influences."""
        for influence in influences.get_by_type(ChangeDirection):
            agent = influence.agent
            
            if influence.target_heading is not None:
                # Absolute heading
                agent.heading = MathUtil.normalize_angle(influence.target_heading)
            else:
                # Relative heading
                agent.heading = MathUtil.normalize_angle(
                    agent.heading + influence.delta_heading
                )
    
    def _process_change_speed_influences(self, influences: InfluencesMap,
                                        environment: Environment):
        """Process ChangeSpeed influences."""
        for influence in influences.get_by_type(ChangeSpeed):
            agent = influence.agent
            
            if influence.target_speed is not None:
                agent.speed = max(0, influence.target_speed)
            else:
                agent.speed = max(0, agent.speed + influence.delta_speed)
    
    def _process_stop_influences(self, influences: InfluencesMap,
                                environment: Environment):
        """Process Stop influences."""
        for influence in influences.get_by_type(Stop):
            agent = influence.agent
            agent.speed = 0.0
            if hasattr(agent, 'acceleration'):
                agent.acceleration = 0.0
    
    def _process_drop_mark_influences(self, influences: InfluencesMap,
                                     environment: Environment):
        """Process DropMark influences."""
        if not hasattr(environment, 'marks'):
            environment.marks = {}

        for influence in influences.get_by_type(DropMark):
            # Special case: if content is "remove", remove all marks at current position
            if influence.content == "remove":
                marks_to_remove = []
                for mark_id, mark in environment.marks.items():
                    if (abs(mark['position'].x - influence.position.x) < 1.0 and
                        abs(mark['position'].y - influence.position.y) < 1.0):
                        marks_to_remove.append(mark_id)
                for mark_id in marks_to_remove:
                    del environment.marks[mark_id]
            else:
                # Normal mark dropping
                mark_id = f"mark_{self.marks_counter}"
                self.marks_counter += 1

                mark = {
                    'id': mark_id,
                    'position': Point2D(influence.position.x, influence.position.y),
                    'content': influence.content,
                    'category': influence.category,
                    'agent': influence.agent
                }

                environment.marks[mark_id] = mark
    
    def _process_remove_mark_influences(self, influences: InfluencesMap,
                                       environment: Environment):
        """Process RemoveMark and RemoveMarks influences."""
        if not hasattr(environment, 'marks'):
            return
        
        # Process single mark removals
        for influence in influences.get_by_type(RemoveMark):
            if influence.mark_id in environment.marks:
                del environment.marks[influence.mark_id]
        
        # Process bulk mark removals
        for influence in influences.get_by_type(RemoveMarks):
            marks_to_remove = []
            
            for mark_id, mark in environment.marks.items():
                # Check category
                if mark['category'] != influence.category:
                    continue
                
                # Check position/radius if specified
                if influence.position and influence.radius:
                    distance = mark['position'].distance(influence.position)
                    if distance > influence.radius:
                        continue
                
                marks_to_remove.append(mark_id)
            
            for mark_id in marks_to_remove:
                del environment.marks[mark_id]
    
    def _process_emit_pheromone_influences(self, influences: InfluencesMap,
                                          environment: Environment):
        """Process EmitPheromone influences."""
        for influence in influences.get_by_type(EmitPheromone):
            x = int(influence.position.x)
            y = int(influence.position.y)
            
            # Clamp to grid
            x = max(0, min(x, environment.width - 1))
            y = max(0, min(y, environment.height - 1))
            
            # Add pheromone to grid
            if influence.pheromone_id in environment.pheromone_grids:
                current = environment.pheromone_grids[influence.pheromone_id][y][x]
                environment.pheromone_grids[influence.pheromone_id][y][x] = current + influence.amount
    
    # ========================================================================
    # Pheromone Dynamics
    # ========================================================================
    
    def _pheromone_diffusion(self, environment: Environment, dt: int):
        """
        Apply diffusion to pheromone fields.
        
        Args:
            environment: The environment containing pheromone grids
            dt: Time step size
        """
        for pheromone_id, pheromone in environment.pheromones.items():
            if pheromone.diffusion_coef <= 0:
                continue
            
            grid = environment.pheromone_grids[pheromone_id]
            new_grid = [row[:] for row in grid]  # Deep copy
            
            # 8-neighbor diffusion
            for y in range(environment.height):
                for x in range(environment.width):
                    current_value = grid[y][x]
                    
                    if current_value <= 0:
                        continue
                    
                    # Get 8 neighbors
                    neighbors = self._get_neighbors(x, y, environment)
                    
                    # Diffuse to each neighbor
                    diffusion_amount = pheromone.diffusion_coef * current_value * dt
                    amount_per_neighbor = diffusion_amount / len(neighbors)
                    
                    for nx, ny in neighbors:
                        new_grid[ny][nx] += amount_per_neighbor
                    
                    # Reduce current cell
                    new_grid[y][x] -= diffusion_amount
            
            environment.pheromone_grids[pheromone_id] = new_grid
    
    def _pheromone_evaporation(self, environment: Environment, dt: int):
        """
        Apply evaporation to pheromone fields.
        
        Args:
            environment: The environment containing pheromone grids
            dt: Time step size
        """
        for pheromone_id, pheromone in environment.pheromones.items():
            if pheromone.evaporation_coef <= 0:
                continue
            
            grid = environment.pheromone_grids[pheromone_id]
            
            for y in range(environment.height):
                for x in range(environment.width):
                    current_value = grid[y][x]
                    
                    # Evaporate
                    evaporation = pheromone.evaporation_coef * current_value * dt
                    new_value = current_value - evaporation
                    
                    # Apply minimum threshold
                    if new_value < pheromone.min_value:
                        new_value = 0.0
                    
                    grid[y][x] = new_value
    
    def _get_neighbors(self, x: int, y: int, environment: Environment):
        """Get 8-connected neighbors of a cell."""
        neighbors = []
        
        for dy in [-1, 0, 1]:
            for dx in [-1, 0, 1]:
                if dx == 0 and dy == 0:
                    continue
                
                nx = x + dx
                ny = y + dy
                
                # Handle toroidal topology
                if environment.toroidal:
                    nx = nx % environment.width
                    ny = ny % environment.height
                else:
                    # Skip out-of-bounds neighbors
                    if nx < 0 or nx >= environment.width:
                        continue
                    if ny < 0 or ny >= environment.height:
                        continue
                
                neighbors.append((nx, ny))
        
        return neighbors
