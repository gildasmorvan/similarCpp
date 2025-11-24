import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'python'))

from similar2logo.model import Turtle, LogoSimulation, HAS_CPP_CORE
from similar2logo.reaction import HAS_CPP_REACTION
from similar2logo.environment import Environment
from similar2logo.tools import Point2D

class MovingTurtle(Turtle):
    def decide(self, perception):
        return [self.influence_move_forward(1.0)]

def test_cpp_reaction():
    print(f"HAS_CPP_CORE: {HAS_CPP_CORE}")
    print(f"HAS_CPP_REACTION: {HAS_CPP_REACTION}")

    print("Initializing environment...")
    env = Environment(100, 100, toroidal=True)
    print(f"Has _cpp_env: {hasattr(env, '_cpp_env')}")
    print(f"Environment type: {type(env)}")
    print(f"Environment base classes: {type(env).__bases__}")
    
    print("Initializing simulation...")
    sim = LogoSimulation(env, num_turtles=1, turtle_class=MovingTurtle)
    
    turtle = sim.turtles[0]
    print(f"Turtle has _pls: {hasattr(turtle, '_pls')}")
    start_pos = turtle.position
    print(f"Start position: {start_pos}")
    
    # Test influence creation
    influences = turtle.decide({})
    print(f"Number of influences: {len(influences)}")
    if influences:
        inf = influences[0]
        print(f"Influence type: {type(inf)}")
        print(f"Influence has getTarget: {hasattr(inf, 'getTarget')}")
    
    print("Running step...")
    sim.step()
    
    end_pos = turtle.position
    print(f"End position: {end_pos}")
    
    if start_pos.x != end_pos.x or start_pos.y != end_pos.y:
        print("SUCCESS: Turtle moved!")
    else:
        print("FAILURE: Turtle did not move.")

if __name__ == "__main__":
    test_cpp_reaction()
