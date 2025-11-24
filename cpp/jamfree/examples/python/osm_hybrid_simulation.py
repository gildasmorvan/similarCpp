import jamfree as jf
import time
import sys

def run_osm_hybrid_simulation():
    print("\n======================================================================")
    print("OSM HYBRID SIMULATION - Real-world Network")
    print("======================================================================\n")

    # 1. Load OSM Network
    print("[1/5] Loading OSM network...")
    
    # Small OSM XML representing a highway junction
    osm_data = """
    <?xml version="1.0" encoding="UTF-8"?>
    <osm version="0.6" generator="JamFree">
      <node id="1" lat="48.8566" lon="2.3522"/>
      <node id="2" lat="48.8580" lon="2.3540"/>
      <node id="3" lat="48.8590" lon="2.3560"/>
      <node id="4" lat="48.8600" lon="2.3580"/>
      <node id="5" lat="48.8550" lon="2.3500"/>
      <node id="6" lat="48.8540" lon="2.3480"/>
      
      <!-- Highway 1 (East-West) -->
      <way id="101">
        <nd ref="6"/>
        <nd ref="5"/>
        <nd ref="1"/>
        <tag k="highway" v="motorway"/>
        <tag k="lanes" v="3"/>
        <tag k="maxspeed" v="130"/>
      </way>
      
      <way id="102">
        <nd ref="1"/>
        <nd ref="2"/>
        <nd ref="3"/>
        <tag k="highway" v="motorway"/>
        <tag k="lanes" v="3"/>
        <tag k="maxspeed" v="130"/>
      </way>
      
      <!-- Ramp -->
      <way id="103">
        <nd ref="2"/>
        <nd ref="4"/>
        <tag k="highway" v="motorway_link"/>
        <tag k="lanes" v="1"/>
        <tag k="maxspeed" v="70"/>
      </way>
    </osm>
    """
    
    network = jf.OSMParser.parse_string(osm_data)
    print(f"  ✓ Loaded {len(network.roads)} roads")
    
    # Count total lanes
    total_lanes = sum(road.get_num_lanes() for road in network.roads)
    print(f"  ✓ Total lanes: {total_lanes}")

    # 2. Configure Adaptive Simulator
    print("\n[2/5] Configuring adaptive simulator...")
    config = jf.AdaptiveSimulatorConfig()
    
    # Set thresholds
    config.micro_to_macro_density = 0.08  # 80 veh/km
    config.macro_to_micro_density = 0.04  # 40 veh/km
    config.micro_to_macro_count = 40      # Switch at 40 vehicles
    config.macro_to_micro_count = 15      # Switch back at 15
    
    # Critical areas
    config.force_micro_intersections = True
    config.force_micro_ramps = True
    
    print(f"  ✓ Micro→Macro density: {config.micro_to_macro_density} veh/m")
    print(f"  ✓ Macro→Micro density: {config.macro_to_micro_density} veh/m")
    
    simulator = jf.AdaptiveSimulator(config)

    # 3. Register Lanes
    print("\n[3/5] Registering lanes...")
    
    for road in network.roads:
        # Determine if road is critical based on type or length
        # In OSM, ramps are usually critical
        is_ramp = False # We would check tags if exposed, or infer from lane count/length
        
        # Heuristic: Short roads or single lane roads might be critical
        is_critical = road.get_length() < 100 or road.get_num_lanes() == 1
        
        for i in range(road.get_num_lanes()):
            lane = road.get_lane(i)
            simulator.register_lane(lane, is_critical)
            
            status = "CRITICAL" if is_critical else "ADAPTIVE"
            print(f"  ✓ {lane.get_id():<20} ({road.get_length():4.0f}m) - {status}")

    # 4. Spawn Vehicles
    print("\n[4/5] Spawning vehicles...")
    
    vehicles = []
    idm = jf.IDM(desired_speed=30.0) # 108 km/h
    
    # Spawn heavy traffic on main highway to trigger macro mode
    main_roads = [r for r in network.roads if r.get_num_lanes() >= 3]
    count = 0
    
    for road in main_roads:
        for i in range(road.get_num_lanes()):
            lane = road.get_lane(i)
            length = road.get_length()
            
            # High density: 1 vehicle every 15 meters
            num_veh = int(length / 15)
            
            for j in range(num_veh):
                v_id = f"v_{count}"
                vehicle = jf.Vehicle(v_id)
                vehicle.set_current_lane(lane)
                vehicle.set_lane_position(j * 15.0)
                vehicle.set_speed(25.0) # 90 km/h
                
                lane.add_vehicle(vehicle)
                vehicles.append(vehicle)
                count += 1
                
    print(f"  ✓ Spawned {count} vehicles")

    # 5. Run Simulation
    print("\n[5/5] Running simulation...")
    
    dt = 0.1
    num_steps = 100
    
    print(f"  ✓ Time step: {dt} s")
    print(f"  ✓ Total steps: {num_steps}")
    print(f"  ✓ Simulation time: {num_steps * dt:.1f} s")
    
    for step in range(num_steps):
        # Update simulator
        simulator.update(dt, idm)
        
        # Print stats every 20 steps
        if step % 20 == 0:
            stats = simulator.get_statistics()
            print(f"\nStep {step:4d} | Speedup: {stats.speedup_factor:.2f}x")
            print(f"  Micro lanes: {stats.micro_lanes}")
            print(f"  Macro lanes: {stats.macro_lanes}")
            
            # Print modes of specific lanes
            print("  Lane Modes:")
            for road in network.roads:
                lane = road.get_lane(0)
                mode = simulator.get_mode(lane.get_id())
                mode_str = str(mode).split('.')[-1]
                print(f"    {lane.get_id()}: {mode_str}")

    print("\n======================================================================")
    print("SIMULATION COMPLETE")
    print("======================================================================")

if __name__ == "__main__":
    run_osm_hybrid_simulation()
