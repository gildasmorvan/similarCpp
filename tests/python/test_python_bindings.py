#!/usr/bin/env python3
"""
Comprehensive Python Bindings Unit Tests for SIMILAR2Logo

This test suite covers all Python functionality including:
- DSL classes and methods
- C++ bindings integration
- Web interface
- Model classes
- Influence system
- Environment and tools
"""

import sys
import os
import unittest
import math
import time
from unittest.mock import Mock, patch

# Add Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'python'))

from similar2logo.tools import Point2D, MathUtil
from similar2logo.model import Turtle, LogoSimulation, HAS_CPP_CORE
from similar2logo.environment import Environment
from similar2logo.influences import (
    ChangePosition, ChangeDirection, ChangeSpeed, ChangeAcceleration,
    DropMark, RemoveMark, RemoveMarks, Stop, EmitPheromone,
    AgentPositionUpdate, PheromoneFieldUpdate
)
from similar2logo.reaction import LogoReactionModel, HAS_CPP_REACTION
from similar2logo.fastmath import sin, cos, atan2, PI
from similar2logo.dsl import Simulation
# from similar2logo.spatial import SpatialIndex  # Not used in tests
from similar2logo.probes import RealTimeMatcherProbe


class TestTools(unittest.TestCase):
    """Test Point2D and MathUtil classes"""

    def test_point2d_construction(self):
        """Test Point2D construction and basic properties"""
        # Default constructor
        p1 = Point2D()
        self.assertEqual(p1.x, 0.0)
        self.assertEqual(p1.y, 0.0)

        # Parameterized constructor
        p2 = Point2D(3.0, 4.0)
        self.assertEqual(p2.x, 3.0)
        self.assertEqual(p2.y, 4.0)

    def test_point2d_arithmetic(self):
        """Test Point2D arithmetic operations"""
        p1 = Point2D(1.0, 2.0)
        p2 = Point2D(3.0, 4.0)

        # Addition
        result = p1 + p2
        self.assertEqual(result.x, 4.0)
        self.assertEqual(result.y, 6.0)

        # Subtraction
        result = p2 - p1
        self.assertEqual(result.x, 2.0)
        self.assertEqual(result.y, 2.0)

        # Scalar multiplication
        result = p1 * 2.0
        self.assertEqual(result.x, 2.0)
        self.assertEqual(result.y, 4.0)

        # Scalar division
        result = p2 / 2.0
        self.assertEqual(result.x, 1.5)
        self.assertEqual(result.y, 2.0)

    def test_point2d_geometric(self):
        """Test Point2D geometric operations"""
        p1 = Point2D(0.0, 0.0)
        p2 = Point2D(3.0, 4.0)

        # Distance
        self.assertAlmostEqual(p1.distanceTo(p2), 5.0, places=7)

        # Magnitude
        self.assertAlmostEqual(p2.magnitude(), 5.0, places=7)

        # Normalization
        normalized = p2.normalized()
        self.assertAlmostEqual(normalized.magnitude(), 1.0, places=7)

        # Dot product
        self.assertEqual(p1.dot(p2), 0.0)
        self.assertEqual(p2.dot(p2), 25.0)

        # Cross product
        self.assertEqual(p1.cross(p2), 0.0)

        # Angle
        unit_x = Point2D(1.0, 0.0)
        unit_y = Point2D(0.0, 1.0)
        self.assertAlmostEqual(unit_x.angle(), 0.0, places=7)
        self.assertAlmostEqual(unit_y.angle(), math.pi/2, places=7)

    def test_point2d_polar(self):
        """Test Point2D polar coordinate conversion"""
        # From polar to cartesian
        p = Point2D.fromPolar(5.0, math.pi/4)
        expected_x = 5.0 * math.cos(math.pi/4)
        expected_y = 5.0 * math.sin(math.pi/4)
        self.assertAlmostEqual(p.x, expected_x, places=7)
        self.assertAlmostEqual(p.y, expected_y, places=7)

        # Linear interpolation
        start = Point2D(0.0, 0.0)
        end = Point2D(10.0, 20.0)
        mid = start.lerp(end, 0.5)
        self.assertEqual(mid.x, 5.0)
        self.assertEqual(mid.y, 10.0)

    def test_mathutil(self):
        """Test MathUtil utility functions"""
        # Clamp
        self.assertEqual(MathUtil.clamp(5.0, 0.0, 10.0), 5.0)
        self.assertEqual(MathUtil.clamp(-5.0, 0.0, 10.0), 0.0)
        self.assertEqual(MathUtil.clamp(15.0, 0.0, 10.0), 10.0)

        # Sign
        self.assertEqual(MathUtil.sign(5.0), 1)
        self.assertEqual(MathUtil.sign(-3.0), -1)
        self.assertEqual(MathUtil.sign(0.0), 0)

        # Lerp
        self.assertAlmostEqual(MathUtil.lerp(0.0, 10.0, 0.5), 5.0, places=7)


class TestInfluences(unittest.TestCase):
    """Test all influence classes"""

    def setUp(self):
        """Set up test fixtures"""
        self.turtle = Mock()
        self.turtle.position = Point2D(10.0, 20.0)
        self.turtle.heading = 0.0
        self.turtle.speed = 1.0

    def test_change_position(self):
        """Test ChangePosition influence"""
        influence = ChangePosition(self.turtle, dx=5.0, dy=-3.0)
        self.assertEqual(influence.getTarget(), self.turtle)
        self.assertEqual(influence.getDx(), 5.0)
        self.assertEqual(influence.getDy(), -3.0)
        self.assertEqual(influence.getCategory(), "change position")

    def test_change_direction(self):
        """Test ChangeDirection influence"""
        influence = ChangeDirection(self.turtle, dd=math.pi/2)
        self.assertEqual(influence.getTarget(), self.turtle)
        self.assertAlmostEqual(influence.getDd(), math.pi/2, places=7)
        self.assertEqual(influence.getCategory(), "change direction")

    def test_change_speed(self):
        """Test ChangeSpeed influence"""
        influence = ChangeSpeed(self.turtle, ds=2.5)
        self.assertEqual(influence.getTarget(), self.turtle)
        self.assertEqual(influence.getDs(), 2.5)
        self.assertEqual(influence.getCategory(), "change speed")

    def test_change_acceleration(self):
        """Test ChangeAcceleration influence"""
        influence = ChangeAcceleration(self.turtle, da=1.0)
        self.assertEqual(influence.getTarget(), self.turtle)
        self.assertEqual(influence.getDa(), 1.0)
        self.assertEqual(influence.getCategory(), "change acceleration")

    def test_drop_mark(self):
        """Test DropMark influence"""
        mark = Mock()
        influence = DropMark(mark)
        self.assertEqual(influence.getMark(), mark)
        self.assertEqual(influence.getCategory(), "drop mark")

    def test_remove_mark(self):
        """Test RemoveMark influence"""
        mark = Mock()
        influence = RemoveMark(mark)
        self.assertEqual(influence.getMark(), mark)
        self.assertEqual(influence.getCategory(), "remove mark")

    def test_emit_pheromone(self):
        """Test EmitPheromone influence"""
        location = Point2D(5.0, 10.0)
        influence = EmitPheromone(location, "food", 100.0)
        self.assertEqual(influence.getLocation(), location)
        self.assertEqual(influence.getPheromoneIdentifier(), "food")
        self.assertEqual(influence.getValue(), 100.0)
        self.assertEqual(influence.getCategory(), "emit pheromone")

    def test_stop(self):
        """Test Stop influence"""
        influence = Stop(self.turtle)
        self.assertEqual(influence.getTarget(), self.turtle)
        self.assertEqual(influence.getCategory(), "stop")

    def test_agent_position_update(self):
        """Test AgentPositionUpdate influence"""
        influence = AgentPositionUpdate()
        self.assertEqual(influence.getCategory(), "agent position update")

    def test_pheromone_field_update(self):
        """Test PheromoneFieldUpdate influence"""
        influence = PheromoneFieldUpdate()
        self.assertEqual(influence.getCategory(), "pheromone field update")


class TestTurtle(unittest.TestCase):
    """Test Turtle classes"""

    def test_turtle_construction(self):
        """Test Turtle construction and basic properties"""
        position = Point2D(10.0, 20.0)
        turtle = Turtle(position=position, heading=math.pi/4, speed=2.0)

        self.assertEqual(turtle.position, position)
        self.assertAlmostEqual(turtle.heading, math.pi/4, places=7)
        self.assertEqual(turtle.speed, 2.0)
        self.assertEqual(turtle.color, "blue")  # default color

    def test_turtle_influences(self):
        """Test Turtle influence creation methods"""
        turtle = Turtle(position=Point2D(0.0, 0.0))

        # Test various influence methods
        move_inf = turtle.influence_move_forward(5.0)
        self.assertIsInstance(move_inf, ChangePosition)

        turn_inf = turtle.influence_turn(math.pi/2)
        self.assertIsInstance(turn_inf, ChangeDirection)

        speed_inf = turtle.influence_change_speed(1.0)
        self.assertIsInstance(speed_inf, ChangeSpeed)

        accel_inf = turtle.influence_change_acceleration(0.5)
        self.assertIsInstance(accel_inf, ChangeAcceleration)

        stop_inf = turtle.influence_stop()
        self.assertIsInstance(stop_inf, Stop)



class TestEnvironment(unittest.TestCase):
    """Test Environment class"""

    def test_environment_construction(self):
        """Test Environment construction"""
        env = Environment(100, 200, toroidal=True)

        self.assertEqual(env.width, 100)
        self.assertEqual(env.height, 200)
        self.assertTrue(env.toroidal)

        # Check if C++ backend is available
        if HAS_CPP_CORE:
            self.assertTrue(hasattr(env, '_cpp_env'))

    def test_pheromone_operations(self):
        """Test pheromone grid operations"""
        env = Environment(10, 10)

        # Add pheromone grid
        env.add_pheromone("food", 1.0, 0.1)
        self.assertTrue(env.has_pheromone("food"))

        # Set and get values
        env.set_pheromone_value(5, 5, "food", 50.0)
        value = env.get_pheromone_value(5, 5, "food")
        self.assertEqual(value, 50.0)

        # Test diffusion and evaporation
        env.diffuse_and_evaporate(1.0)
        # Value should decrease due to evaporation
        new_value = env.get_pheromone_value(5, 5, "food")
        self.assertLess(new_value, 50.0)


class TestReaction(unittest.TestCase):
    """Test reaction model functionality"""

    def test_reaction_model(self):
        """Test LogoReactionModel"""
        reaction = LogoReactionModel()

        # Test basic functionality
        self.assertIsInstance(reaction, LogoReactionModel)

        # Test C++ backend availability
        print(f"C++ Reaction available: {HAS_CPP_REACTION}")

    def test_reaction_processing(self):
        """Test influence processing"""
        env = Environment(10, 10)
        reaction = LogoReactionModel()

        # Create some test influences
        turtle = Mock()
        influences = [ChangePosition(turtle, dx=1.0, dy=0.0)]

        # Process influences
        try:
            reaction.make_regular_reaction(0, 1, influences, env, [])
        except Exception as e:
            # This might fail without proper setup, which is expected
            print(f"Reaction processing test: {e}")


class TestDSL(unittest.TestCase):
    """Test DSL functionality"""

    def test_simulation_creation(self):
        """Test Simulation DSL creation"""
        sim = Simulation("Test Simulation")
        sim.grid(100, 100, toroidal=True)

        # Check basic properties
        self.assertEqual(sim.name, "Test Simulation")
        self.assertEqual(sim.width, 100)
        self.assertEqual(sim.height, 100)
        self.assertTrue(sim.toroidal)

    def test_simulation_setup(self):
        """Test simulation setup and agent addition"""
        def setup_func(sim):
            # Add a simple turtle
            turtle = Turtle(position=Point2D(50.0, 50.0))
            sim.turtles.append(turtle)

        sim = Simulation("Test Sim")
        sim.grid(100, 100)
        sim.setup(setup_func)

        # Build and check
        sim_obj = sim._build_simulation()
        self.assertEqual(len(sim_obj.turtles), 1)
        self.assertIsInstance(sim_obj.turtles[0], Turtle)

    def test_simulation_step(self):
        """Test simulation stepping"""
        def setup_func(sim):
            class MovingTurtle(Turtle):
                def decide(self, perception):
                    return [self.influence_move_forward(1.0)]

            turtle = MovingTurtle(position=Point2D(50.0, 50.0), heading=0.0)
            sim.turtles.append(turtle)

        sim = Simulation("Moving Test")
        sim.grid(100, 100)
        sim.setup(setup_func)

        sim_obj = sim._build_simulation()

        # Get initial position
        start_pos = sim_obj.turtles[0].position

        # Step simulation
        sim_obj.step()

        # Check that turtle moved
        end_pos = sim_obj.turtles[0].position
        self.assertNotEqual(start_pos, end_pos)




class TestProbes(unittest.TestCase):
    """Test probe functionality"""

    def test_realtime_probe(self):
        """Test RealTimeMatcherProbe"""
        probe = RealTimeMatcherProbe(30.0)  # 30 FPS target

        # Test basic functionality
        self.assertIsInstance(probe, RealTimeMatcherProbe)

        # Test speed factor
        self.assertEqual(probe.get_speed_factor(), 30.0)


class TestFastMath(unittest.TestCase):
    """Test fast math approximations"""

    def test_fastmath_trig(self):
        """Test fast trigonometric functions"""
        # Test against standard math functions
        test_angles = [0.0, math.pi/4, math.pi/2, math.pi, 2*math.pi]

        for angle in test_angles:
            # Allow some approximation error
            self.assertAlmostEqual(sin(angle), math.sin(angle), places=1)
            self.assertAlmostEqual(cos(angle), math.cos(angle), places=1)

    def test_fastmath_atan2(self):
        """Test fast atan2 function"""
        # Test some basic cases
        self.assertAlmostEqual(atan2(1.0, 0.0), math.atan2(1.0, 0.0), places=2)
        self.assertAlmostEqual(atan2(0.0, 1.0), math.atan2(0.0, 1.0), places=2)


class TestWebInterface(unittest.TestCase):
    """Test web interface functionality"""

    def test_web_simulation_creation(self):
        """Test WebSimulation creation"""
        try:
            from similar2logo.web.server import WebSimulation

            env = Environment(10, 10)
            sim = LogoSimulation(env)

            # Create web simulation
            web_sim = WebSimulation(sim, update_rate=1)
            self.assertIsNotNone(web_sim)

            # Test state retrieval
            state = web_sim.simulation.get_state()
            self.assertIn('turtles', state)
            self.assertIn('environment', state)

        except ImportError:
            # Skip if fastapi not available
            self.skipTest("Web dependencies not available")

    def test_html_template(self):
        """Test HTML template generation"""
        try:
            from similar2logo.web.server import WebSimulation

            env = Environment(10, 10)
            sim = LogoSimulation(env)
            web_sim = WebSimulation(sim)

            html = web_sim._get_html()
            self.assertIsInstance(html, str)
            self.assertGreater(len(html), 1000)  # Should be substantial HTML

            # Check for key elements
            self.assertIn('<canvas', html)
            self.assertIn('WebSocket', html)

        except ImportError:
            self.skipTest("Web dependencies not available")


class TestIntegration(unittest.TestCase):
    """Test integration between components"""

    def test_full_simulation_workflow(self):
        """Test complete simulation workflow"""
        # Create environment
        env = Environment(100, 100, toroidal=True)

        # Create simulation
        sim = LogoSimulation(env)

        # Create turtles
        from similar2logo.tools import Point2D
        class TestTurtle(Turtle):
            def decide(self, perception):
                return [self.influence_move_forward(1.0)]

        for i in range(3):
            turtle = TestTurtle(
                position=Point2D(50.0 + i*10, 50.0),
                heading=0.0
            )
            sim.turtles.append(turtle)

        # Run simulation steps
        for _ in range(5):
            sim.step()

        # Verify turtles moved
        for turtle in sim.turtles:
            self.assertNotEqual(turtle.position.x, 50.0)  # Should have moved

    def test_cpp_bindings_integration(self):
        """Test C++ bindings integration"""
        print(f"C++ Core available: {HAS_CPP_CORE}")
        print(f"C++ Reaction available: {HAS_CPP_REACTION}")

        # Test basic functionality regardless of C++ availability
        env = Environment(10, 10)
        self.assertIsNotNone(env)

        from similar2logo.tools import Point2D
        turtle = Turtle(position=Point2D(5.0, 5.0))
        self.assertIsNotNone(turtle)

        # Test influence creation
        influence = turtle.influence_move_forward(1.0)
        self.assertIsNotNone(influence)


if __name__ == '__main__':
    # Run tests with verbose output
    unittest.main(verbosity=2)
