"""
Bridge module to use multithreading/multiprocessing for parallel agent decisions.

This provides both threading and process-based parallel executors:
- Threading: Low overhead but limited by Python's GIL for CPU-bound tasks
- Process: True parallelism but higher overhead due to process creation

For maximum performance with no GIL limitations, use the C++ MultiThreadedSimulationEngine.
"""

from typing import List, Callable, Any
import multiprocessing
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor


class ThreadedDecisionExecutor:
    """
    Threaded executor for agent decision-making.
    
    Note: Due to Python's GIL, this won't provide true parallelism for
    CPU-bound tasks. However, it provides the correct architecture and
    can benefit I/O-bound operations.
    
    For true parallelism, use ProcessDecisionExecutor or the C++ engine.
    """
    
    def __init__(self, num_workers=None):
        """
        Initialize the threaded executor.
        
        Args:
            num_workers: Number of worker threads (None = CPU count * 2)
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count() * 2
        self.executor = ThreadPoolExecutor(max_workers=num_workers)
        self.num_workers = num_workers
    
    def map_decisions(self, turtles, perceptions):
        """
        Execute decisions for all turtles using threads.
        
        Args:
            turtles: List of turtle agents
            perceptions: Dictionary mapping turtles to their perceptions
            
        Returns:
            List of influences from all turtles
        """
        def get_decision(turtle):
            return turtle.decide(perceptions[turtle])
        
        results = []
        for turtle_influences in self.executor.map(get_decision, turtles):
            if turtle_influences:
                results.append(turtle_influences)
        
        return results
    
    def shutdown(self):
        """Shutdown the executor."""
        self.executor.shutdown(wait=True)


class ProcessDecisionExecutor:
    """
    Process-based executor for agent decision-making.
    
    Provides true parallelism by using separate Python processes,
    bypassing the GIL. Higher overhead than threading but much
    faster for CPU-bound agent logic.
    
    Note: Agents and perceptions must be picklable.
    """
    
    def __init__(self, num_workers=None):
        """
        Initialize the process executor.
        
        Args:
            num_workers: Number of worker processes (None = CPU count)
        """
        if num_workers is None:
            num_workers = multiprocessing.cpu_count()
        self.executor = ProcessPoolExecutor(max_workers=num_workers)
        self.num_workers = num_workers
    
    def map_decisions(self, turtles, perceptions):
        """
        Execute decisions for all turtles using processes.
        
        Args:
            turtles: List of turtle agents (must be picklable)
            perceptions: Dictionary mapping turtles to their perceptions
            
        Returns:
            List of influences from all turtles
        """
        # Create list of (turtle, perception) tuples for mapping
        tasks = [(turtle, perceptions[turtle]) for turtle in turtles]
        
        results = []
        for turtle_influences in self.executor.map(_process_decision_worker, tasks):
            if turtle_influences:
                results.append(turtle_influences)
        
        return results
    
    def shutdown(self):
        """Shutdown the executor."""
        self.executor.shutdown(wait=True)


# Module-level function for pickling compatibility
def _process_decision_worker(task):
    """Worker function for process-based decision execution."""
    turtle, perception = task
    return turtle.decide(perception)



def create_executor(backend='thread', num_workers=None):
    """
    Factory function to create an appropriate executor.
    
    Args:
        backend: 'thread', 'process', or None
                - 'thread': Uses threading (limited by GIL for CPU-bound tasks)
                - 'process': Uses multiprocessing (true parallelism, higher overhead)
                - None: Sequential execution (no parallelism)
        num_workers: Number of workers (None = auto-detect)
        
    Returns:
        Executor instance or None
        
    Note:
        For maximum performance without GIL limitations, use the C++
        MultiThreadedSimulationEngine directly via the _core module bindings.
        
    Examples:
        # Threading (good for I/O-bound or when overhead matters)
        executor = create_executor('thread', num_workers=8)
        
        # Multiprocessing (true parallelism for CPU-bound tasks)
        executor = create_executor('process', num_workers=4)
        
        # Sequential (no parallelism)
        executor = create_executor(None)
    """
    if backend == 'thread':
        return ThreadedDecisionExecutor(num_workers)
    elif backend == 'process':
        return ProcessDecisionExecutor(num_workers)
    elif backend is None:
        return None
    else:
        raise ValueError(
            f"Unknown backend: {backend}. "
            f"Supported: 'thread', 'process', or None"
        )
