"""SIMILAR2Logo Python Package - Agent-Based Modeling Framework"""

__version__ = "0.1.0"
__author__ = "SIMILAR Team"

# Try to import C++ core, but don't fail if not available
try:
    from . import _core
    HAS_CPP_CORE = True
except ImportError:
    HAS_CPP_CORE = False
    _core = None

# Import Python modules (these work without C++ core)
from .tools import Point2D, MathUtil
from .environment import Pheromone, Environment
from .influences import *
from .reaction import LogoReactionModel
from .model import Turtle, LogoSimulation
from .probes import IProbe, RealTimeMatcherProbe, ProbeManager
from . import fastmath

__all__ = [
    "Point2D",
    "MathUtil",
    "Pheromone",
    "Environment",
    "Turtle",
    "LogoSimulation",
    "LogoReactionModel",
    "HAS_CPP_CORE",
    "fastmath",
    # Probes
    "IProbe",
    "RealTimeMatcherProbe",
    "ProbeManager",
    # Influences
    "IInfluence",
    "RegularInfluence",
    "SystemInfluence",
    "ChangePosition",
    "ChangeDirection",
    "ChangeSpeed",
    "ChangeAcceleration",
    "Stop",
    "DropMark",
    "RemoveMark",
    "RemoveMarks",
    "EmitPheromone",
    "AgentPositionUpdate",
    "PheromoneFieldUpdate",
    "SystemInfluenceAddAgent",
    "SystemInfluenceRemoveAgent",
    "InfluencesMap",
    "_core",
]
