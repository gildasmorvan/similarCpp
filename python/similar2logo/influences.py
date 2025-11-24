"""
Influence classes for SIMILAR2Logo.

Influences represent intended actions by agents or the environment.
They are processed by the reaction model to update the simulation state.

This module uses C++ implementations when available for better performance.
"""

from abc import ABC, abstractmethod
from typing import Any, Optional
from .tools import Point2D

# Try to import C++ influence classes
try:
    from . import _core
    HAS_CPP_INFLUENCES = hasattr(_core, 'influences')
except (ImportError, AttributeError):
    HAS_CPP_INFLUENCES = False


# ============================================================================
# Base Classes
# ============================================================================

class IInfluence(ABC):
    """Base interface for all influences."""
    
    @abstractmethod
    def get_category(self) -> str:
        """Get the category of this influence."""
        pass


class RegularInfluence(IInfluence):
    """Base class for regular (agent-initiated) influences."""
    
    def __init__(self, source_agent=None):
        self.source_agent = source_agent
        self.time_stamp = None
    
    def get_category(self) -> str:
        return "REGULAR"


class SystemInfluence(IInfluence):
    """Base class for system influences."""
    
    def get_category(self) -> str:
        return "SYSTEM"


# ============================================================================
# Agent Movement Influences (Python Fallback)
# ============================================================================

class _PyChangePosition(RegularInfluence):
    """
    Python implementation of ChangePosition influence.
    
    Args:
        agent: The agent requesting the move
        dx: Change in x coordinate
        dy: Change in y coordinate
        target_position: Optional absolute target position
    """
    
    def __init__(self, agent, dx: float = 0.0, dy: float = 0.0, 
                 target_position: Optional[Point2D] = None):
        super().__init__(agent)
        self.agent = agent
        self.dx = dx
        self.dy = dy
        self.target_position = target_position
    
    def __repr__(self):
        if self.target_position:
            return f"ChangePosition(agent={self.agent}, target={self.target_position})"
        return f"ChangePosition(agent={self.agent}, dx={self.dx}, dy={self.dy})"


class _PyChangeDirection(RegularInfluence):
    """
    Python implementation of ChangeDirection influence.
    
    Args:
        agent: The agent requesting the turn
        delta_heading: Change in heading (radians)
        target_heading: Optional absolute target heading
    """
    
    def __init__(self, agent, delta_heading: float = 0.0,
                 target_heading: Optional[float] = None):
        super().__init__(agent)
        self.agent = agent
        self.delta_heading = delta_heading
        self.target_heading = target_heading
    
    def __repr__(self):
        if self.target_heading is not None:
            return f"ChangeDirection(agent={self.agent}, target={self.target_heading})"
        return f"ChangeDirection(agent={self.agent}, delta={self.delta_heading})"


class _PyChangeSpeed(RegularInfluence):
    """
    Python implementation of ChangeSpeed influence.
    
    Args:
        agent: The agent requesting the speed change
        delta_speed: Change in speed
        target_speed: Optional absolute target speed
    """
    
    def __init__(self, agent, delta_speed: float = 0.0,
                 target_speed: Optional[float] = None):
        super().__init__(agent)
        self.agent = agent
        self.delta_speed = delta_speed
        self.target_speed = target_speed


class _PyChangeAcceleration(RegularInfluence):
    """
    Python implementation of ChangeAcceleration influence.
    
    Args:
        agent: The agent requesting the acceleration change
        delta_acceleration: Change in acceleration
        target_acceleration: Optional absolute target acceleration
    """
    
    def __init__(self, agent, delta_acceleration: float = 0.0,
                 target_acceleration: Optional[float] = None):
        super().__init__(agent)
        self.agent = agent
        self.delta_acceleration = delta_acceleration
        self.target_acceleration = target_acceleration


class _PyStop(RegularInfluence):
    """
    Python implementation of Stop influence.
    
    Args:
        agent: The agent requesting to stop
    """
    
    def __init__(self, agent):
        super().__init__(agent)
        self.agent = agent


# ============================================================================
# Environmental Interaction Influences
# ============================================================================

class DropMark(RegularInfluence):
    """
    Influence to drop a mark in the environment.
    
    Args:
        agent: The agent dropping the mark
        position: Where to drop the mark
        content: The content/data of the mark
        category: Optional category for the mark
    """
    
    def __init__(self, agent, position: Point2D, content: Any,
                 category: str = "default"):
        super().__init__(agent)
        self.agent = agent
        self.position = position
        self.content = content
        self.category = category
    
    def __repr__(self):
        return f"DropMark(position={self.position}, content={self.content})"


class RemoveMark(RegularInfluence):
    """
    Influence to remove a specific mark from the environment.
    
    Args:
        agent: The agent removing the mark
        mark_id: Identifier of the mark to remove
    """
    
    def __init__(self, agent, mark_id):
        super().__init__(agent)
        self.agent = agent
        self.mark_id = mark_id


class RemoveMarks(RegularInfluence):
    """
    Influence to remove all marks matching criteria.
    
    Args:
        agent: The agent removing marks
        category: Category of marks to remove
        position: Optional position to remove marks from
        radius: Optional radius around position
    """
    
    def __init__(self, agent, category: str = "default",
                 position: Optional[Point2D] = None,
                 radius: Optional[float] = None):
        super().__init__(agent)
        self.agent = agent
        self.category = category
        self.position = position
        self.radius = radius


class EmitPheromone(RegularInfluence):
    """
    Influence to emit pheromone into the environment.
    
    Args:
        agent: The agent emitting pheromone
        position: Where to emit the pheromone
        pheromone_id: Identifier of the pheromone type
        amount: Amount of pheromone to emit
    """
    
    def __init__(self, agent, position: Point2D, pheromone_id: str,
                 amount: float):
        super().__init__(agent)
        self.agent = agent
        self.position = position
        self.pheromone_id = pheromone_id
        self.amount = amount
    
    def __repr__(self):
        return f"EmitPheromone(position={self.position}, id={self.pheromone_id}, amount={self.amount})"


# ============================================================================
# System Influences
# ============================================================================

class AgentPositionUpdate(SystemInfluence):
    """
    System influence to update agent position based on dynamics.
    
    This is emitted by the environment to update agent positions
    based on their velocity, acceleration, etc.
    """
    
    def __init__(self, agent, new_position: Point2D):
        super().__init__()
        self.agent = agent
        self.new_position = new_position


class PheromoneFieldUpdate(SystemInfluence):
    """
    System influence to update pheromone field.
    
    This triggers diffusion and evaporation of pheromones.
    """
    
    def __init__(self, environment):
        super().__init__()
        self.environment = environment


class SystemInfluenceAddAgent(SystemInfluence):
    """
    System influence to add an agent to the simulation.
    
    Args:
        agent: The agent to add
        level: The level to add the agent to
    """
    
    def __init__(self, agent, level: str = "LOGO"):
        super().__init__()
        self.agent = agent
        self.level = level


class SystemInfluenceRemoveAgent(SystemInfluence):
    """
    System influence to remove an agent from the simulation.
    
    Args:
        agent: The agent to remove
        level: The level to remove the agent from
    """
    
    def __init__(self, agent, level: str = "LOGO"):
        super().__init__()
        self.agent = agent
        self.level = level


# ============================================================================
# Export C++ or Python implementations
# ============================================================================

if HAS_CPP_INFLUENCES:
    # Use C++ implementations when available
    try:
        ChangePosition = _core.influences.ChangePosition
        ChangeDirection = _core.influences.ChangeDirection
        ChangeSpeed = _core.influences.ChangeSpeed
        ChangeAcceleration = _core.influences.ChangeAcceleration
        Stop = _core.influences.Stop
        _USING_CPP = True
    except AttributeError:
        # C++ module exists but doesn't have influences yet
        ChangePosition = _PyChangePosition
        ChangeDirection = _PyChangeDirection
        ChangeSpeed = _PyChangeSpeed
        ChangeAcceleration = _PyChangeAcceleration
        Stop = _PyStop
        _USING_CPP = False
else:
    # Use Python implementations
    ChangePosition = _PyChangePosition
    ChangeDirection = _PyChangeDirection
    ChangeSpeed = _PyChangeSpeed
    ChangeAcceleration = _PyChangeAcceleration
    Stop = _PyStop
    _USING_CPP = False


# ============================================================================
# Influence Collection
# ============================================================================

class InfluencesMap:
    """
    Container for collecting and organizing influences.
    
    Influences are organized by category (REGULAR, SYSTEM) and type.
    """
    
    def __init__(self):
        self.regular_influences = []
        self.system_influences = []
        self._influences_by_type = {}
    
    def add(self, influence: IInfluence):
        """Add an influence to the map."""
        if isinstance(influence, SystemInfluence):
            self.system_influences.append(influence)
        else:
            self.regular_influences.append(influence)
        
        # Also organize by type
        influence_type = type(influence).__name__
        if influence_type not in self._influences_by_type:
            self._influences_by_type[influence_type] = []
        self._influences_by_type[influence_type].append(influence)
    
    def add_all(self, influences):
        """Add multiple influences."""
        for influence in influences:
            self.add(influence)
    
    def get_by_type(self, influence_type):
        """Get all influences of a specific type."""
        type_name = influence_type.__name__ if hasattr(influence_type, '__name__') else str(influence_type)
        return self._influences_by_type.get(type_name, [])
    
    def get_all_regular(self):
        """Get all regular influences."""
        return self.regular_influences
    
    def get_all_system(self):
        """Get all system influences."""
        return self.system_influences
    
    def clear(self):
        """Clear all influences."""
        self.regular_influences.clear()
        self.system_influences.clear()
        self._influences_by_type.clear()
    
    def __len__(self):
        return len(self.regular_influences) + len(self.system_influences)
    
    def __repr__(self):
        backend = "C++" if _USING_CPP else "Python"
        return f"InfluencesMap(regular={len(self.regular_influences)}, system={len(self.system_influences)}, backend={backend})"


# ============================================================================
# Module Info
# ============================================================================

def get_backend_info():
    """Get information about which backend is being used."""
    return {
        'cpp_available': HAS_CPP_INFLUENCES,
        'using_cpp': _USING_CPP,
        'backend': 'C++' if _USING_CPP else 'Python'
    }


__all__ = [
    # Base classes
    'IInfluence',
    'RegularInfluence',
    'SystemInfluence',
    
    # Movement influences (C++ or Python)
    'ChangePosition',
    'ChangeDirection',
    'ChangeSpeed',
    'ChangeAcceleration',
    'Stop',
    
    # Environmental influences (Python only for now)
    'DropMark',
    'RemoveMark',
    'RemoveMarks',
    'EmitPheromone',
    
    # System influences
    'AgentPositionUpdate',
    'PheromoneFieldUpdate',
    'SystemInfluenceAddAgent',
    'SystemInfluenceRemoveAgent',
    
    # Utilities
    'InfluencesMap',
    'get_backend_info',
]
