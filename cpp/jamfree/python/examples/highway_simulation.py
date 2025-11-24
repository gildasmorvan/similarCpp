#!/usr/bin/env python3
"""
Highway simulation example using JamFree Python bindings.

This example demonstrates:
- Creating a multi-lane highway
- Adding vehicles with different speeds
- Using IDM car-following model
- Using MOBIL lane-changing model
- Running a traffic simulation
"""

import jamfree
import time

def main():
    print("=" * 60)
    print("JamFree Python - Highway Simulation with Lane Changing")
    print("=" * 60)
    print()

    # Create a 3-lane highway
    highway = jamfree.Road(
        "highway_1",
        jamfree.Point2D(0, 0),
        jamfree.Point2D(3000, 0),  # 3 km
        3,    # 3 lanes
        3.5   # 3.5m lane width
    )
    
    print(f"Created: {highway}")
    print()

    # Create IDM car-following model
    idm = jamfree.IDM(
        desired_speed=jamfree.kmh_to_ms(120),  # 120 km/h
        time_headway=1.5,
        min_gap=2.0,
        max_accel=1.0,
        comfortable_decel=1.5
    )
    
    print(f"Car-following model: {idm}")
    
    # Create MOBIL lane-changing model
    mobil = jamfree.MOBIL(
        politeness=0.3,
        threshold=0.1,
        max_safe_decel=4.0,
        bias_right=0.2
    )
    
    print(f"Lane-changing model: {mobil}")
    print()

    # Create vehicles
    vehicles = []
    
    # Slow truck in right lane
    truck = jamfree.Vehicle("truck", length=12.0)
    truck.set_current_lane(highway.get_lane(0))
    truck.set_lane_position(500.0)
    truck.set_speed(jamfree.kmh_to_ms(80))
    highway.get_lane(0).get_vehicles()  # This adds it to the lane
    vehicles.append(truck)
    print(f"Added: {truck} in lane 0")
    
    # Fast cars behind truck
    for i in range(5):
        car = jamfree.Vehicle(f"car_{i}")
        car.set_current_lane(highway.get_lane(0))
        car.set_lane_position(100.0 + i * 50.0)
        car.set_speed(jamfree.kmh_to_ms(100 + i * 2))
        vehicles.append(car)
        print(f"Added: {car} in lane 0")
    
    print()
    print("Running simulation for 60 seconds...")
    print()

    # Simulation parameters
    dt = 0.1  # Time step: 0.1s
    num_steps = 600  # 60 seconds
    report_interval = 100  # Report every 10 seconds

    # Simulation loop
    for step in range(num_steps):
        # Phase 1: Update vehicle dynamics (IDM)
        for vehicle in vehicles:
            lane = vehicle.get_current_lane()
            if not lane:
                continue
            
            # Find leader (simplified - would need proper implementation)
            leader = None  # In real implementation, find leader in lane
            
            # Calculate acceleration
            accel = idm.calculate_acceleration(vehicle, leader)
            
            # Update vehicle
            vehicle.update(dt, accel)
        
        # Phase 2: Lane changing (MOBIL)
        for vehicle in vehicles:
            current_lane = vehicle.get_current_lane()
            if not current_lane:
                continue
            
            lane_idx = current_lane.get_index()
            
            # Get adjacent lanes
            left_lane = highway.get_lane(lane_idx + 1) if lane_idx < 2 else None
            right_lane = highway.get_lane(lane_idx - 1) if lane_idx > 0 else None
            
            # Decide lane change
            decision = mobil.decide_lane_change(
                vehicle, current_lane, 
                left_lane, right_lane, 
                idm
            )
            
            # Execute lane change
            if decision == jamfree.LaneChangeDirection.LEFT and left_lane:
                print(f"  [t={step*dt:.1f}s] {vehicle.get_id()} changes LEFT "
                      f"(lane {lane_idx} -> {lane_idx+1})")
                vehicle.set_current_lane(left_lane)
                
            elif decision == jamfree.LaneChangeDirection.RIGHT and right_lane:
                print(f"  [t={step*dt:.1f}s] {vehicle.get_id()} changes RIGHT "
                      f"(lane {lane_idx} -> {lane_idx-1})")
                vehicle.set_current_lane(right_lane)
        
        # Periodic status report
        if step % report_interval == 0 and step > 0:
            print()
            print(f"Status at t={step*dt:.1f}s:")
            for vehicle in vehicles:
                lane = vehicle.get_current_lane()
                lane_idx = lane.get_index() if lane else -1
                print(f"  {vehicle.get_id():12s}: lane={lane_idx}, "
                      f"pos={vehicle.get_lane_position():7.1f}m, "
                      f"speed={jamfree.ms_to_kmh(vehicle.get_speed()):5.1f} km/h")
            print()

    # Final state
    print()
    print("Final State (t=60s):")
    for vehicle in vehicles:
        lane = vehicle.get_current_lane()
        lane_idx = lane.get_index() if lane else -1
        print(f"{vehicle.get_id()}:")
        print(f"  Lane: {lane_idx}")
        print(f"  Position: {vehicle.get_lane_position():.1f} m")
        print(f"  Speed: {jamfree.ms_to_kmh(vehicle.get_speed()):.1f} km/h")
        print(f"  Acceleration: {vehicle.get_acceleration():.2f} m/s²")
        print()

    print("=" * 60)
    print("Simulation complete!")
    print("=" * 60)

if __name__ == "__main__":
    main()
