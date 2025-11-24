#!/usr/bin/env python3
"""
OpenStreetMap import example for JamFree.

This example demonstrates:
- Loading road networks from OSM XML files
- Filtering roads by type
- Creating traffic simulations on real-world networks
- Visualizing OSM road networks
"""

import jamfree

def main():
    print("=" * 60)
    print("JamFree - OpenStreetMap Import Example")
    print("=" * 60)
    print()

    # Example 1: Parse OSM file
    print("Example 1: Parsing OSM file")
    print("-" * 60)
    
    # In a real scenario, you would download OSM data from:
    # https://www.openstreetmap.org/export
    # or use Overpass API
    
    osm_file = "example_area.osm"  # Replace with actual file
    
    try:
        # Parse OSM file
        network = jamfree.OSMParser.parse_file(osm_file)
        
        print(f"Loaded: {network}")
        print(f"Bounding box:")
        print(f"  Latitude:  {network.min_lat:.6f} to {network.max_lat:.6f}")
        print(f"  Longitude: {network.min_lon:.6f} to {network.max_lon:.6f}")
        print(f"  Roads: {len(network.roads)}")
        print()
        
        # List some roads
        print("Sample roads:")
        for i, road in enumerate(network.roads[:5]):
            print(f"  {road}")
        print()
        
    except FileNotFoundError:
        print(f"Note: {osm_file} not found.")
        print("To use this example, download OSM data from:")
        print("  https://www.openstreetmap.org/export")
        print()

    # Example 2: Get default values for highway types
    print("Example 2: Highway type defaults")
    print("-" * 60)
    
    highway_types = [
        "motorway",
        "trunk",
        "primary",
        "secondary",
        "residential"
    ]
    
    print(f"{'Highway Type':<15} {'Lanes':<8} {'Speed (km/h)'}")
    print("-" * 40)
    for htype in highway_types:
        lanes = jamfree.OSMParser.get_default_lanes(htype)
        speed = jamfree.OSMParser.get_default_speed_limit(htype, "FR")
        print(f"{htype:<15} {lanes:<8} {speed:.0f}")
    print()

    # Example 3: Coordinate conversion
    print("Example 3: Coordinate conversion")
    print("-" * 60)
    
    # Example coordinates (Paris, France)
    center_lat = 48.8566
    center_lon = 2.3522
    
    # Point 1 km north
    point_lat = center_lat + 0.009  # ~1 km
    point_lon = center_lon
    
    pos = jamfree.OSMParser.lat_lon_to_meters(
        point_lat, point_lon,
        center_lat, center_lon
    )
    
    print(f"Center: ({center_lat:.6f}, {center_lon:.6f})")
    print(f"Point:  ({point_lat:.6f}, {point_lon:.6f})")
    print(f"Local coordinates: ({pos.x:.1f}m, {pos.y:.1f}m)")
    print(f"Distance: {pos.length():.1f}m")
    print()

    # Example 4: Creating a simulation from OSM data
    print("Example 4: Simulation setup (conceptual)")
    print("-" * 60)
    
    print("To create a simulation from OSM data:")
    print("1. Parse OSM file: network = OSMParser.parse_file('area.osm')")
    print("2. Filter roads: motorways = [r for r in network.roads if ...]")
    print("3. Create vehicles on roads")
    print("4. Run simulation with IDM/MOBIL")
    print()
    
    print("Example code:")
    print("""
    # Load network
    network = jamfree.OSMParser.parse_file('highway.osm')
    
    # Create IDM model
    idm = jamfree.IDM(desired_speed=jamfree.kmh_to_ms(120))
    
    # Add vehicles to roads
    for road in network.roads:
        vehicle = jamfree.Vehicle(f"car_{road.get_id()}")
        vehicle.set_current_lane(road.get_lane(0))
        vehicle.set_speed(jamfree.kmh_to_ms(100))
        # ... simulation loop
    """)
    print()

    print("=" * 60)
    print("OSM Import Example Complete!")
    print("=" * 60)
    print()
    print("To download OSM data:")
    print("  1. Go to https://www.openstreetmap.org")
    print("  2. Navigate to your area of interest")
    print("  3. Click 'Export' and download as .osm file")
    print("  4. Or use Overpass API for programmatic access")
    print()

if __name__ == "__main__":
    main()
