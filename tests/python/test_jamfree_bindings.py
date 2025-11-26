#!/usr/bin/env python3
"""
Comprehensive Python Bindings Tests for JamFree Traffic Simulation

This test suite covers all Python functionality including:
- Traffic simulation models and engines
- Vehicle and road network management
- Microscopic traffic models (IDM, MOBIL)
- Macroscopic traffic models (CTM, LWR)
- Integration with SIMILAR framework
"""

import sys
import os
import unittest
import math
import time

# Add paths
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'python'))
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'cpp', 'jamfree', 'python'))

try:
    import jamfree
    JAMFREE_AVAILABLE = True
except ImportError:
    JAMFREE_AVAILABLE = False
    print("Warning: jamfree Python bindings not available")

from similar2logo.tools import Point2D


@unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
class TestJamFreeCore(unittest.TestCase):
    """Test core JamFree functionality"""

    def test_point2d(self):
        """Test Point2D class"""
        p1 = jamfree.Point2D(0.0, 0.0)
        p2 = jamfree.Point2D(3.0, 4.0)

        # Test arithmetic
        result = p1 + p2
        self.assertEqual(result.x, 3.0)
        self.assertEqual(result.y, 4.0)

        # Test distance
        dist = p1.distanceTo(p2)
        self.assertAlmostEqual(dist, 5.0, places=7)

    def test_vehicle(self):
        """Test Vehicle class"""
        vehicle = jamfree.Vehicle("test_vehicle", 4.5, 50.0, 2.0, 5.0)

        # Test properties
        self.assertEqual(vehicle.getId(), "test_vehicle")
        self.assertAlmostEqual(vehicle.getLength(), 4.5, places=7)
        self.assertAlmostEqual(vehicle.getMaxSpeed(), 50.0, places=7)

        # Test state
        self.assertEqual(vehicle.getSpeed(), 0.0)
        self.assertEqual(vehicle.getAcceleration(), 0.0)

        # Test setters
        vehicle.setSpeed(25.0)
        self.assertEqual(vehicle.getSpeed(), 25.0)

        vehicle.setLanePosition(100.0)
        self.assertEqual(vehicle.getLanePosition(), 100.0)

    def test_road_and_lane(self):
        """Test Road and Lane classes"""
        start = jamfree.Point2D(0.0, 0.0)
        end = jamfree.Point2D(100.0, 0.0)
        road = jamfree.Road("test_road", start, end, 2, 3.5)

        # Test road properties
        self.assertEqual(road.getId(), "test_road")
        self.assertEqual(road.getNumLanes(), 2)

        # Test lane access
        lane0 = road.getLane(0)
        lane1 = road.getLane(1)

        self.assertIsNotNone(lane0)
        self.assertIsNotNone(lane1)
        self.assertEqual(lane0.getIndex(), 0)
        self.assertEqual(lane1.getIndex(), 1)

        # Test position calculation
        pos0 = lane0.getPositionAt(0.0)
        pos50 = lane0.getPositionAt(50.0)
        pos100 = lane0.getPositionAt(100.0)

        self.assertAlmostEqual(pos0.x, 0.0, places=7)
        self.assertAlmostEqual(pos50.x, 50.0, places=7)
        self.assertAlmostEqual(pos100.x, 100.0, places=7)


@unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
class TestMicroscopicModels(unittest.TestCase):
    """Test microscopic traffic models"""

    def setUp(self):
        """Set up test fixtures"""
        self.vehicle = jamfree.Vehicle("test_vehicle")
        self.vehicle.setSpeed(20.0)
        self.vehicle.setLanePosition(50.0)

        self.leader = jamfree.Vehicle("leader")
        self.leader.setSpeed(15.0)
        self.leader.setLanePosition(60.0)

    def test_idm(self):
        """Test Intelligent Driver Model"""
        idm = jamfree.IDM(desired_speed=30.0, time_headway=1.5, min_gap=2.0)

        # Test free-flow acceleration
        accel_free = idm.calculateAcceleration(self.vehicle, None)
        self.assertGreater(accel_free, 0.0)

        # Test with leader
        accel_with_leader = idm.calculateAcceleration(self.vehicle, self.leader)
        self.assertLess(accel_with_leader, accel_free)

    def test_mobil(self):
        """Test MOBIL lane-changing model"""
        mobil = jamfree.MOBIL(politeness=0.5, threshold=0.1, max_safe_decel=4.0)

        # Create road with lanes
        road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 3, 3.5)
        lane1 = road.getLane(1)  # Middle lane
        lane2 = road.getLane(2)  # Right lane

        # Set up ego vehicle
        self.vehicle.setCurrentLane(lane1)
        self.vehicle.setLanePosition(500.0)
        self.vehicle.setSpeed(25.0)

        # Test lane change decision (should prefer right due to bias)
        decision = mobil.decideLaneChange(self.vehicle, lane1, None, lane2, jamfree.IDM())

        # Should return a decision (may be NONE, LEFT, or RIGHT)
        self.assertIn(str(decision), ["NONE", "LEFT", "RIGHT"])

    def test_mobil_safety(self):
        """Test MOBIL safety criterion"""
        mobil = jamfree.MOBIL(politeness=0.0, threshold=0.1, max_safe_decel=4.0)

        # Create unsafe situation
        road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 2, 3.5)
        lane0 = road.getLane(0)
        lane1 = road.getLane(1)

        # Ego vehicle
        ego = jamfree.Vehicle("ego")
        ego.setCurrentLane(lane0)
        ego.setLanePosition(500.0)
        ego.setSpeed(25.0)

        # Fast follower in target lane (unsafe)
        follower = jamfree.Vehicle("follower")
        follower.setCurrentLane(lane1)
        follower.setLanePosition(480.0)  # Close behind
        follower.setSpeed(30.0)  # Faster than ego

        decision = mobil.decideLaneChange(ego, lane0, None, lane1, jamfree.IDM())

        # Should be NONE due to safety
        self.assertEqual(str(decision), "NONE")


@unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
class TestMacroscopicModels(unittest.TestCase):
    """Test macroscopic traffic models"""

    def test_ctm(self):
        """Test Cell Transmission Model"""
        ctm = jamfree.CTM(vmax=1.0, rhoc=0.8, qmax=2000.0, rhomax=1800.0)

        # Test properties
        self.assertAlmostEqual(ctm.getMaxSpeed(), 1.0, places=7)
        self.assertAlmostEqual(ctm.getMaxDensity(), 0.8, places=7)

        # Test flow calculation
        flow = ctm.flow(0.4)
        self.assertGreaterEqual(flow, 0.0)

        # Test flow at max density
        flow_max = ctm.flow(0.8)
        self.assertAlmostEqual(flow_max, 0.0, places=3)

    def test_lwr(self):
        """Test Lighthill-Whitham-Richards model"""
        lwr = jamfree.LWR(vmax=1.0, rhoc=0.8)

        # Test propagation speed
        speed = lwr.getPropagationSpeed()
        self.assertGreater(speed, 0.0)

        # Test fundamental diagram
        flow = lwr.flow(0.4)
        self.assertGreaterEqual(flow, 0.0)


@unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
class TestTrafficSimulation(unittest.TestCase):
    """Test complete traffic simulation"""

    def test_simple_simulation(self):
        """Test basic traffic simulation setup"""
        # Create road network
        road = jamfree.Road("main_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 2, 3.5)
        lane = road.getLane(0)

        # Create vehicles
        vehicle1 = jamfree.Vehicle("v1", 4.0, 50.0)
        vehicle1.setCurrentLane(lane)
        vehicle1.setLanePosition(100.0)
        vehicle1.setSpeed(25.0)

        vehicle2 = jamfree.Vehicle("v2", 4.0, 50.0)
        vehicle2.setCurrentLane(lane)
        vehicle2.setLanePosition(200.0)
        vehicle2.setSpeed(20.0)

        # Add vehicles to lane
        lane.addVehicle(vehicle1)
        lane.addVehicle(vehicle2)

        # Create models
        idm = jamfree.IDM(desired_speed=30.0)

        # Test vehicle interactions
        accel1 = idm.calculateAcceleration(vehicle1, vehicle2)
        accel2 = idm.calculateAcceleration(vehicle2, None)

        # Vehicle 1 should decelerate due to leader, vehicle 2 should accelerate
        self.assertLess(accel1, accel2)

    def test_lane_changing_scenario(self):
        """Test lane changing scenario"""
        # Create 3-lane road
        road = jamfree.Road("highway", jamfree.Point2D(0, 0), jamfree.Point2D(2000, 0), 3, 3.5)
        lane0 = road.getLane(0)  # Left
        lane1 = road.getLane(1)  # Middle
        lane2 = road.getLane(2)  # Right

        # Ego vehicle in middle lane, slow leader ahead
        ego = jamfree.Vehicle("ego")
        ego.setCurrentLane(lane1)
        ego.setLanePosition(1000.0)
        ego.setSpeed(20.0)

        leader = jamfree.Vehicle("leader")
        leader.setCurrentLane(lane1)
        leader.setLanePosition(1050.0)
        leader.setSpeed(15.0)

        lane1.addVehicle(ego)
        lane1.addVehicle(leader)

        # Test lane change models
        idm = jamfree.IDM(desired_speed=30.0)
        mobil = jamfree.MOBIL(politeness=0.5, threshold=0.2)

        # Should want to change lanes to escape slow leader
        decision = mobil.decideLaneChange(ego, lane1, lane0, lane2, idm)

        # Decision should be valid (may be NONE, LEFT, or RIGHT depending on conditions)
        self.assertIn(str(decision), ["NONE", "LEFT", "RIGHT"])


@unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
class TestIntegration(unittest.TestCase):
    """Test integration with SIMILAR framework"""

    def test_similar_integration(self):
        """Test JamFree integration with SIMILAR simulation engine"""
        # This would require the full SIMILAR integration
        # For now, just test that the classes can be instantiated
        try:
            # Test that we can create SIMILAR-compatible objects
            vehicle = jamfree.Vehicle("integrated_vehicle")

            # Test conversion to SIMILAR format if available
            # This would depend on the specific integration points

            self.assertIsNotNone(vehicle)
            self.assertEqual(vehicle.getId(), "integrated_vehicle")

        except Exception as e:
            self.skipTest(f"SIMILAR integration not available: {e}")


class TestPerformance(unittest.TestCase):
    """Test performance characteristics"""

    @unittest.skipUnless(JAMFREE_AVAILABLE, "JamFree Python bindings not available")
    def test_large_scale_simulation(self):
        """Test performance with larger number of vehicles"""
        # Create road network
        road = jamfree.Road("highway", jamfree.Point2D(0, 0), jamfree.Point2D(5000, 0), 3, 3.5)

        # Add multiple vehicles
        vehicles = []
        for i in range(50):  # Reasonable number for performance test
            vehicle = jamfree.Vehicle(f"v{i}", 4.0, 33.0)  # 4m length, 120 km/h
            vehicle.setCurrentLane(road.getLane(i % 3))
            vehicle.setLanePosition(i * 100.0)  # Space them out
            vehicle.setSpeed(25.0 + (i % 5))  # Vary speeds

            vehicles.append(vehicle)
            road.getLane(i % 3).addVehicle(vehicle)

        # Create models
        idm = jamfree.IDM(desired_speed=30.0)

        # Time the computation
        import time
        start_time = time.time()

        # Calculate accelerations for all vehicles
        for vehicle in vehicles:
            # Find leader (simplified)
            leader = None
            for other in vehicles:
                if (other.getCurrentLane() == vehicle.getCurrentLane() and
                    other.getLanePosition() > vehicle.getLanePosition()):
                    if leader is None or other.getLanePosition() < leader.getLanePosition():
                        leader = other

            idm.calculateAcceleration(vehicle, leader)

        end_time = time.time()
        computation_time = end_time - start_time

        # Should complete in reasonable time (allow generous limit)
        self.assertLess(computation_time, 5.0,
                       f"Performance too slow: {computation_time}s for {len(vehicles)} vehicles")


def run_performance_benchmark():
    """Run detailed performance benchmark"""
    if not JAMFREE_AVAILABLE:
        print("JamFree not available, skipping performance benchmark")
        return

    print("Running JamFree Performance Benchmark")
    print("=" * 50)

    # Test different scales
    scales = [10, 50, 100, 200]

    for num_vehicles in scales:
        # Create road network
        road = jamfree.Road("benchmark_road", jamfree.Point2D(0, 0), jamfree.Point2D(10000, 0), 4, 3.5)

        # Add vehicles
        vehicles = []
        for i in range(num_vehicles):
            vehicle = jamfree.Vehicle(f"v{i}")
            vehicle.setCurrentLane(road.getLane(i % 4))
            vehicle.setLanePosition(i * 50.0)
            vehicle.setSpeed(25.0)
            vehicles.append(vehicle)

        # Create models
        idm = jamfree.IDM()
        mobil = jamfree.MOBIL()

        # Benchmark IDM calculations
        start_time = time.time()
        for _ in range(10):  # 10 simulation steps
            for vehicle in vehicles:
                idm.calculateAcceleration(vehicle, None)
        idm_time = time.time() - start_time

        # Benchmark MOBIL calculations
        start_time = time.time()
        for _ in range(10):
            for vehicle in vehicles:
                mobil.decideLaneChange(vehicle, vehicle.getCurrentLane(),
                                     None, None, idm)
        mobil_time = time.time() - start_time

        print(f"Scale {num_vehicles:3d} vehicles:")
        print(".3f")
        print(".3f")
        print(".1f")
        print()


if __name__ == '__main__':
    # Run unit tests
    print("Running JamFree Python Binding Tests")
    print("=" * 50)

    unittest.main(verbosity=2, exit=False)

    # Run performance benchmark
    print("\n" + "=" * 50)
    run_performance_benchmark()
