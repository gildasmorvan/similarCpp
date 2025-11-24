"""
Probe system for SIMILAR simulations.

Probes observe the simulation at various points and can perform actions
like logging, visualization, or timing control.
"""

import time
from abc import ABC, abstractmethod
from typing import Optional


class IProbe(ABC):
    """
    Base interface for simulation probes.
    
    Probes can observe the simulation at different points:
    - At initialization
    - At each simulation step (partial consistent time)
    - At final time
    """
    
    @abstractmethod
    def observe_at_initial_time(self, initial_time: int, simulation):
        """
        Called when the simulation starts.
        
        Args:
            initial_time: Initial simulation time
            simulation: The LogoSimulation instance
        """
        pass
    
    @abstractmethod
    def observe_at_partial_consistent_time(self, current_time: int, simulation):
        """
        Called at each simulation step.
        
        Args:
            current_time: Current simulation time
            simulation: The LogoSimulation instance
        """
        pass
    
    def observe_at_final_time(self, final_time: int, simulation):
        """
        Called when the simulation ends.
        
        Args:
            final_time: Final simulation time
            simulation: The LogoSimulation instance
        """
        pass


class RealTimeMatcherProbe(IProbe):
    """
    Probe that slows down the simulation to match real time.
    
    This ensures the simulation runs at a specific speed relative to real time.
    Useful for visualization and interactive simulations.
    
    Args:
        acceleration_factor: Speed multiplier relative to real time.
            - 1.0 = real time (1 sim second = 1 real second)
            - 2.0 = 2x faster than real time
            - 0.5 = half speed (slow motion)
    
    Examples:
        >>> # Run at real time
        >>> probe = RealTimeMatcherProbe(1.0)
        >>> 
        >>> # Run at 10x speed
        >>> probe = RealTimeMatcherProbe(10.0)
        >>> 
        >>> # Run at half speed
        >>> probe = RealTimeMatcherProbe(0.5)
    """
    
    NANOSECONDS_IN_SECOND = 1_000_000_000
    MILLISECONDS_IN_SECOND = 1_000
    
    def __init__(self, acceleration_factor: float = 1.0):
        if acceleration_factor <= 0:
            raise ValueError(f"Acceleration factor must be positive, got {acceleration_factor}")
        
        self.acceleration_factor = acceleration_factor
        self.previous_measure_sim_time: Optional[int] = None
        self.previous_measure_real_time: Optional[float] = None
    
    def observe_at_initial_time(self, initial_time: int, simulation):
        """Initialize timing measurements."""
        self.previous_measure_sim_time = initial_time
        self.previous_measure_real_time = time.perf_counter()
    
    def observe_at_partial_consistent_time(self, current_time: int, simulation):
        """
        Slow down the simulation if it's running too fast.
        
        Compares simulation time elapsed vs real time elapsed and sleeps
        if the simulation is ahead of schedule.
        """
        if self.previous_measure_sim_time is None:
            self.observe_at_initial_time(current_time, simulation)
            return
        
        # Calculate simulation time elapsed (in seconds)
        sim_time_elapsed = current_time - self.previous_measure_sim_time
        
        # Calculate real time elapsed (in seconds)
        current_real_time = time.perf_counter()
        real_time_elapsed = current_real_time - self.previous_measure_real_time
        
        # Calculate how much time should have elapsed in real time
        # for the simulation time that passed
        target_real_time = sim_time_elapsed / self.acceleration_factor
        
        # If simulation is ahead of real time, sleep
        difference = target_real_time - real_time_elapsed
        
        if difference > 0:
            time.sleep(difference)
        
        # Update measurements every second of simulation time
        # This prevents accumulation of small timing errors
        if sim_time_elapsed >= 1.0:
            self.previous_measure_sim_time = current_time
            self.previous_measure_real_time = time.perf_counter()


class ProbeManager:
    """
    Manages a collection of probes for a simulation.
    
    Examples:
        >>> manager = ProbeManager()
        >>> manager.add_probe("timing", RealTimeMatcherProbe(10.0))
        >>> manager.notify_initial_time(0, simulation)
        >>> manager.notify_step(1, simulation)
    """
    
    def __init__(self):
        self.probes = {}
    
    def add_probe(self, name: str, probe: IProbe):
        """Add a probe with a given name."""
        self.probes[name] = probe
    
    def remove_probe(self, name: str):
        """Remove a probe by name."""
        if name in self.probes:
            del self.probes[name]
    
    def notify_initial_time(self, initial_time: int, simulation):
        """Notify all probes of simulation start."""
        for probe in self.probes.values():
            probe.observe_at_initial_time(initial_time, simulation)
    
    def notify_step(self, current_time: int, simulation):
        """Notify all probes of a simulation step."""
        for probe in self.probes.values():
            probe.observe_at_partial_consistent_time(current_time, simulation)
    
    def notify_final_time(self, final_time: int, simulation):
        """Notify all probes of simulation end."""
        for probe in self.probes.values():
            probe.observe_at_final_time(final_time, simulation)
