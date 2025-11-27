
import sys
import os
import time

# Add python/src to path to import jamfree
sys.path.append(os.path.join(os.path.dirname(__file__), 'jamfree'))
# Add python/web to path to import engine_manager
sys.path.append(os.path.join(os.path.dirname(__file__), 'web'))

import jamfree
from engine_manager import SimulationEngineManager

def test_full_simulation():
    print("Testing full simulation integration...")
    
    # Path to an OSM file
    osm_file = os.path.join(os.path.dirname(__file__), 'web/uploads/osm_download_48.85_2.34.osm')
    
    if not os.path.exists(osm_file):
        print(f"OSM file not found: {osm_file}")
        # Create a dummy OSM file if needed, or fail
        return

    print(f"Loading OSM file: {osm_file}")
    
    # Parse OSM file
    try:
        network = jamfree.OSMParser.parse_file(osm_file)
        print(f"Network loaded: {len(network.roads)} roads")
    except Exception as e:
        print(f"Error parsing OSM file: {e}")
        return

    # Configuration
    config = {
        'num_vehicles': 5,
        'num_threads': 1
    }
    
    # Initialize Manager
    manager = SimulationEngineManager(network, config)
    
    # Set center lat/lon (usually comes from OSM parser or frontend)
    # For this test, we use dummy values or extract from network if possible
    # RoadNetwork binding exposes min_lat, etc.
    manager.center_lat = (network.min_lat + network.max_lat) / 2
    manager.center_lon = (network.min_lon + network.max_lon) / 2
    
    print("Initializing engine...")
    if not manager.initialize():
        print("Initialization failed")
        return
        
    print("Running simulation steps...")
    for i in range(10):
        data = manager.step()
        if 'error' in data:
            print(f"Step {i} error: {data['error']}")
            break
            
        print(f"Step {i}: {len(data['vehicles'])} vehicles, time={data['performance']['update_time_ms']:.2f}ms")
        if data['vehicles']:
            v = data['vehicles'][0]
            print(f"  Veh 0: pos=({v['lat']:.6f}, {v['lon']:.6f}), speed={v['speed']:.2f} km/h")
            
    print("Test completed.")

if __name__ == "__main__":
    test_full_simulation()
