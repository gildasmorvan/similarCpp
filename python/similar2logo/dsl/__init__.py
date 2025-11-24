"""
Similar2Logo DSL - Core Module

A simplified, Pythonic API for creating agent-based simulations.

This DSL provides:
- Cleaner syntax than Jython examples
- Automatic C++ optimization
- Built-in web visualization
- Minimal boilerplate

Example:
    ```python
    from similar2logo.dsl import Simulation, SimpleTurtle
    
    class Boid(SimpleTurtle):
        def decide(self, perception):
            neighbors = self.nearby_turtles(radius=5)
            self.align_with(neighbors)
            return []
    
    sim = Simulation().grid(100, 100).agents(Boid, 50).run_web()
    ```
"""

from dataclasses import dataclass, field
from typing import List, Optional, Callable, Any, Type
import math
from ..model import LogoSimulation, Turtle
from ..tools import Point2D
from ..tools import Point2D
try:
    from ..web import WebSimulation
except ImportError:
    WebSimulation = None


@dataclass
class SimulationConfig:
    """Configuration for a simulation"""
    name: str = "SIMILAR Simulation"
    width: int = 100
    height: int = 100
    toroidal: bool = True
    max_steps: Optional[int] = None
    max_time: Optional[float] = None
    update_rate: int = 30
    port: int = 8080


class Simulation:
    """
    Main simulation class with fluent API.
    
    Example:
        ```python
        sim = (Simulation("Boids")
               .grid(100, 100)
               .agents(Boid, count=50)
               .run_web())
        ```
    """
    
    def __init__(self, name: str = "SIMILAR Simulation"):
        self.config = SimulationConfig(name=name)
        self._sim: Optional[LogoSimulation] = None
        self._agent_specs: List[tuple] = []
        self._probes: List[Any] = []
        self._custom_env: Optional[Any] = None
        self._setup_callback: Optional[Callable] = None
        self._parallel_backend: str = 'thread'  # 'thread', 'process', or None
        self._num_workers: Optional[int] = None
    
    def grid(self, width: int, height: int, toroidal: bool = True):
        """
        Set grid dimensions.
        
        Args:
            width: Grid width
            height: Grid height
            toroidal: Whether grid wraps around edges
        
        Returns:
            self for method chaining
        """
        self.config.width = width
        self.config.height = height
        self.config.toroidal = toroidal
        return self
    
    def parallel(self, backend: str = 'thread', num_workers: Optional[int] = None):
        """
        Configure parallel execution for agent decisions.
        
        Args:
            backend: 'thread', 'process', or None
                    - 'thread': Uses threading (limited by GIL for CPU-bound tasks)
                    - 'process': Uses multiprocessing (TRUE parallelism, higher overhead)
                    - None: Sequential execution
            num_workers: Number of workers (None = auto-detect from CPU count)
        
        Returns:
            self for method chaining
            
        Note:
            - Threading: Low overhead but limited by Python's GIL for CPU-bound operations
            - Process: True parallelism (no GIL) but higher memory/startup overhead
            - For maximum performance, use process backend or implement agents in C++
            
        Performance Guide:
            - Small simulations (<100 agents): Use None (sequential)
            - Medium simulations (100-1000 agents): Use 'thread' (low overhead)
            - Large simulations (>1000 agents, CPU-intensive): Use 'process' (true parallelism)
            
        Example:
            >>> # True parallelism for CPU-intensive boids
            >>> sim = (Simulation()
            ...        .parallel(backend='process', num_workers=4)
            ...        .agents(Boid, 1000)
            ...        .run_web())
        """
        self._parallel_backend = backend
        self._num_workers = num_workers
        return self
    
    def agents(self, agent_class: Type, count: int, **kwargs):
        """
        Add agents to the simulation.
        
        Args:
            agent_class: Agent class (must inherit from Turtle)
            count: Number of agents to create
            **kwargs: Additional arguments passed to agent constructor
        
        Returns:
            self for method chaining
        """
        self._agent_specs.append((agent_class, count, kwargs))
        return self
    
    def environment(self, env_class: Type):
        """
        Set custom environment class.
        
        Args:
            env_class: Environment class
        
        Returns:
            self for method chaining
        """
        self._custom_env = env_class
        return self
    
    def probe(self, probe):
        """
        Add a probe for monitoring.
        
        Args:
            probe: Probe instance
        
        Returns:
            self for method chaining
        """
        self._probes.append(probe)
        return self
    
    def setup(self, callback: Callable):
        """
        Register a setup callback to run before simulation starts.
        
        The callback receives the LogoSimulation instance.
        Use this to configure environment, add pheromones, place obstacles, etc.
        
        Args:
            callback: Function taking (simulation) as argument
        
        Returns:
            self for method chaining
        """
        self._setup_callback = callback
        return self
    
    def speed(self, multiplier: float):
        """
        Set simulation speed multiplier.
        
        Args:
            multiplier: Speed multiplier
        
        Returns:
            self for method chaining
        """
        # TODO: Implement speed control
        return self
    
    def _build_simulation(self) -> LogoSimulation:
        """Build the LogoSimulation instance"""
        if self._sim is None:
            # Create environment first
            from ..environment import Environment
            
            if self._custom_env:
                env = self._custom_env(
                    width=self.config.width,
                    height=self.config.height
                )
            else:
                env = Environment(
                    width=self.config.width,
                    height=self.config.height,
                    toroidal=self.config.toroidal
                )
            
            # Determine default turtle class and kwargs from first agent spec if available
            default_class = None
            default_kwargs = {}
            if self._agent_specs:
                default_class, _, default_kwargs = self._agent_specs[0]

            # Create simulation with environment and parallel configuration
            self._sim = LogoSimulation(
                environment=env,
                turtle_class=default_class,
                parallel_backend=self._parallel_backend,
                num_workers=self._num_workers,
                **default_kwargs
            )
            
            # Create agents
            for agent_class, count, kwargs in self._agent_specs:
                for _ in range(count):
                    agent = agent_class(**kwargs)
                    agent._environment = self._sim.environment
                    self._sim.turtles.append(agent)
            
            # Add probes
            for probe in self._probes:
                self._sim.probe_manager.add_probe(probe)
            
            # Run setup callback
            if self._setup_callback:
                self._setup_callback(self._sim)
        
        return self._sim
    
    def run(self, max_steps: Optional[int] = None, max_time: Optional[float] = None):
        """
        Run simulation in console mode.
        
        Args:
            max_steps: Maximum number of steps
            max_time: Maximum simulation time
        
        Returns:
            The LogoSimulation instance
        """
        sim = self._build_simulation()
        
        if max_steps:
            print(f"Running {self.config.name} for {max_steps} steps...")
            for step in range(max_steps):
                sim.step()
                if step % 100 == 0:
                    print(f"  Step {step}/{max_steps}")
        elif max_time:
            print(f"Running {self.config.name} until time {max_time}...")
            while sim.time < max_time:
                sim.step()
                if int(sim.time) % 100 == 0:
                    print(f"  Time: {sim.time:.2f}/{max_time}")
        else:
            print(f"Running {self.config.name}...")
            sim.run()
        
        print("Simulation complete!")
        return sim
    
    def run_web(self, port: int = 8080, update_rate: int = 30):
        """
        Run simulation with web interface.
        
        Args:
            port: Port for web server
            update_rate: Updates per second
        
        Returns:
            The WebSimulation instance
        """
        if WebSimulation is None:
            raise ImportError("Web simulation requires 'fastapi' and 'uvicorn'. Install them with: pip install fastapi uvicorn")

        sim = self._build_simulation()
        
        print("=" * 60)
        print(f"{self.config.name} - Web Interface")
        print("=" * 60)
        print(f"\\nOpen your browser to: http://localhost:{port}")
        print("=" * 60 + "\\n")
        
        web_sim = WebSimulation(sim, update_rate=update_rate)
        web_sim.start_server(port=port)
        
        return web_sim
    
    def __enter__(self):
        """Context manager entry"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        return False


class SimpleTurtle(Turtle):
    """
    Simplified Turtle class with helper methods.
    
    This extends the base Turtle with convenience methods
    for common operations.
    """
    
    def nearby_turtles(self, perception=None, radius: float = 5.0):
        """
        Get all turtles within radius.
        
        Note: This should be called from within decide() and passed the perception.
        """
        if perception is None:
            # Fallback: try to get from nearby_turtles in perception
            return []
        
        # Use the nearby_turtles from perception
        if 'nearby_turtles' in perception:
            return [n['turtle'] for n in perception['nearby_turtles'] 
                   if n['distance'] <= radius]
        return []
    
    def turtles_in_cone(self, perception=None, radius: float = 5.0, angle: float = math.pi):
        """Get turtles within a cone of vision"""
        if perception is None or 'nearby_turtles' not in perception:
            return []
        
        nearby = []
        for neighbor_data in perception['nearby_turtles']:
            if neighbor_data['distance'] <= radius:
                other = neighbor_data['turtle']
                # Calculate angle to other turtle
                dx = other.position.x - self.position.x
                dy = other.position.y - self.position.y
                # Convert atan2 (0=East) to Logo heading (0=North)
                angle_to = math.atan2(dy, dx) + math.pi / 2
                
                angle_diff = abs(angle_to - self.heading)
                
                # Normalize angle difference
                while angle_diff > math.pi:
                    angle_diff -= 2 * math.pi
                angle_diff = abs(angle_diff)
                
                if angle_diff <= angle / 2:
                    nearby.append(other)
        return nearby
    
    def align_with(self, neighbors, weight: float = 1.0):
        """Align heading with average of neighbors"""
        if not neighbors:
            return []
        
        avg_heading = sum(n.heading for n in neighbors) / len(neighbors)
        angle_diff = avg_heading - self.heading
        
        # Normalize to [-pi, pi]
        while angle_diff > math.pi:
            angle_diff -= 2 * math.pi
        while angle_diff < -math.pi:
            angle_diff += 2 * math.pi
        
        return [self.influence_turn(angle_diff * weight)]
    
    def separate_from(self, neighbors, min_distance: float = 1.0, weight: float = 1.0):
        """Move away from neighbors that are too close"""
        if not neighbors:
            return []
        
        repulsion_x = 0.0
        repulsion_y = 0.0
        
        for neighbor in neighbors:
            dist = self.position.distance(neighbor.position)
            if dist < min_distance and dist > 0:
                # Calculate repulsion vector
                dx = self.position.x - neighbor.position.x
                dy = self.position.y - neighbor.position.y
                # Normalize and weight by inverse distance
                factor = (min_distance - dist) / dist
                repulsion_x += dx * factor
                repulsion_y += dy * factor
        
        if abs(repulsion_x) > 0.01 or abs(repulsion_y) > 0.01:
            # Convert atan2 (0=East) to Logo heading (0=North)
            desired_heading = math.atan2(repulsion_y, repulsion_x) + math.pi / 2
            angle_diff = desired_heading - self.heading
            
            # Normalize
            while angle_diff > math.pi:
                angle_diff -= 2 * math.pi
            while angle_diff < -math.pi:
                angle_diff += 2 * math.pi
            
            return [self.influence_turn(angle_diff * weight)]
        
        return []
    
    def cohere_with(self, neighbors, weight: float = 1.0):
        """Move towards average position of neighbors"""
        if not neighbors:
            return []
        
        avg_x = sum(n.position.x for n in neighbors) / len(neighbors)
        avg_y = sum(n.position.y for n in neighbors) / len(neighbors)
        
        dx = avg_x - self.position.x
        dy = avg_y - self.position.y
        
        if abs(dx) > 0.01 or abs(dy) > 0.01:
            # Convert atan2 (0=East) to Logo heading (0=North)
            desired_heading = math.atan2(dy, dx) + math.pi / 2
            angle_diff = desired_heading - self.heading
            
            # Normalize
            while angle_diff > math.pi:
                angle_diff -= 2 * math.pi
            while angle_diff < -math.pi:
                angle_diff += 2 * math.pi
            
            return [self.influence_turn(angle_diff * weight)]
        
        return []


__all__ = [
    'Simulation',
    'SimpleTurtle',
    'SimulationConfig',
]
