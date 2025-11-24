"""
Verification script for the SIMILAR2Logo Influence/Reaction Architecture.

This script tests:
1. Turtle emitting influences instead of modifying state
2. Reaction model processing those influences
3. Environment state updating correctly
"""

import sys
import os
import math  # TODO: Replace with fastmath
from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI

# Add python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../python'))

from similar2logo import (
    LogoSimulation, Turtle, Environment, Point2D,
    ChangePosition, ChangeDirection, DropMark, EmitPheromone
)

class TestTurtle(Turtle):
    """A turtle for testing influences."""
    
    def decide(self, perception):
        influences = []
        
        # Test 1: Move forward influence
        # Should move 1.0 unit in current heading (0.0 = North/Up)
        influences.append(self.influence_move_forward(1.0))
        
        # Test 2: Turn influence
        # Should turn 90 degrees (PI/2)
        influences.append(self.influence_turn(PI / 2))
        
        # Test 3: Drop mark influence
        influences.append(self.influence_drop_mark("test_mark"))
        
        return influences

def main():
    print("=" * 70)
    print("SIMILAR2Logo Architecture Verification")
    print("=" * 70)
    
    # Setup
    env = Environment(100, 100)
    sim = LogoSimulation(env, num_turtles=1, turtle_class=TestTurtle)
    turtle = sim.turtles[0]
    
    # Set initial state
    turtle.position = Point2D(50, 50)
    turtle.heading = 0.0  # Facing North (Up)
    
    print(f"Initial State: {turtle}")
    print(f"Marks count: {len(env.marks) if hasattr(env, 'marks') else 0}")
    
    # Run one step
    print("\nRunning 1 simulation step...")
    sim.step()
    
    # Verify results
    print(f"\nFinal State: {turtle}")
    
    # Check Position (Should be 50, 49 due to y-down coordinate system usually, 
    # but let's check the math in influence_move_forward:
    # dx = sin(0) * 1 = 0
    # dy = -cos(0) * 1 = -1
    # So new pos should be (50, 49)
    expected_pos = Point2D(50, 49)
    pos_ok = abs(turtle.position.x - expected_pos.x) < 0.001 and \
             abs(turtle.position.y - expected_pos.y) < 0.001
    
    print(f"Position Check: {'✅ PASS' if pos_ok else '❌ FAIL'}")
    if not pos_ok:
        print(f"  Expected: {expected_pos.to_tuple()}")
        print(f"  Actual:   {turtle.position.to_tuple()}")

    # Check Heading (Should be PI/2 approx 1.57)
    expected_heading = PI / 2
    heading_ok = abs(turtle.heading - expected_heading) < 0.001
    
    print(f"Heading Check:  {'✅ PASS' if heading_ok else '❌ FAIL'}")
    if not heading_ok:
        print(f"  Expected: {expected_heading}")
        print(f"  Actual:   {turtle.heading}")

    # Check Marks
    marks_count = len(env.marks) if hasattr(env, 'marks') else 0
    marks_ok = marks_count == 1
    print(f"Marks Check:    {'✅ PASS' if marks_ok else '❌ FAIL'}")
    
    print("=" * 70)

if __name__ == "__main__":
    main()
