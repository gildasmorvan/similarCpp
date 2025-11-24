import jamfree as jf
import time
import sys

def run_traffic_signal_simulation():
    print("\n======================================================================")
    print("TRAFFIC SIGNAL OPTIMIZATION - Split Lane Strategy")
    print("======================================================================\n")

    # 1. Create Intersection Approach
    print("[1/5] Creating intersection approach...")
    
    # Strategy: Split approach into "Queue Area" (Micro) and "Approach" (Adaptive)
    
    # Approach Road (Adaptive) - 950m
    approach_road = jf.Road("approach", 
                           jf.Point2D(0, 0), 
                           jf.Point2D(950, 0), 
                           num_lanes=1, 
                           lane_width=3.5)
    approach_lane = approach_road.get_lane(0)
    approach_lane.set_speed_limit(15.0)
    
    # Queue Area (Critical/Micro) - 50m before light
    queue_road = jf.Road("queue_area", 
                        jf.Point2D(950, 0), 
                        jf.Point2D(1000, 0), 
                        num_lanes=1, 
                        lane_width=3.5)
    queue_lane = queue_road.get_lane(0)
    queue_lane.set_speed_limit(15.0)
    
    # Connect them (logically - in a real graph we'd link them)
    # Here we just simulate them independently for the mode demonstration
    
    # 2. Configure Adaptive Simulator
    print("\n[2/5] Configuring adaptive simulator...")
    config = jf.AdaptiveSimulatorConfig()
    simulator = jf.AdaptiveSimulator(config)
    
    # Register lanes
    simulator.register_lane(approach_lane, False) # Adaptive
    simulator.register_lane(queue_lane, True)     # Critical (Always Micro)
    
    print("  ✓ approach_lane: ADAPTIVE")
    print("  ✓ queue_lane:    CRITICAL (Always Micro)")

    # 3. Spawn Traffic
    print("\n[3/5] Spawning traffic...")
    
    # Fill approach lane with high density (simulating peak hour)
    # 950m / 7.5m per veh = ~126 vehicles
    for i in range(100):
        v = jf.Vehicle(f"v_{i}")
        v.set_current_lane(approach_lane)
        v.set_lane_position(i * 8.0)
        v.set_speed(10.0)
        approach_lane.add_vehicle(v)
        
    # Fill queue lane (stopped at red light)
    for i in range(5):
        v = jf.Vehicle(f"q_{i}")
        v.set_current_lane(queue_lane)
        v.set_lane_position(i * 7.5)
        v.set_speed(0.0)
        queue_lane.add_vehicle(v)
        
    print(f"  ✓ Approach: {len(approach_lane.get_vehicles())} vehicles")
    print(f"  ✓ Queue:    {len(queue_lane.get_vehicles())} vehicles")

    # 4. Run Simulation
    print("\n[4/5] Running simulation (Red Light Phase)...")
    
    idm = jf.IDM(desired_speed=15.0)
    dt = 0.1
    
    # Run for 100 steps
    for step in range(100):
        simulator.update(dt, idm)
        
        if step == 50:
            print("\n  --- Traffic Light Turns GREEN ---")
            # In a real sim, we'd release the vehicles in queue_lane
            # Here we just observe the modes
            
        if step % 20 == 0:
            approach_mode = str(simulator.get_mode("approach_lane")).split('.')[-1]
            queue_mode = str(simulator.get_mode("queue_lane")).split('.')[-1]
            
            print(f"Step {step:3d} | Approach: {approach_mode:<11} | Queue: {queue_mode:<11}")

    print("\n======================================================================")
    print("SIMULATION COMPLETE")
    print("======================================================================")

if __name__ == "__main__":
    run_traffic_signal_simulation()
