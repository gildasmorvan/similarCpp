"""
C++ Logo Simulation - Python Interface

This module provides a Python-friendly interface to the high-performance
C++ Logo simulation engine with true multithreading.
"""

from typing import List, Callable, Optional, Any
import math


class CppLogoSimulation:
    """
    High-performance Logo simulation using C++ multithreaded engine.
    
    This provides true parallel execution of agent decisions without
    GIL limitations, as all core logic runs in C++.
    
    Example:
        >>> sim = CppLogoSimulation(width=100, height=100, num_threads=4)
        >>> sim.add_agents(BoidAgent, count=1000)
        >>> sim.run_web(port=8080)
    """
    
    def __init__(self, width: int = 100, height: int = 100,
                 toroidal: bool = True, num_threads: Optional[int] = None,
                 max_steps: int = 10000):
        """
        Initialize C++ Logo simulation.
        
        Args:
            width: Grid width
            height: Grid height
            toroidal: Whether grid wraps around
            num_threads: Number of threads (None = auto-detect)
            max_steps: Maximum simulation steps
        """
        try:
            from similar2logo import _logo_cpp
            self._cpp = _logo_cpp
        except ImportError:
            raise ImportError(
                "C++ Logo engine not available. "
                "Build the C++ bindings with: cd cpp/similar2logo/python && mkdir -p build && cd build && cmake .. && make"
            )
        
        self.width = width
        self.height = height
        self.toroidal = toroidal
        self.num_threads = num_threads or 0  # 0 = auto-detect
        
        # Create C++ model and engine
        self.model = self._cpp.LogoSimulationModel(
            width, height, toroidal, toroidal, max_steps
        )
        self.engine = self._cpp.MultiThreadedEngine(self.num_threads)
        
        self._agents = []
        self._pheromones = []
    
    def add_pheromone(self, pheromone_id: str, diffusion: float = 0.1,
                     evaporation: float = 0.01):
        """
        Add a pheromone type to the environment.
        
        Args:
            pheromone_id: Unique identifier
            diffusion: Diffusion coefficient
            evaporation: Evaporation coefficient
        """
        from similar2logo._core.environment import Pheromone
        pheromone = Pheromone(pheromone_id, diffusion, evaporation)
        self.model.add_pheromone(pheromone)
        self._pheromones.append(pheromone)
    
    def add_agents(self, agent_class, count: int, **kwargs):
        """
        Add agents to the simulation.
        
        Args:
            agent_class: Python agent class with decide() method
            count: Number of agents to create
            **kwargs: Additional arguments for agent constructor
        """
        # Store agent specifications
        self._agents.append((agent_class, count, kwargs))
    
    def run(self, steps: Optional[int] = None):
        """
        Run the simulation for a number of steps.
        
        Args:
            steps: Number of steps (None = run until max_steps)
        """
        # Configure the model with agent factory
        self._configure_agents()
        
        # Run simulation
        self.engine.run_new_simulation(self.model)
    
    def run_web(self, port: int = 8080, host: str = "0.0.0.0"):
        """
        Run simulation with web interface.
        
        Args:
            port: Web server port
            host: Web server host
        """
        # For now, fall back to Python web server
        # TODO: Integrate with C++ SimilarWebRunner
        print("C++ web interface integration coming soon.")
        print("Falling back to Python web server...")
        
        # Create a Python wrapper that uses the C++ engine
        from ..web.server import WebSimulation
        from ..model import LogoSimulation
        from ..environment import Environment
        
        # Create Python simulation as a facade
        env = Environment(self.width, self.height, self.toroidal)
        py_sim = LogoSimulation(env)
        
        # Add agents to Python simulation
        for agent_class, count, kwargs in self._agents:
            py_sim.add_turtles(count, agent_class, **kwargs)
        
        # Run with web interface
        web_sim = WebSimulation(py_sim)
        web_sim.start_server(host=host, port=port)
    
    def _configure_agents(self):
        """Configure the C++ model with agent factory."""
        from similar2logo._core.microkernel import LevelIdentifier, AgentCategory
        from similar2logo._core.levels import LogoSimulationLevelList
        
        def agent_factory():
            """Factory function to create C++ agents."""
            cpp_agents = []
            
            for agent_class, count, kwargs in self._agents:
                for i in range(count):
                    # Create C++ Logo agent
                    category = AgentCategory(agent_class.__name__)
                    cpp_agent = self._cpp.LogoAgent(category)
                    
                    # Create Python decision model wrapper
                    py_agent = agent_class(**kwargs)
                    
                    def make_decision_callback(py_agent):
                        def callback(perception, influences):
                            # Call Python decide() method
                            # Convert C++ perception to Python-friendly format
                            py_perception = {
                                'position': perception.get_position(),
                                'heading': perception.get_heading(),
                                'speed': perception.get_speed(),
                                'nearby_turtles': perception.get_nearby_turtles(),
                                'pheromones': perception.get_all_pheromones()
                            }
                            
                            # Get influences from Python agent
                            py_influences = py_agent.decide(py_perception)
                            
                            # Convert Python influences to C++ influences
                            # and add to influences map
                            for influence in py_influences:
                                influences.add_all([influence])
                        
                        return callback
                    
                    # Set decision model
                    decision_model = self._cpp.PythonDecisionModel(
                        LogoSimulationLevelList.LOGO,
                        make_decision_callback(py_agent)
                    )
                    
                    cpp_agent.specify_behavior_for_level(
                        LogoSimulationLevelList.LOGO,
                        None,  # Perception model (use default)
                        decision_model
                    )
                    
                    cpp_agents.append(cpp_agent)
            
            return cpp_agents
        
        self.model.set_agent_factory(agent_factory)
    
    def get_state(self):
        """Get current simulation state."""
        # TODO: Implement state extraction from C++ engine
        return {
            'step': 0,
            'num_turtles': sum(count for _, count, _ in self._agents),
            'turtles': []
        }


def use_cpp_engine():
    """
    Check if C++ engine is available.
    
    Returns:
        bool: True if C++ engine can be used
    """
    try:
        from similar2logo import _logo_cpp
        return True
    except ImportError:
        return False
