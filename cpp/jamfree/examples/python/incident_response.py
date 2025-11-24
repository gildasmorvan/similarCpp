import jamfree as jf
import time
import sys

def run_incident_response_simulation():
    print("\n======================================================================")
    print("INCIDENT RESPONSE SIMULATION - Adaptive Hybrid")
    print("======================================================================\n")

    # 1. Create Highway
    print("[1/5] Creating highway...")
    
    # 3 lanes, 2km long
    road = jf.Road("highway", 
                  jf.Point2D(0, 0), 
                  jf.Point2D(2000, 0), 
                  num_lanes=3, 
                  lane_width=3.5)
    
    lanes = [road.get_lane(i) for i in range(3)]
    for lane in lanes:
        lane.set_speed_limit(30.0)

    # 2. Configure Adaptive Simulator
    print("\n[2/5] Configuring adaptive simulator...")
    config = jf.AdaptiveSimulatorConfig()
    config.micro_to_macro_density = 0.06  # Switch earlier (60 veh/km)
    config.macro_to_micro_density = 0.03
    
    simulator = jf.AdaptiveSimulator(config)
    
    for lane in lanes:
        simulator.register_lane(lane, False) # Not critical initially

    # 3. Spawn Traffic
    print("\n[3/5] Spawning traffic...")
    
    vehicles = []
    # Moderate traffic
    for lane in lanes:
        for i in range(40): # 40 vehicles per lane (below threshold of 50)
            v = jf.Vehicle(f"{lane.get_id()}_{i}")
            v.set_current_lane(lane)
            v.set_lane_position(i * 40.0) # Spread out
            v.set_speed(25.0)
            lane.add_vehicle(v)
            vehicles.append(v)
            
    print(f"  ✓ Spawned {len(vehicles)} vehicles")

    # 4. Simulate Incident
    print("\n[4/5] Simulating incident...")
    print("  ! Accident occurs on lane_1 at 1500m")
    
    # Mark lane_1 as critical (force micro) to simulate detailed accident dynamics
    # In a real system, this would be triggered by an incident detection system
    simulator.force_microscopic("lane_1")
    print("  ✓ Forced MICROSCOPIC mode on lane_1")
    
    # Create a stopped vehicle (the accident)
    accident_veh = jf.Vehicle("accident")
    accident_veh.set_current_lane(lanes[1])
    accident_veh.set_lane_position(1500.0)
    accident_veh.set_speed(0.0)
    lanes[1].add_vehicle(accident_veh)

    # 5. Run Simulation
    print("\n[5/5] Running simulation...")
    
    idm = jf.IDM(desired_speed=30.0)
    dt = 0.1
    
    for step in range(200):
        simulator.update(dt, idm)
        
        if step % 50 == 0:
            stats = simulator.get_statistics()
            print(f"\nStep {step:3d}")
            print(f"  Speedup: {stats.speedup_factor:.2f}x")
            
            # Check modes
            l0_mode = str(simulator.get_mode("lane_0")).split('.')[-1]
            l1_mode = str(simulator.get_mode("lane_1")).split('.')[-1]
            l2_mode = str(simulator.get_mode("lane_2")).split('.')[-1]
            
            print(f"  lane_0: {l0_mode} (Vehicles: {len(lanes[0].get_vehicles())})")
            print(f"  lane_1: {l1_mode} (Vehicles: {len(lanes[1].get_vehicles())}) - ACCIDENT")
            print(f"  lane_2: {l2_mode} (Vehicles: {len(lanes[2].get_vehicles())})")
            
            # If congestion builds up on lane_1, it might switch to macro if we didn't force it.
            # But we forced it.
            # Neighboring lanes might switch to macro if density increases due to lane changing (not implemented yet in this simple example)
            # or if we add more inflow.

    print("\n======================================================================")
    print("SIMULATION COMPLETE")
    print("======================================================================")

if __name__ == "__main__":
    run_incident_response_simulation()
