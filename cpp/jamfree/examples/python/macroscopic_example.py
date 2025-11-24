#!/usr/bin/env python3
"""
Macroscopic Traffic Flow Simulation Example

This example demonstrates:
1. LWR (Lighthill-Whitham-Richards) model
2. CTM (Cell Transmission Model)
3. Micro-Macro bridge for hybrid simulation
"""

import jamfree
import matplotlib.pyplot as plt
import numpy as np

def example_lwr():
    """Demonstrate LWR macroscopic model."""
    print("=" * 60)
    print("LWR Model Example")
    print("=" * 60)
    
    # Create LWR model
    # 1 km road, 100 cells, free-flow speed 120 km/h
    lwr = jamfree.LWR(
        free_flow_speed=33.3,  # 120 km/h in m/s
        jam_density=0.15,       # 150 vehicles/km
        road_length=1000.0,     # 1 km
        num_cells=100
    )
    
    print(f"Created: {lwr}")
    print(f"Critical density: {lwr.get_critical_density():.4f} veh/m")
    print(f"Max flow: {lwr.get_max_flow():.4f} veh/s")
    
    # Initialize with a traffic wave
    for i in range(lwr.get_num_cells()):
        x = i / lwr.get_num_cells()
        # Create a density wave
        density = 0.05 + 0.05 * np.sin(2 * np.pi * x)
        lwr.set_density(i, density)
    
    # Simulate
    dt = 0.1  # 100ms time step
    num_steps = 100
    
    densities_over_time = []
    for step in range(num_steps):
        lwr.update(dt)
        densities = [lwr.get_density(i) for i in range(lwr.get_num_cells())]
        densities_over_time.append(densities)
    
    # Plot results
    plt.figure(figsize=(12, 5))
    
    # Space-time diagram
    plt.subplot(1, 2, 1)
    plt.imshow(np.array(densities_over_time).T, aspect='auto', cmap='hot', origin='lower')
    plt.colorbar(label='Density (veh/m)')
    plt.xlabel('Time step')
    plt.ylabel('Cell index')
    plt.title('LWR: Space-Time Diagram')
    
    # Final density profile
    plt.subplot(1, 2, 2)
    final_densities = densities_over_time[-1]
    positions = [i * lwr.get_cell_length() for i in range(lwr.get_num_cells())]
    plt.plot(positions, final_densities, 'b-', linewidth=2)
    plt.xlabel('Position (m)')
    plt.ylabel('Density (veh/m)')
    plt.title('LWR: Final Density Profile')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('lwr_example.png', dpi=150)
    print(f"\n✓ Saved plot to lwr_example.png")

def example_ctm():
    """Demonstrate CTM macroscopic model."""
    print("\n" + "=" * 60)
    print("CTM Model Example")
    print("=" * 60)
    
    # Create CTM model
    ctm = jamfree.CTM(
        free_flow_speed=33.3,  # 120 km/h
        wave_speed=5.56,        # 20 km/h
        jam_density=0.15,
        road_length=1000.0,
        num_cells=100
    )
    
    print(f"Created: {ctm}")
    print(f"Critical density: {ctm.get_critical_density():.4f} veh/m")
    print(f"Max flow: {ctm.get_max_flow():.4f} veh/s")
    
    # Initialize with congestion
    cell_length = ctm.get_cell_length()
    for i in range(ctm.get_num_cells()):
        if 30 <= i <= 70:
            # Congested region
            num_veh = 0.12 * cell_length  # High density
        else:
            # Free-flow region
            num_veh = 0.03 * cell_length  # Low density
        ctm.set_num_vehicles(i, num_veh)
    
    # Simulate
    dt = 0.1
    num_steps = 200
    
    densities_over_time = []
    flows_over_time = []
    
    for step in range(num_steps):
        ctm.update(dt)
        densities = [ctm.get_density(i) for i in range(ctm.get_num_cells())]
        flows = [ctm.get_flow(i) for i in range(ctm.get_num_cells())]
        densities_over_time.append(densities)
        flows_over_time.append(flows)
    
    # Plot results
    plt.figure(figsize=(12, 10))
    
    # Density space-time
    plt.subplot(2, 2, 1)
    plt.imshow(np.array(densities_over_time).T, aspect='auto', cmap='hot', origin='lower')
    plt.colorbar(label='Density (veh/m)')
    plt.xlabel('Time step')
    plt.ylabel('Cell index')
    plt.title('CTM: Density Evolution')
    
    # Flow space-time
    plt.subplot(2, 2, 2)
    plt.imshow(np.array(flows_over_time).T, aspect='auto', cmap='viridis', origin='lower')
    plt.colorbar(label='Flow (veh/s)')
    plt.xlabel('Time step')
    plt.ylabel('Cell index')
    plt.title('CTM: Flow Evolution')
    
    # Final profiles
    plt.subplot(2, 2, 3)
    positions = [i * cell_length for i in range(ctm.get_num_cells())]
    plt.plot(positions, densities_over_time[-1], 'b-', linewidth=2)
    plt.xlabel('Position (m)')
    plt.ylabel('Density (veh/m)')
    plt.title('CTM: Final Density')
    plt.grid(True)
    
    plt.subplot(2, 2, 4)
    plt.plot(positions, flows_over_time[-1], 'g-', linewidth=2)
    plt.xlabel('Position (m)')
    plt.ylabel('Flow (veh/s)')
    plt.title('CTM: Final Flow')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('ctm_example.png', dpi=150)
    print(f"\n✓ Saved plot to ctm_example.png")

def example_fundamental_diagram():
    """Plot fundamental diagram for LWR and CTM."""
    print("\n" + "=" * 60)
    print("Fundamental Diagram")
    print("=" * 60)
    
    lwr = jamfree.LWR()
    ctm = jamfree.CTM()
    
    # Generate density range
    densities = np.linspace(0, lwr.get_jam_density(), 100)
    
    # Calculate speed and flow
    lwr_speeds = [lwr.speed_from_density(d) for d in densities]
    lwr_flows = [lwr.flow_from_density(d) for d in densities]
    
    ctm_speeds = []
    ctm_flows = []
    for d in densities:
        # For CTM, we need to set density and query
        cell_length = ctm.get_cell_length()
        num_veh = d * cell_length
        ctm.set_num_vehicles(50, num_veh)  # Middle cell
        ctm_speeds.append(ctm.get_speed(50))
        ctm_flows.append(ctm.get_flow(50))
    
    # Plot
    plt.figure(figsize=(12, 5))
    
    # Speed-density
    plt.subplot(1, 2, 1)
    plt.plot(densities, lwr_speeds, 'b-', linewidth=2, label='LWR')
    plt.plot(densities, ctm_speeds, 'r--', linewidth=2, label='CTM')
    plt.xlabel('Density (veh/m)')
    plt.ylabel('Speed (m/s)')
    plt.title('Speed-Density Relationship')
    plt.legend()
    plt.grid(True)
    
    # Flow-density
    plt.subplot(1, 2, 2)
    plt.plot(densities, lwr_flows, 'b-', linewidth=2, label='LWR')
    plt.plot(densities, ctm_flows, 'r--', linewidth=2, label='CTM')
    plt.axvline(lwr.get_critical_density(), color='k', linestyle=':', label='Critical density')
    plt.xlabel('Density (veh/m)')
    plt.ylabel('Flow (veh/s)')
    plt.title('Flow-Density Relationship')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('fundamental_diagram.png', dpi=150)
    print(f"\n✓ Saved plot to fundamental_diagram.png")

def example_micro_macro_bridge():
    """Demonstrate micro-macro bridge."""
    print("\n" + "=" * 60)
    print("Micro-Macro Bridge Example")
    print("=" * 60)
    
    # Create microscopic simulation
    road = jamfree.Road("test_road", 
                        jamfree.Point2D(0, 0), 
                        jamfree.Point2D(1000, 0),
                        num_lanes=1)
    lane = road.get_lane(0)
    lane.set_speed_limit(33.3)  # 120 km/h
    
    # Add vehicles
    idm = jamfree.IDM(desired_speed=33.3)
    num_vehicles = 20
    
    for i in range(num_vehicles):
        vehicle = jamfree.Vehicle(f"v_{i}")
        vehicle.set_current_lane(lane)
        vehicle.set_lane_position(i * 50.0)  # 50m spacing
        vehicle.set_speed(30.0 + np.random.randn() * 2.0)
        lane.add_vehicle(vehicle)
    
    # Run microscopic simulation
    dt = 0.1
    for step in range(100):
        for vehicle in lane.get_vehicles():
            leader = lane.get_leader(vehicle)
            acc = idm.calculate_acceleration(vehicle, leader)
            vehicle.update(dt, acc)
    
    # Extract macroscopic state
    print("\nExtracting macroscopic state from microscopic simulation...")
    
    num_cells = 50
    density_profile = jamfree.MicroMacroBridge.extract_density_profile(lane, num_cells)
    flow_profile = jamfree.MicroMacroBridge.extract_flow_profile(lane, num_cells)
    speed_profile = jamfree.MicroMacroBridge.extract_speed_profile(lane, num_cells)
    
    # Calculate aggregate statistics
    stats = jamfree.MicroMacroBridge.calculate_aggregate_stats(lane)
    print(f"\nAggregate Statistics:")
    print(f"  Vehicles: {stats.num_vehicles}")
    print(f"  Avg Density: {stats.avg_density:.4f} veh/m")
    print(f"  Avg Flow: {stats.avg_flow:.4f} veh/s")
    print(f"  Avg Speed: {stats.avg_speed:.2f} m/s ({stats.avg_speed * 3.6:.1f} km/h)")
    
    # Initialize macroscopic models from microscopic state
    lwr = jamfree.LWR(road_length=1000.0, num_cells=num_cells)
    ctm = jamfree.CTM(road_length=1000.0, num_cells=num_cells)
    
    jamfree.MicroMacroBridge.initialize_lwr_from_lane(lane, lwr)
    jamfree.MicroMacroBridge.initialize_ctm_from_lane(lane, ctm)
    
    print("\n✓ Initialized LWR and CTM from microscopic state")
    
    # Plot comparison
    plt.figure(figsize=(12, 8))
    
    cell_length = 1000.0 / num_cells
    positions = [i * cell_length for i in range(num_cells)]
    
    plt.subplot(3, 1, 1)
    plt.plot(positions, density_profile, 'b-', linewidth=2, label='Microscopic')
    plt.plot(positions, [lwr.get_density(i) for i in range(num_cells)], 
             'r--', linewidth=2, label='LWR')
    plt.ylabel('Density (veh/m)')
    plt.title('Density Profile Comparison')
    plt.legend()
    plt.grid(True)
    
    plt.subplot(3, 1, 2)
    plt.plot(positions, flow_profile, 'b-', linewidth=2, label='Microscopic')
    plt.plot(positions, [lwr.get_flow(i) for i in range(num_cells)], 
             'r--', linewidth=2, label='LWR')
    plt.ylabel('Flow (veh/s)')
    plt.title('Flow Profile Comparison')
    plt.legend()
    plt.grid(True)
    
    plt.subplot(3, 1, 3)
    plt.plot(positions, speed_profile, 'b-', linewidth=2, label='Microscopic')
    plt.plot(positions, [lwr.get_speed(i) for i in range(num_cells)], 
             'r--', linewidth=2, label='LWR')
    plt.xlabel('Position (m)')
    plt.ylabel('Speed (m/s)')
    plt.title('Speed Profile Comparison')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('micro_macro_bridge.png', dpi=150)
    print(f"\n✓ Saved plot to micro_macro_bridge.png")

if __name__ == "__main__":
    print("JamFree Macroscopic Models Example")
    print("=" * 60)
    
    # Run examples
    example_lwr()
    example_ctm()
    example_fundamental_diagram()
    example_micro_macro_bridge()
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
