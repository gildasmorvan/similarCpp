#!/usr/bin/env python3
"""
DSL Examples Integration Tests

Test the actual DSL examples to ensure they work end-to-end
"""

import sys
import os
import unittest
import tempfile
import time

# Add paths
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'python'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'examples', 'python'))

from similar2logo.dsl import Simulation


class TestDSLExamples(unittest.TestCase):
    """Test actual DSL examples"""

    def test_boids_dsl_basic(self):
        """Test basic boids DSL functionality"""
        try:
            # Import boids example
            from boids_dsl import BoidParams, Boid, setup_simulation

            # Test parameter creation
            params = BoidParams(num_boids=5)
            self.assertEqual(params.grid_size, 100)
            self.assertEqual(params.num_boids, 5)

            # Test boid creation
            from similar2logo.tools import Point2D
            boid = Boid(position=Point2D(25.0, 25.0), heading=0.0)
            self.assertIsNotNone(boid)

            # Test simulation setup
            sim = Simulation("Boids Test")
            sim.grid(50, 50, toroidal=True)
            sim.setup(setup_simulation)

            # Build and step
            sim_obj = sim._build_simulation()
            initial_count = len(sim_obj.turtles)
            self.assertGreater(initial_count, 0)

            # Step simulation
            sim_obj.step()
            self.assertEqual(len(sim_obj.turtles), initial_count)

        except ImportError as e:
            self.skipTest(f"Boids example not available: {e}")

    def test_multiturmite_dsl_basic(self):
        """Test basic multiturmite DSL functionality"""
        try:
            # Import multiturmite example
            from multiturmite import Turmite, setup

            # Test turmite creation
            from similar2logo.tools import Point2D
            turmite = Turmite(position=Point2D(25.0, 25.0), heading=0.0)
            self.assertIsNotNone(turmite)

            # Test simulation setup
            sim = Simulation("Turmite Test")
            sim.grid(50, 50, toroidal=True)
            sim.setup(setup)

            # Build and step
            sim_obj = sim._build_simulation()
            initial_count = len(sim_obj.turtles)
            self.assertGreater(initial_count, 0)

            # Step simulation
            sim_obj.step()
            self.assertEqual(len(sim_obj.turtles), initial_count)

            # Check for marks
            state = sim_obj.get_state()
            marks = state.get('marks', [])
            # Might not have marks immediately, but should have the structure
            self.assertIsInstance(marks, list)

        except ImportError as e:
            self.skipTest(f"Multiturmite example not available: {e}")

    def test_dsl_simulation_lifecycle(self):
        """Test full DSL simulation lifecycle"""
        # Create a comprehensive test
        def custom_setup(sim):
            class CustomTurtle:
                def __init__(self, **kwargs):
                    self.position = kwargs.get('position', (0, 0))
                    self.heading = kwargs.get('heading', 0.0)
                    self.speed = kwargs.get('speed', 1.0)
                    self.color = kwargs.get('color', 'blue')

                def decide(self, perception):
                    return []  # No-op for this test

            # Add multiple turtles
            from similar2logo.tools import Point2D
            for i in range(3):
                turtle = CustomTurtle(
                    position=Point2D(i*10, i*10),
                    heading=i*0.5,
                    speed=1.0 + i*0.5
                )
                sim.turtles.append(turtle)

        # Create simulation
        sim = Simulation("Lifecycle Test")
        sim.grid(100, 100, toroidal=True)
        sim.setup(custom_setup)

        # Build simulation
        sim_obj = sim._build_simulation()
        self.assertEqual(len(sim_obj.turtles), 3)

        # Test state extraction
        state = sim_obj.get_state()
        self.assertEqual(len(state['turtles']), 3)
        self.assertEqual(state['environment']['width'], 100)
        self.assertEqual(state['environment']['height'], 100)

        # Test multiple steps
        for _ in range(5):
            sim_obj.step()

        # Verify simulation is still intact
        self.assertEqual(len(sim_obj.turtles), 3)
        final_state = sim_obj.get_state()
        self.assertEqual(len(final_state['turtles']), 3)


class TestDSLPerformance(unittest.TestCase):
    """Test DSL performance characteristics"""

    def test_large_simulation_performance(self):
        """Test performance with larger number of agents"""
        def large_setup(sim):
            class SimpleAgent:
                def __init__(self, **kwargs):
                    self.position = kwargs.get('position', (0, 0))
                    self.heading = kwargs.get('heading', 0.0)
                    self.speed = 1.0
                    self.color = 'blue'

                def decide(self, perception):
                    return []  # Minimal decision for performance test

            # Add many agents
            from similar2logo.tools import Point2D
            num_agents = 100
            for i in range(num_agents):
                x = (i % 10) * 10
                y = (i // 10) * 10
                agent = SimpleAgent(position=Point2D(x, y), heading=i * 0.1)
                sim.turtles.append(agent)

        sim = Simulation("Performance Test")
        sim.grid(100, 100, toroidal=True)
        sim.setup(large_setup)

        sim_obj = sim._build_simulation()

        # Time a few steps
        import time
        start_time = time.time()

        num_steps = 10
        for _ in range(num_steps):
            sim_obj.step()

        end_time = time.time()
        total_time = end_time - start_time

        # Should complete in reasonable time (allow generous limit for CI)
        self.assertLess(total_time, 30.0, f"Simulation too slow: {total_time}s for {num_steps} steps")

        # Verify all agents still exist
        self.assertEqual(len(sim_obj.turtles), 100)


class TestDSLWebIntegration(unittest.TestCase):
    """Test DSL with web interface integration"""

    def test_web_simulation_creation(self):
        """Test creating web-enabled simulation"""
        try:
            from similar2logo.web.server import WebSimulation
            from similar2logo.model import LogoSimulation
            from similar2logo.environment import Environment
            from similar2logo.model import Turtle

            # Create proper simulation
            env = Environment(100, 100)
            sim = LogoSimulation(env)

            # Add turtle
            from similar2logo.tools import Point2D
            turtle = Turtle(position=Point2D(50, 50))
            sim.turtles.append(turtle)

            # Create web simulation
            web_sim = WebSimulation(sim, update_rate=5)
            self.assertIsNotNone(web_sim)

            # Test HTML generation
            html = web_sim._get_html()
            self.assertIn('<html>', html)
            self.assertIn('WebSocket', html)

        except ImportError:
            self.skipTest("Web dependencies not available")


if __name__ == '__main__':
    unittest.main(verbosity=2)
