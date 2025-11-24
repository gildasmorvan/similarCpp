
import unittest
from similar2logo.dsl import Simulation, Turtle
from dataclasses import dataclass

@dataclass
class TestParams:
    val: float = 1.0

class TestAgent(Turtle):
    def __init__(self, params: TestParams = None, **kwargs):
        super().__init__(**kwargs)
        self.params = params
    
    def decide(self, perception):
        """Simple decide method for testing."""
        return []

class TestParameterUpdate(unittest.TestCase):
    def test_parameter_update_mechanism(self):
        # Setup
        params = TestParams(val=1.0)
        sim = Simulation("Test").agents(TestAgent, count=10, params=params)._build_simulation()
        
        # Verify initial state
        self.assertEqual(sim.turtles[0].params.val, 1.0)
        
        # Simulate what server.py does
        new_params = {"val": 5.0, "speed": 10.0}
        updated = {}
        
        for turtle in sim.turtles:
            # Update direct attributes
            for key, value in new_params.items():
                # Try setting on turtle directly
                if hasattr(turtle, key):
                        try:
                            setattr(turtle, key, value)
                            updated[key] = value
                        except Exception:
                            pass
                
                # Try setting on turtle.params
                if hasattr(turtle, 'params') and hasattr(turtle.params, key):
                    try:
                        setattr(turtle.params, key, value)
                        updated[key] = value
                    except Exception:
                        pass
        
        # Verify updates
        # 1. Check shared params
        self.assertEqual(sim.turtles[0].params.val, 5.0)
        self.assertEqual(params.val, 5.0) # Should update the original object too since it's shared
        
        # 2. Check individual attributes
        self.assertEqual(sim.turtles[0].speed, 10.0)
        self.assertEqual(sim.turtles[9].speed, 10.0)
        
        print("Verification successful: Parameters updated correctly.")

if __name__ == '__main__':
    unittest.main()
