#!/usr/bin/env python3
"""
Adaptive Hybrid Simulation Example
===================================

Demonstrates the micro-macro interface with dynamic switching:
- Long lanes: Macroscopic model (efficient)
- Intersections: Microscopic model (accurate)
- Small lanes: Microscopic model (detailed)
- Dynamic switching based on density and vehicle count

This example creates a network with:
- Highway segments (long lanes) → Macro mode
- Intersection zones (short lanes) → Micro mode
- Adaptive switching based on traffic conditions
"""

import sys
import time
import math

try:
    import jamfree as jf
except ImportError:
    print("Error: jamfree module not found. Please build the C++ extension first.")
    print("Run: cd /Users/morvan/Antigravity/similar/cpp/jamfree && ./build_python.sh")
    sys.exit(1)


def create_highway_network():
    """
    Create a network with highways and intersections.
    
    Network topology:
    
    Highway 1 (1000m) → Intersection (50m) → Highway 2 (1000m)
           ↓                                        ↓
    Highway 3 (800m)                         Highway 4 (800m)
    
    Returns:
        tuple: (roads, lanes, critical_lanes)
    """
    roads = []
    lanes = []
    critical_lanes = set()
    
    # Highway 1 (long, will use macro)
    highway1 = jf.Road(
        "highway1",
        jf.Point2D(0, 0),
        jf.Point2D(1000, 0),
        num_lanes=3,
        lane_width=3.5
    )
    roads.append(highway1)
    for i in range(3):
        lane = highway1.get_lane(i)
        lane.set_speed_limit(33.3)  # 120 km/h
        lanes.append(lane)
    
    # Intersection (short, will use micro)
    intersection = jf.Road(
        "intersection",
        jf.Point2D(1000, 0),
        jf.Point2D(1050, 0),
        num_lanes=3,
        lane_width=3.5
    )
    roads.append(intersection)
    for i in range(3):
        lane = intersection.get_lane(i)
        lane.set_speed_limit(16.7)  # 60 km/h
        lanes.append(lane)
        critical_lanes.add(lane.get_id())  # Mark as critical
    
    # Highway 2 (long, will use macro)
    highway2 = jf.Road(
        "highway2",
        jf.Point2D(1050, 0),
        jf.Point2D(2050, 0),
        num_lanes=3,
        lane_width=3.5
    )
    roads.append(highway2)
    for i in range(3):
        lane = highway2.get_lane(i)
        lane.set_speed_limit(33.3)  # 120 km/h
        lanes.append(lane)
    
    # Highway 3 (medium, adaptive)
    highway3 = jf.Road(
        "highway3",
        jf.Point2D(500, 0),
        jf.Point2D(500, -800),
        num_lanes=2,
        lane_width=3.5
    )
    roads.append(highway3)
    for i in range(2):
        lane = highway3.get_lane(i)
        lane.set_speed_limit(27.8)  # 100 km/h
        lanes.append(lane)
    
    # Highway 4 (medium, adaptive)
    highway4 = jf.Road(
        "highway4",
        jf.Point2D(1550, 0),
        jf.Point2D(1550, -800),
        num_lanes=2,
        lane_width=3.5
    )
    roads.append(highway4)
    for i in range(2):
        lane = highway4.get_lane(i)
        lane.set_speed_limit(27.8)  # 100 km/h
        lanes.append(lane)
    
    return roads, lanes, critical_lanes


def spawn_vehicles(lanes, num_vehicles_per_lane):
    """
    Spawn vehicles on lanes with varying densities.
    
    Args:
        lanes: List of lanes
        num_vehicles_per_lane: Dictionary mapping lane_id to vehicle count
    
    Returns:
        list: All created vehicles
    """
    vehicles = []
    vehicle_id = 0
    
    for lane in lanes:
        lane_id = lane.get_id()
        num_vehicles = num_vehicles_per_lane.get(lane_id, 0)
        
        if num_vehicles == 0:
            continue
        
        # Distribute vehicles evenly along the lane
        lane_length = lane.get_length()
        spacing = lane_length / (num_vehicles + 1)
        
        for i in range(num_vehicles):
            vehicle = jf.Vehicle(
                f"v{vehicle_id}",
                length=5.0,
                max_speed=lane.get_speed_limit(),
                max_accel=3.0,
                max_decel=6.0
            )
            
            # Position along lane
            position = (i + 1) * spacing
            vehicle.set_current_lane(lane)
            vehicle.set_lane_position(position)
            
            # Initial speed (80-100% of speed limit)
            speed = lane.get_speed_limit() * (0.8 + 0.2 * (i / max(num_vehicles - 1, 1)))
            vehicle.set_speed(speed)
            
            lane.add_vehicle(vehicle)
            vehicles.append(vehicle)
            vehicle_id += 1
    
    return vehicles


def print_mode_distribution(simulator, step):
    """Print the current mode distribution across lanes."""
    stats = simulator.get_statistics()
    
    print(f"\n{'='*70}")
    print(f"Step {step:4d} | Mode Distribution")
    print(f"{'='*70}")
    print(f"  Total Lanes:        {stats.total_lanes}")
    print(f"  Microscopic:        {stats.micro_lanes:2d} lanes ({stats.micro_lanes/stats.total_lanes*100:5.1f}%)")
    print(f"  Macroscopic:        {stats.macro_lanes:2d} lanes ({stats.macro_lanes/stats.total_lanes*100:5.1f}%)")
    print(f"  Transitioning:      {stats.transitioning_lanes:2d} lanes")
    print(f"  Total Vehicles:     {stats.total_vehicles}")
    print(f"  Avg Density:        {stats.avg_density:.4f} veh/m")
    print(f"  Update Time:        {stats.total_update_time_ms:.2f} ms")
    print(f"  Speedup Factor:     {stats.speedup_factor:.2f}x")
    print(f"{'='*70}")


def run_simulation():
    """Run the adaptive hybrid simulation."""
    
    print("\n" + "="*70)
    print("ADAPTIVE HYBRID SIMULATION - Micro-Macro Interface")
    print("="*70)
    
    # Create network
    print("\n[1/5] Creating highway network...")
    roads, lanes, critical_lanes = create_highway_network()
    print(f"  ✓ Created {len(roads)} roads with {len(lanes)} lanes")
    print(f"  ✓ {len(critical_lanes)} critical lanes (intersections)")
    
    # Configure adaptive simulator
    print("\n[2/5] Configuring adaptive simulator...")
    config = jf.AdaptiveSimulatorConfig()
    
    # Density thresholds (veh/m)
    config.micro_to_macro_density = 0.08  # Switch to macro at 80 veh/km
    config.macro_to_micro_density = 0.04  # Switch to micro at 40 veh/km
    
    # Vehicle count thresholds
    config.micro_to_macro_count = 50  # Switch to macro with 50+ vehicles
    config.macro_to_micro_count = 20  # Switch to micro with <20 vehicles
    
    # Macroscopic resolution
    config.macro_num_cells = 50  # 50 cells for macroscopic lanes
    
    # Hysteresis to prevent oscillation
    config.hysteresis_factor = 1.2
    
    # Force microscopic at critical areas
    config.force_micro_intersections = True
    config.force_micro_ramps = True
    
    print(f"  ✓ Micro→Macro density: {config.micro_to_macro_density} veh/m")
    print(f"  ✓ Macro→Micro density: {config.macro_to_micro_density} veh/m")
    print(f"  ✓ Hysteresis factor: {config.hysteresis_factor}")
    
    # Create simulator
    simulator = jf.AdaptiveSimulator(config)
    
    # Register lanes
    print("\n[3/5] Registering lanes...")
    for lane in lanes:
        is_critical = lane.get_id() in critical_lanes
        simulator.register_lane(lane, is_critical)
        mode_str = "CRITICAL (micro)" if is_critical else "ADAPTIVE"
        print(f"  ✓ {lane.get_id():20s} ({lane.get_length():6.0f}m) - {mode_str}")
    
    # Spawn vehicles with varying densities
    print("\n[4/5] Spawning vehicles...")
    vehicle_distribution = {
        # Highway 1 - High density (will switch to macro)
        "highway1_lane_0": 60,
        "highway1_lane_1": 55,
        "highway1_lane_2": 50,
        
        # Intersection - Low density (stays micro)
        "intersection_lane_0": 5,
        "intersection_lane_1": 4,
        "intersection_lane_2": 3,
        
        # Highway 2 - Medium density (adaptive)
        "highway2_lane_0": 40,
        "highway2_lane_1": 35,
        "highway2_lane_2": 30,
        
        # Highway 3 - Low density (stays micro)
        "highway3_lane_0": 15,
        "highway3_lane_1": 12,
        
        # Highway 4 - Medium density (adaptive)
        "highway4_lane_0": 25,
        "highway4_lane_1": 20,
    }
    
    vehicles = spawn_vehicles(lanes, vehicle_distribution)
    total_vehicles = sum(vehicle_distribution.values())
    print(f"  ✓ Spawned {total_vehicles} vehicles across {len(lanes)} lanes")
    
    # Create IDM model
    idm = jf.IDM(
        desired_speed=30.0,  # 108 km/h
        time_headway=1.5,
        min_gap=2.0,
        max_accel=1.0,
        comfortable_decel=1.5
    )
    
    # Simulation parameters
    dt = 0.1  # 100ms time step
    num_steps = 200
    print_interval = 50
    
    print(f"\n[5/5] Running simulation...")
    print(f"  ✓ Time step: {dt} s")
    print(f"  ✓ Total steps: {num_steps}")
    print(f"  ✓ Simulation time: {num_steps * dt} s")
    
    # Run simulation
    start_time = time.time()
    
    for step in range(num_steps):
        # Update simulation
        simulator.update(dt, idm)
        
        # Print statistics periodically
        if step % print_interval == 0 or step == num_steps - 1:
            print_mode_distribution(simulator, step)
            
            # Print individual lane modes
            print("\nLane Modes:")
            for lane in lanes:
                mode = simulator.get_mode(lane.get_id())
                mode_str = str(mode).split('.')[-1]  # Extract enum name
                num_vehicles = len(lane.get_vehicles())
                print(f"  {lane.get_id():20s}: {mode_str:15s} ({num_vehicles:3d} vehicles)")
    
    elapsed_time = time.time() - start_time
    
    # Final statistics
    print("\n" + "="*70)
    print("SIMULATION COMPLETE")
    print("="*70)
    stats = simulator.get_statistics()
    print(f"  Simulation time:    {num_steps * dt:.1f} s")
    print(f"  Wall-clock time:    {elapsed_time:.2f} s")
    print(f"  Real-time factor:   {(num_steps * dt) / elapsed_time:.1f}x")
    print(f"  Speedup (vs micro): {stats.speedup_factor:.2f}x")
    print(f"  Final vehicles:     {stats.total_vehicles}")
    print(f"  Micro lanes:        {stats.micro_lanes}/{stats.total_lanes}")
    print(f"  Macro lanes:        {stats.macro_lanes}/{stats.total_lanes}")
    print("="*70 + "\n")


if __name__ == "__main__":
    try:
        run_simulation()
    except KeyboardInterrupt:
        print("\n\nSimulation interrupted by user.")
    except Exception as e:
        print(f"\n\nError: {e}")
        import traceback
        traceback.print_exc()
