import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), 'python'))

# Force Python influences by importing before C++
from similar2logo.influences import ChangePosition as PyChangePosition
from similar2logo.model import Turtle, LogoSimulation
from similar2logo.environment import Environment

class SimpleTurtle(Turtle):
    def decide(self, perception):
        # Use Python influence explicitly
        return [PyChangePosition(self, dx=1.0, dy=0.0)]

def test_python_fallback():
    print("Testing Python fallback warning...")
    env = Environment(100, 100, toroidal=True)
    sim = LogoSimulation(env, num_turtles=1, turtle_class=SimpleTurtle)
    
    # This should trigger the warning
    sim.step()
    print("✓ Warning should have appeared above")

if __name__ == "__main__":
    test_python_fallback()
