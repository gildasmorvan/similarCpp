"""
JamFree Web UI - Flask application for traffic simulation visualization.

This provides a web interface for:
- Loading OSM road networks
- Configuring simulations
- Real-time visualization
- Accessing real traffic data sources
"""

from flask import Flask, render_template, jsonify, request, send_from_directory
from flask_cors import CORS
import json
import os
from pathlib import Path
import math
from routing import RoutingEngine

# Try to import jamfree (may not be built yet)
try:
    import jamfree
    JAMFREE_AVAILABLE = True
except ImportError:
    JAMFREE_AVAILABLE = False
    print("Warning: jamfree not available. Build Python bindings first.")

app = Flask(__name__)
CORS(app)

# Configuration
UPLOAD_FOLDER = Path('uploads')
UPLOAD_FOLDER.mkdir(exist_ok=True)
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

# Global simulation state
simulation_state = {
    'network': None,
    'vehicles': [],
    'running': False,
    'step': 0,
    'config': {
        'num_vehicles': 200,  # Increased for large-scale simulation
        'desired_speed': 120,  # km/h
        'time_headway': 1.5,
        'politeness': 0.3,
        # Performance optimizations
        'use_idm_lookup': True,      # Use IDM lookup tables (30-40% faster)
        'use_spatial_index': True,   # Use spatial index (O(log N) queries)
        'use_multithreading': True,  # Use multithreading for large N
        'use_adaptive_hybrid': True, # Adaptive micro/macro switching
        'adaptive_threshold': 50,    # Switch to macro at N vehicles per lane
    },
    # Optimization components
    'spatial_index': None,
    'adaptive_simulator': None,
    'performance_stats': {
        'avg_update_time_ms': 0.0,
        'micro_lanes': 0,
        'macro_lanes': 0,
        'speedup_factor': 1.0,
    }
}

@app.route('/')
def index():
    """Main page."""
    return render_template('index.html', jamfree_available=JAMFREE_AVAILABLE)

@app.route('/api/status')
def get_status():
    """Get simulation status."""
    return jsonify({
        'jamfree_available': JAMFREE_AVAILABLE,
        'running': simulation_state['running'],
        'step': simulation_state['step'],
        'num_vehicles': len(simulation_state['vehicles']),
        'network_loaded': simulation_state['network'] is not None
    })

@app.route('/api/config', methods=['GET', 'POST'])
def config():
    """Get or update simulation configuration."""
    if request.method == 'POST':
        data = request.json
        simulation_state['config'].update(data)
        return jsonify({'status': 'ok', 'config': simulation_state['config']})
    else:
        return jsonify(simulation_state['config'])

@app.route('/api/osm/upload', methods=['POST'])
def upload_osm():
    """Upload OSM file."""
    if 'file' not in request.files:
        return jsonify({'error': 'No file provided'}), 400
    
    file = request.files['file']
    if file.filename == '':
        return jsonify({'error': 'No file selected'}), 400
    
    if not file.filename.endswith('.osm'):
        return jsonify({'error': 'File must be .osm format'}), 400
    
    # Save file
    filepath = UPLOAD_FOLDER / file.filename
    file.save(filepath)
    
    # Parse OSM file if jamfree available
    if JAMFREE_AVAILABLE:
        try:
            network = jamfree.OSMParser.parse_file(str(filepath))
            simulation_state['network'] = network
            
            # Extract road geometry for visualization
            road_geometry = []
            center_lat = (network.min_lat + network.max_lat) / 2.0
            center_lon = (network.min_lon + network.max_lon) / 2.0
            
            for road in network.roads:
                road_id = road.get_id()
                for i in range(road.get_num_lanes()):
                    lane = road.get_lane(i)
                    lane_id = lane.get_id()
                    
                    # Get start and end positions
                    # Note: This is a simplification. Real OSM roads have multiple points.
                    # For now we just use start and end of the road/lane.
                    # In a full implementation we would export the full geometry.
                    
                    # Convert meters back to lat/lon for visualization
                    # This is an approximation since we don't have the original nodes stored in Road object easily accessible
                    # But we can assume the road is straight for this visualization or use the bounds
                    
                    # Actually, we need the geometry. 
                    # If the Road object doesn't store full geometry, we can't easily draw it perfectly.
                    # But we can try to use the OSM nodes if we had them.
                    # For now, let's just return the bounds or a simple line.
                    
                    # Let's use the vehicle positions to infer geometry? No, that's too late.
                    # Let's assume we can get start/end from the Road object if it has them.
                    # The C++ Road object has getStart() and getEnd() returning Point2D.
                    
                    start = road.get_start()
                    end = road.get_end()
                    
                    start_geo = jamfree.OSMParser.meters_to_lat_lon(start.x, start.y, center_lat, center_lon)
                    end_geo = jamfree.OSMParser.meters_to_lat_lon(end.x, end.y, center_lat, center_lon)
                    
                    road_geometry.append({
                        'id': lane_id,
                        'road_id': road_id,
                        'points': [
                            [start_geo.x, start_geo.y],
                            [end_geo.x, end_geo.y]
                        ]
                    })

            return jsonify({
                'status': 'ok',
                'filename': file.filename,
                'roads': len(network.roads),
                'geometry': road_geometry,
                'bounds': {
                    'min_lat': network.min_lat,
                    'max_lat': network.max_lat,
                    'min_lon': network.min_lon,
                    'max_lon': network.max_lon
                }
            })
        except Exception as e:
            return jsonify({'error': str(e)}), 500
    else:
        return jsonify({
            'status': 'ok',
            'filename': file.filename,
            'message': 'File uploaded but not parsed (jamfree not available)'
        })

@app.route('/api/osm/download', methods=['POST'])
def download_osm():
    """Download OSM data from Overpass API."""
    data = request.json
    bbox = data.get('bbox')  # [min_lat, min_lon, max_lat, max_lon]
    
    if not bbox or len(bbox) != 4:
        return jsonify({'error': 'Invalid bounding box'}), 400
    
    # Import here to avoid dependency if not used
    import requests
    
    # Overpass API query
    query = f"""
    [out:xml];
    (
      way["highway"~"motorway|trunk|primary|secondary"]({bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]});
    );
    out body;
    >;
    out skel qt;
    """
    
    try:
        response = requests.post(
            "https://overpass-api.de/api/interpreter",
            data=query,
            timeout=30
        )
        response.raise_for_status()
        
        # Save to file
        filename = f"osm_download_{bbox[0]}_{bbox[1]}.osm"
        filepath = UPLOAD_FOLDER / filename
        filepath.write_text(response.text)
        
        # Parse if jamfree available
        if JAMFREE_AVAILABLE:
            network = jamfree.OSMParser.parse_string(response.text)
            simulation_state['network'] = network
            
            # Extract road geometry for visualization
            road_geometry = []
            center_lat = (network.min_lat + network.max_lat) / 2.0
            center_lon = (network.min_lon + network.max_lon) / 2.0
            
            for road in network.roads:
                road_id = road.get_id()
                for i in range(road.get_num_lanes()):
                    lane = road.get_lane(i)
                    lane_id = lane.get_id()
                    
                    start = road.get_start()
                    end = road.get_end()
                    
                    start_geo = jamfree.OSMParser.meters_to_lat_lon(start.x, start.y, center_lat, center_lon)
                    end_geo = jamfree.OSMParser.meters_to_lat_lon(end.x, end.y, center_lat, center_lon)
                    
                    road_geometry.append({
                        'id': lane_id,
                        'road_id': road_id,
                        'points': [
                            [start_geo.x, start_geo.y],
                            [end_geo.x, end_geo.y]
                        ]
                    })

            return jsonify({
                'status': 'ok',
                'filename': filename,
                'roads': len(network.roads),
                'geometry': road_geometry,
                'bounds': {
                    'min_lat': network.min_lat,
                    'max_lat': network.max_lat,
                    'min_lon': network.min_lon,
                    'max_lon': network.max_lon
                }
            })
        else:
            return jsonify({
                'status': 'ok',
                'filename': filename,
                'message': 'Downloaded but not parsed'
            })
            
    except Exception as e:
        return jsonify({'error': str(e)}), 500

# Traffic data module is optional - don't block if it fails
simulation_state['traffic_aggregator'] = None

@app.route('/api/config/keys', methods=['POST'])
def update_keys():
    """Update API keys for traffic data sources."""
    keys = request.json
    try:
        import traffic_data
        simulation_state['traffic_aggregator'] = traffic_data.create_aggregator(keys)
        return jsonify({'status': 'ok'})
    except Exception as e:
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/data/traffic', methods=['GET'])
def get_traffic_data():
    """Get real-time traffic data from various sources."""
    source = request.args.get('source', 'tomtom')
    try:
        lat = float(request.args.get('lat', 48.8566))
        lon = float(request.args.get('lon', 2.3522))
        radius = float(request.args.get('radius', 1000))
    except ValueError:
        return jsonify({'error': 'Invalid coordinates or radius'}), 400
    
    aggregator = simulation_state['traffic_aggregator']
    
    # If aggregator not initialized, return mock data
    if aggregator is None:
        return jsonify({
            'source': source,
            'location': {'lat': lat, 'lon': lon},
            'data': {
                'speed': 45.5,  # km/h
                'flow': 1200,   # vehicles/hour
                'density': 26,  # vehicles/km
                'timestamp': '2025-11-24T10:00:00Z'
            },
            'message': f'Mock data - traffic aggregator not initialized'
        })
    
    # If source is 'all', get from all sources
    if source == 'all':
        data = aggregator.get_all_traffic(lat, lon, radius)
        # Convert to JSON serializable format
        result = {}
        for src, items in data.items():
            result[src] = [vars(item) for item in items]
            # Handle datetime serialization
            for item in result[src]:
                if item['timestamp']:
                    item['timestamp'] = item['timestamp'].isoformat()
        return jsonify(result)
    
    # Specific source
    if source in aggregator.sources:
        try:
            items = aggregator.sources[source].get_traffic(lat, lon, radius)
            result = [vars(item) for item in items]
            for item in result:
                if item['timestamp']:
                    item['timestamp'] = item['timestamp'].isoformat()
            return jsonify({'source': source, 'data': result})
        except Exception as e:
            return jsonify({'error': str(e)}), 500
            
    # Fallback to mock data if source not configured or unknown
    return jsonify({
        'source': source,
        'location': {'lat': lat, 'lon': lon},
        'data': {
            'speed': 45.5,  # km/h
            'flow': 1200,   # vehicles/hour
            'density': 26,  # vehicles/km
            'timestamp': '2025-11-22T13:25:00Z'
        },
        'message': f'Mock data - source {source} not configured or unknown'
    })

def build_road_graph(network):
    """Build adjacency graph for roads."""
    graph = {}
    roads = network.roads
    
    print(f"Building graph for {len(roads)} roads...")
    
    # Simple O(N^2) build - can be optimized with spatial index
    for i, r1 in enumerate(roads):
        if i % 10 == 0:
            print(f"Processing road {i}/{len(roads)}")
        
        road_id = r1.get_id()
        graph[road_id] = []
        
        try:
            end_point = r1.get_end()
            
            for r2 in roads:
                if r1.get_id() == r2.get_id():
                    continue
                    
                try:
                    # Check if r2 starts where r1 ends
                    # Use a small tolerance (e.g., 5 meters for OSM data)
                    start_point = r2.get_start()
                    distance = end_point.distance_to(start_point)
                    
                    if distance < 5.0:
                        graph[road_id].append(r2)
                except Exception as e:
                    print(f"Error checking road connection: {e}")
                    continue
                    
        except Exception as e:
            print(f"Error processing road {road_id}: {e}")
            continue
                
    print(f"Graph built with {len(graph)} nodes")
    return graph


@app.route('/api/simulation/start', methods=['POST'])
def start_simulation():
    """Start simulation."""
    if not JAMFREE_AVAILABLE:
        return jsonify({'error': 'JamFree not available'}), 503
    
    if simulation_state['network'] is None:
        return jsonify({'error': 'No network loaded'}), 400
    
    simulation_state['running'] = True
    simulation_state['step'] = 0
    simulation_state['vehicles'] = []
    
    # Build road graph for routing
    print("Building road graph...")
    try:
        # Use new RoutingEngine
        simulation_state['routing_engine'] = RoutingEngine(simulation_state['network'].roads)
        print(f"Routing engine initialized with {len(simulation_state['network'].roads)} roads")
    except Exception as e:
        print(f"Error building road graph: {e}")
        import traceback
        traceback.print_exc()
        return jsonify({'error': f'Failed to build road graph: {str(e)}'}), 500
    
    # Initialize models (default for vehicles without specific settings)
    print("Initializing models...")
    
    # Get network reference
    network = simulation_state['network']
    
    # Use IDM Lookup if enabled (30-40% faster)
    use_lookup = simulation_state['config'].get('use_idm_lookup', True)
    if use_lookup and hasattr(jamfree, 'IDMLookup'):
        print("  Using IDM Lookup Tables (30-40% faster)")
        IDMClass = jamfree.IDMLookup
    else:
        print("  Using standard IDM")
        IDMClass = jamfree.IDM
    
    default_idm = IDMClass(
        desired_speed=simulation_state['config']['desired_speed'] / 3.6,
        time_headway=simulation_state['config']['time_headway']
    )
    mobil = jamfree.MOBIL(politeness=simulation_state['config']['politeness'])
    simulation_state['models'] = {'default_idm': default_idm, 'mobil': mobil}
    simulation_state['vehicle_models'] = {}  # Per-vehicle IDM models
    simulation_state['vehicle_routes'] = {}  # Per-vehicle routes
    simulation_state['vehicle_destinations'] = {} # Per-vehicle destinations
    
    # Initialize spatial index if enabled
    use_spatial = simulation_state['config'].get('use_spatial_index', True)
    if use_spatial and hasattr(jamfree, 'SpatialIndex'):
        print("  Using Spatial Index (O(log N) queries)")
        simulation_state['spatial_index'] = {}  # Per-lane spatial indices
    else:
        simulation_state['spatial_index'] = None # Indicate not in use
    
    # Initialize Adaptive Hybrid Simulator if enabled
    use_adaptive = simulation_state['config'].get('use_adaptive_hybrid', True)
    adaptive_threshold = simulation_state['config'].get('adaptive_threshold', 50)
    
    if use_adaptive and hasattr(jamfree, 'AdaptiveSimulator'):
        print("  Using Adaptive Hybrid Simulation (micro/macro switching)")
        
        # Create configuration
        adaptive_config = jamfree.AdaptiveSimulatorConfig()
        adaptive_config.micro_to_macro_density = 0.08  # 80 veh/km
        adaptive_config.macro_to_micro_density = 0.04  # 40 veh/km
        adaptive_config.micro_to_macro_count = adaptive_threshold
        adaptive_config.macro_to_micro_count = int(adaptive_threshold * 0.4)
        adaptive_config.macro_num_cells = 50
        adaptive_config.hysteresis_factor = 1.2
        adaptive_config.force_micro_intersections = True
        adaptive_config.force_micro_ramps = True
        
        # Create simulator
        simulation_state['adaptive_simulator'] = jamfree.AdaptiveSimulator(adaptive_config)
        
        # Register all lanes with specific modes based on geometry
        micro_count = 0
        macro_count = 0
        
        for road in network.roads:
            length = road.get_length()
            # Threshold for short lanes/crossings (e.g. 100m)
            is_short = length < 100.0 
            
            for lane_idx in range(road.get_num_lanes()):
                lane = road.get_lane(lane_idx)
                lane_id = lane.get_id()
                
                # Register lane (is_critical=True for short lanes)
                simulation_state['adaptive_simulator'].register_lane(lane, is_short)
                
                if is_short:
                    # Force microscopic for short lanes and crossings
                    simulation_state['adaptive_simulator'].force_microscopic(lane_id)
                    micro_count += 1
                else:
                    # Force macroscopic for long lanes
                    simulation_state['adaptive_simulator'].force_macroscopic(lane_id)
                    macro_count += 1
                    
        print(f"    Mode assignment: {micro_count} Micro (short/crossing), {macro_count} Macro (long)")
        
        print(f"    Registered {sum(r.get_num_lanes() for r in network.roads)} lanes")
        print(f"    Threshold: {adaptive_threshold} vehicles per lane")
    else:
        simulation_state['adaptive_simulator'] = None
        print("  Adaptive Hybrid Simulation disabled")
    
    # Initialize vehicles
    print("Initializing vehicles...")
    num_vehicles = int(simulation_state['config']['num_vehicles'])
    
    roads = network.roads
    if not roads:
        print("No roads in network")
        return jsonify({'error': 'No roads in network'}), 400
        
    import random
    
    count = 0
    count = 0
    routing_engine = simulation_state['routing_engine']
    
    for i in range(num_vehicles):
        # Assign random OD
        start_road_id, end_road_id = routing_engine.get_random_od()
        
        if not start_road_id:
            continue
            
        # Get path
        path = routing_engine.get_shortest_path(start_road_id, end_road_id)
        if not path:
            continue
            
        # Get start road
        road = routing_engine.roads[start_road_id]
        
        if road.get_num_lanes() > 0:
            lane_idx = random.randint(0, road.get_num_lanes() - 1)
            lane = road.get_lane(lane_idx)
            
            # Random position
            pos = random.uniform(0, min(50, road.get_length())) # Start near beginning
            
            # Create vehicle
            v_id = f"v_{i}"
            vehicle = jamfree.Vehicle(v_id)
            vehicle.set_current_lane(lane)
            vehicle.set_lane_position(pos)
            
            # Use lane speed limit as desired speed
            lane_speed_limit = lane.get_speed_limit()  # Already in m/s
            vehicle.set_speed(random.uniform(0, lane_speed_limit * 0.8))
            
            # Create per-vehicle IDM with lane speed limit (use lookup if enabled)
            vehicle_idm = IDMClass(
                desired_speed=lane_speed_limit,
                time_headway=simulation_state['config']['time_headway'],
                min_gap=2.0,
                max_accel=1.0,
                comfortable_decel=1.5,
                accel_exponent=4.0
            )
            simulation_state['vehicle_models'][v_id] = vehicle_idm
            
            # Store route
            simulation_state['vehicle_routes'][v_id] = {
                'path': path,
                'current_index': 0,
                'destination': end_road_id
            }
            
            # Add to lane
            lane.add_vehicle(vehicle)
            
            # Add to spatial index if enabled
            if simulation_state['spatial_index'] is not None:
                lane_id = lane.get_id()
                if lane_id not in simulation_state['spatial_index']:
                    if hasattr(jamfree, 'SpatialIndex'):
                        simulation_state['spatial_index'][lane_id] = jamfree.SpatialIndex()
                if lane_id in simulation_state['spatial_index']:
                    simulation_state['spatial_index'][lane_id].add_vehicle(vehicle)
            
            simulation_state['vehicles'].append(vehicle)
            count += 1
    
    print(f"Initialized {count} vehicles")
    return jsonify({'status': 'started', 'vehicle_count': len(simulation_state['vehicles'])})

@app.route('/api/simulation/stop', methods=['POST'])
def stop_simulation():
    """Stop simulation."""
    simulation_state['running'] = False
    return jsonify({'status': 'stopped'})

@app.route('/api/simulation/step', methods=['POST'])
def simulation_step():
    """Execute one simulation step."""
    if not simulation_state['running']:
        return jsonify({'error': 'Simulation not running'}), 400
    
    import time
    step_start = time.time()
    
    dt = 0.1  # 100ms time step
    vehicles_data = []
    
    default_idm = simulation_state['models']['default_idm']
    vehicle_models = simulation_state.get('vehicle_models', {})
    vehicle_routes = simulation_state.get('vehicle_routes', {})
    routing_engine = simulation_state.get('routing_engine')
    spatial_indices = simulation_state.get('spatial_index', {})
    
    # Pre-compute network center for coordinate conversion
    network = simulation_state['network']
    center_lat = (network.min_lat + network.max_lat) / 2.0
    center_lon = (network.min_lon + network.max_lon) / 2.0
    
    # Update spatial indices if enabled
    if spatial_indices:
        for lane_id, index in spatial_indices.items():
            index.update()
            
    # Handle mandatory lane changes for routing (pre-update)
    if routing_engine:
        for vehicle in simulation_state['vehicles']:
            v_id = vehicle.get_id()
            route_info = vehicle_routes.get(v_id)
            
            if route_info:
                path = route_info['path']
                current_idx = route_info['current_index']
                
                # Check if approaching end of road
                lane = vehicle.get_current_lane()
                if lane:
                    dist_to_end = lane.get_length() - vehicle.get_lane_position()
                    
                    # Lookahead distance (e.g., 100m)
                    if dist_to_end < 100.0 and current_idx + 1 < len(path):
                        current_road_id = path[current_idx]
                        next_road_id = path[current_idx + 1]
                        
                        target_lane_idx = routing_engine.get_target_lane_index(
                            current_road_id, next_road_id, lane.get_index()
                        )
                        
                        if target_lane_idx != lane.get_index():
                            # Need to change lane
                            # For now, force switch if close to ensures routing works
                            # In a real simulation, we would use MOBIL with a mandatory term
                            
                            if dist_to_end < 60.0:
                                current_road = routing_engine.roads.get(current_road_id)
                                if current_road:
                                    target_lane = current_road.get_lane(target_lane_idx)
                                    if target_lane:
                                        lane.remove_vehicle(vehicle)
                                        target_lane.add_vehicle(vehicle)
                                        vehicle.set_current_lane(target_lane)
                                        # Keep same lane position
    
    # Check if using Adaptive Hybrid Simulator
    adaptive_sim = simulation_state.get('adaptive_simulator')
    num_vehicles = len(simulation_state['vehicles']) # Define here for both paths
    
    # Check if using Metal GPU
    use_gpu = simulation_state['config'].get('use_gpu', False)
    metal_compute = simulation_state.get('metal_compute')
    
    # Initialize Metal if enabled but not ready
    if use_gpu and not metal_compute and hasattr(jamfree, 'MetalCompute') and jamfree.is_metal_available():
        try:
            print("Initializing Metal GPU...")
            metal_compute = jamfree.MetalCompute()
            if metal_compute.initialize(""):
                simulation_state['metal_compute'] = metal_compute
                print("Metal GPU initialized successfully")
            else:
                print("Failed to initialize Metal GPU")
                simulation_state['config']['use_gpu'] = False
        except Exception as e:
            print(f"Error initializing Metal GPU: {e}")
            simulation_state['config']['use_gpu'] = False

    if use_gpu and metal_compute:
        # GPU Accelerated Step
        try:
            # 1. Upload vehicles to GPU
            metal_compute.upload_vehicles(simulation_state['vehicles'])
            
            # 2. Set IDM parameters (global for now)
            metal_compute.set_idm_params(
                simulation_state['config']['desired_speed'] / 3.6,
                simulation_state['config']['time_headway'],
                2.0, 1.0, 1.5, 4.0
            )
            
            # 3. Run simulation step on GPU
            metal_compute.simulation_step(num_vehicles, dt)
            
            # 4. Download results back to CPU
            metal_compute.download_vehicles(simulation_state['vehicles'])
            
            # 5. Update spatial indices since positions changed
            if spatial_indices:
                for lane_id, index in spatial_indices.items():
                    index.update()
                    
            # Collect data for visualization
            for vehicle in simulation_state['vehicles']:
                try:
                    pos = vehicle.get_position()
                    geo_pos = jamfree.OSMParser.meters_to_lat_lon(pos.x, pos.y, center_lat, center_lon)
                    
                    vehicles_data.append({
                        'id': vehicle.get_id(),
                        'x': pos.x, 'y': pos.y,
                        'lat': geo_pos.x, 'lon': geo_pos.y,
                        'angle': vehicle.get_heading(),
                        'speed': vehicle.get_speed() * 3.6
                    })
                except Exception:
                    pass
                    
        except Exception as e:
            print(f"GPU Simulation Error: {e}")
            simulation_state['config']['use_gpu'] = False

    elif adaptive_sim:
        # Use Adaptive Hybrid Simulator (handles micro/macro switching automatically)
        adaptive_sim.update(dt, default_idm)
        
        # Get statistics
        stats = adaptive_sim.get_statistics()
        simulation_state['performance_stats']['micro_lanes'] = stats.micro_lanes
        simulation_state['performance_stats']['macro_lanes'] = stats.macro_lanes
        simulation_state['performance_stats']['speedup_factor'] = stats.speedup_factor
        
        # Collect vehicle data for visualization
        for vehicle in simulation_state['vehicles']:
            try:
                pos = vehicle.get_position()
                geo_pos = jamfree.OSMParser.meters_to_lat_lon(pos.x, pos.y, center_lat, center_lon)
                
                vehicles_data.append({
                    'id': vehicle.get_id(),
                    'x': pos.x,
                    'y': pos.y,
                    'lat': geo_pos.x,
                    'lon': geo_pos.y,
                    'angle': vehicle.get_heading(),
                    'speed': vehicle.get_speed() * 3.6  # km/h
                })
            except Exception as e:
                print(f"Error getting vehicle data: {e}")
    
    else:
        # Original per-vehicle update logic
        # Use multithreading for large simulations
        import concurrent.futures
        import threading
        
        def update_vehicle(vehicle):
            """Update a single vehicle (thread-safe)."""
            try:
                lane = vehicle.get_current_lane()
                if not lane:
                    return None
                
                # Get leader using spatial index if available
                if spatial_indices and lane.get_id() in spatial_indices:
                    leader = spatial_indices[lane.get_id()].find_leader(vehicle)
                else:
                    leader = lane.get_leader(vehicle)
                
                # Get vehicle-specific IDM or use default
                v_id = vehicle.get_id()
                idm = vehicle_models.get(v_id, default_idm)
                
                # Calculate acceleration
                acc = idm.calculate_acceleration(vehicle, leader)
                
                # Update vehicle state
                vehicle.update(dt, acc)
                
                # Get current position for visualization
                pos = vehicle.get_position()
                
                # Convert to lat/lon
                geo_pos = jamfree.OSMParser.meters_to_lat_lon(pos.x, pos.y, center_lat, center_lon)
                
                return {
                    'id': v_id,
                    'x': pos.x,
                    'y': pos.y,
                    'lat': geo_pos.x,
                    'lon': geo_pos.y,
                    'angle': vehicle.get_heading(),
                    'speed': vehicle.get_speed() * 3.6  # km/h
                }
            except Exception as e:
                print(f"Error updating vehicle: {e}")
                return None
        
        # Update vehicles in parallel for large simulations
        use_threading = simulation_state['config'].get('use_multithreading', True)
        
        if use_threading and num_vehicles > 100:
            # Use thread pool for large simulations
            with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
                results = list(executor.map(update_vehicle, simulation_state['vehicles']))
                vehicles_data = [r for r in results if r is not None]
        else:
            # Sequential for small simulations
            for vehicle in simulation_state['vehicles']:
                result = update_vehicle(vehicle)
                if result:
                    vehicles_data.append(result)
    
    # Handle road transitions (must be done sequentially to avoid race conditions)
    # Handle road transitions and routing
    for vehicle in simulation_state['vehicles']:
        lane = vehicle.get_current_lane()
        if lane and vehicle.get_lane_position() > lane.get_length():
            v_id = vehicle.get_id()
            route_info = vehicle_routes.get(v_id)
            
            if route_info and routing_engine:
                # Get next road in path
                path = route_info['path']
                current_idx = route_info['current_index']
                
                if current_idx + 1 < len(path):
                    next_road_id = path[current_idx + 1]
                    next_road = routing_engine.roads.get(next_road_id)
                    
                    if next_road:
                        # Move to next road
                        # Pick a lane (try to keep same index, or closest valid)
                        next_lane_idx = min(lane.get_index(), next_road.get_num_lanes() - 1)
                        next_lane = next_road.get_lane(next_lane_idx)
                        
                        # Move vehicle
                        lane.remove_vehicle(vehicle)
                        next_lane.add_vehicle(vehicle)
                        vehicle.set_current_lane(next_lane)
                        vehicle.set_lane_position(0)
                        
                        # Update route index
                        route_info['current_index'] += 1
                        
                        # Update IDM for new lane speed limit
                        lane_speed_limit = next_lane.get_speed_limit()
                        
                        # Update existing IDM model if possible, or create new one
                        if v_id in vehicle_models:
                            vehicle_models[v_id].set_desired_speed(lane_speed_limit)
                        
                    else:
                        # Road not found (shouldn't happen)
                        vehicle.set_lane_position(lane.get_length())
                        vehicle.set_speed(0)
                else:
                    # Destination reached!
                    # Respawn at a new random location with new route
                    start_road_id, end_road_id = routing_engine.get_random_od()
                    if start_road_id:
                        new_path = routing_engine.get_shortest_path(start_road_id, end_road_id)
                        if new_path:
                            # Remove from old lane
                            lane.remove_vehicle(vehicle)
                            
                            # Add to new start lane
                            new_start_road = routing_engine.roads[start_road_id]
                            new_lane = new_start_road.get_lane(0)
                            new_lane.add_vehicle(vehicle)
                            vehicle.set_current_lane(new_lane)
                            vehicle.set_lane_position(0)
                            vehicle.set_speed(new_lane.get_speed_limit() * 0.5)
                            
                            # Update route info
                            vehicle_routes[v_id] = {
                                'path': new_path,
                                'current_index': 0,
                                'destination': end_road_id
                            }
                            
                            # Update speed limit
                            if v_id in vehicle_models:
                                vehicle_models[v_id].set_desired_speed(new_lane.get_speed_limit())
            else:
                # No route info - just stop at end
                vehicle.set_lane_position(lane.get_length())
                vehicle.set_speed(0)
    
    simulation_state['step'] += 1
    
    # Track performance
    step_time = (time.time() - step_start) * 1000  # ms
    simulation_state['performance_stats']['avg_update_time_ms'] = step_time
    
    # Calculate speedup estimate (use adaptive speedup if available)
    adaptive_sim = simulation_state.get('adaptive_simulator')
    if adaptive_sim:
        speedup = simulation_state['performance_stats'].get('speedup_factor', 1.0)
    else:
        # Baseline: 1ms per vehicle without optimizations
        baseline_time = num_vehicles * 1.0
        speedup = baseline_time / max(step_time, 0.1)
        simulation_state['performance_stats']['speedup_factor'] = speedup
    
    # Collect lane mode information if using adaptive simulator
    lane_modes = {}
    if adaptive_sim:
        for road in network.roads:
            for lane_idx in range(road.get_num_lanes()):
                lane = road.get_lane(lane_idx)
                lane_id = lane.get_id()
                mode = adaptive_sim.get_mode(lane_id)
                # Convert enum to string
                mode_str = str(mode).split('.')[-1] if mode else 'UNKNOWN'
                lane_modes[lane_id] = mode_str
    
    return jsonify({
        'step': simulation_state['step'],
        'vehicles': vehicles_data,
        'performance': {
            'update_time_ms': round(step_time, 2),
            'vehicles_count': num_vehicles,
            'speedup_factor': round(speedup, 2),
            'micro_lanes': simulation_state['performance_stats'].get('micro_lanes', 0),
            'macro_lanes': simulation_state['performance_stats'].get('macro_lanes', 0),
            'optimizations': {
                'idm_lookup': simulation_state['config'].get('use_idm_lookup', False),
                'spatial_index': bool(spatial_indices),
                'multithreading': simulation_state['config'].get('use_multithreading', False) and num_vehicles > 100,
                'adaptive_hybrid': adaptive_sim is not None,
                'gpu_metal': use_gpu and metal_compute is not None,
            }
        },
        'lane_modes': lane_modes if lane_modes else None
    })

@app.route('/api/simulation/sync_traffic', methods=['POST'])
def sync_traffic():
    """Sync simulation with real-time traffic data."""
    if not simulation_state['running']:
        return jsonify({'error': 'Simulation not running'}), 400
        
    aggregator = simulation_state.get('traffic_aggregator')
    if not aggregator:
        return jsonify({'error': 'Traffic aggregator not configured'}), 400
        
    network = simulation_state['network']
    routing_engine = simulation_state.get('routing_engine')
    
    if not routing_engine:
        return jsonify({'error': 'Routing engine not ready'}), 400
        
    # Get center for projection
    center_lat = (network.min_lat + network.max_lat) / 2.0
    center_lon = (network.min_lon + network.max_lon) / 2.0
    
    # Fetch data
    # Use network bounds + margin
    radius = 5000 # 5km default
    traffic_data = aggregator.get_all_traffic(center_lat, center_lon, radius)
    
    updated_count = 0
    
    for source, items in traffic_data.items():
        for item in items:
            if item.location and item.speed > 0:
                lat, lon = item.location
                
                # Convert to meters
                p = jamfree.OSMParser.lat_lon_to_meters(lat, lon, center_lat, center_lon)
                
                # Find nearest road
                road = routing_engine.find_nearest_road(p.x, p.y, max_distance=50.0)
                
                if road:
                    # Update speed limit for all lanes
                    # Convert km/h to m/s
                    speed_ms = item.speed / 3.6
                    
                    for i in range(road.get_num_lanes()):
                        lane = road.get_lane(i)
                        lane.set_speed_limit(speed_ms)
                    updated_count += 1
                    
    return jsonify({
        'status': 'synced',
        'updated_roads': updated_count
    })

@app.route('/api/data/sources')
def list_data_sources():
    """List available real-time traffic data sources."""
    sources = [
        {
            'id': 'tomtom',
            'name': 'TomTom Traffic API',
            'type': 'commercial',
            'coverage': 'global',
            'features': ['speed', 'flow', 'incidents'],
            'url': 'https://developer.tomtom.com/traffic-api'
        },
        {
            'id': 'here',
            'name': 'HERE Traffic API',
            'type': 'commercial',
            'coverage': 'global',
            'features': ['speed', 'flow', 'incidents'],
            'url': 'https://developer.here.com/products/traffic'
        },
        {
            'id': 'google',
            'name': 'Google Maps Traffic',
            'type': 'commercial',
            'coverage': 'global',
            'features': ['speed', 'congestion'],
            'url': 'https://developers.google.com/maps/documentation/roads'
        },
        {
            'id': 'opentraffic',
            'name': 'OpenTraffic',
            'type': 'open',
            'coverage': 'limited',
            'features': ['speed', 'flow'],
            'url': 'http://opentraffic.io/'
        },
        {
            'id': 'datex2',
            'name': 'DATEX II (Europe)',
            'type': 'government',
            'coverage': 'europe',
            'features': ['incidents', 'roadworks', 'conditions'],
            'url': 'https://www.datex2.eu/'
        },
        {
            'id': 'pems',
            'name': 'PeMS (California)',
            'type': 'government',
            'coverage': 'california',
            'features': ['speed', 'flow', 'occupancy'],
            'url': 'http://pems.dot.ca.gov/'
        }
    ]
    
    return jsonify({'sources': sources})

if __name__ == '__main__':
    print("=" * 60)
    print("JamFree Web UI")
    print("=" * 60)
    print()
    print(f"JamFree available: {JAMFREE_AVAILABLE}")
    print()
    print("Starting server on http://localhost:5001")
    print()
    
    app.run(debug=False, host='0.0.0.0', port=5001, threaded=True)
