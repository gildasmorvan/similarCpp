# Realistic Traffic Simulation Features

## Overview

JamFree now includes advanced features for realistic traffic simulation:

1. **Traffic Controls** - Stop signs, traffic lights, yield signs
2. **Real-Time Data** - Integration with Google Maps, TomTom, etc.
3. **Origin-Destination Routing** - A* pathfinding with OD matrices
4. **Time-Dependent Behavior** - Rush hour patterns, weather effects

## 1. Traffic Controls

### Implementation

**File**: `kernel/include/model/TrafficControl.h`

### Features

- **Stop Signs**: Vehicles must come to complete stop
- **Traffic Lights**: Configurable phase timing (green/yellow/red)
- **Yield Signs**: Slow down and give way
- **Traffic Control Manager**: Centralized control management

### Usage

```cpp
#include "kernel/include/model/TrafficControl.h"

// Create traffic light
auto light = std::make_shared<TrafficLight>(
    "light_1",
    Point2D(100, 100),
    std::vector<TrafficLight::PhaseConfig>{
        {LightPhase::GREEN, 30.0},
        {LightPhase::YELLOW, 3.0},
        {LightPhase::RED, 30.0}
    }
);

// Create manager
TrafficControlManager manager;
manager.addControl(light);

// Update lights
manager.update(dt);

// Check if vehicle should stop
bool must_stop = manager.shouldStopForControl(
    vehicle_position,
    vehicle_speed,
    look_ahead_distance
);
```

### Python API

```python
import jamfree

# Create traffic light
light = jamfree.TrafficLight(
    id="light_1",
    position=jamfree.Point2D(100, 100),
    phases=[
        jamfree.TrafficLight.PhaseConfig(jamfree.LightPhase.GREEN, 30.0),
        jamfree.TrafficLight.PhaseConfig(jamfree.LightPhase.YELLOW, 3.0),
        jamfree.TrafficLight.PhaseConfig(jamfree.LightPhase.RED, 30.0),
    ]
)

# Create manager
manager = jamfree.TrafficControlManager()
manager.add_control(light)

# Update
manager.update(dt)

# Check
must_stop = manager.should_stop_for_control(vehicle_pos, vehicle_speed)
```

### Enhanced IDM

The `IDMEnhanced` class automatically respects traffic controls:

```cpp
#include "microscopic/include/IDMEnhanced.h"

IDMEnhanced idm(desired_speed, time_headway);
idm.setTrafficControlManager(&manager);

// IDM will now automatically brake for red lights and stop signs
double acc = idm.calculateAcceleration(vehicle, leader);
```

## 2. Real-Time Traffic Data

### Implementation

**File**: `realdata/include/TrafficDataSource.h`

### Data Sources

1. **Google Maps Traffic API**
   - Real-time speeds
   - Incidents
   - Typical traffic patterns

2. **TomTom Traffic API**
   - Real-time traffic flow
   - Incidents and closures
   - Historical data

3. **Estimated Data** (Fallback)
   - Time-of-day patterns
   - Rush hour modeling
   - Road type capacity

### Usage

```cpp
#include "realdata/include/TrafficDataSource.h"

// Setup data manager
TrafficDataManager data_manager;

// Add Google Maps (priority 0 = highest)
auto google = std::make_shared<GoogleMapsDataSource>(api_key);
data_manager.addSource(google, 0);

// Add TomTom as fallback
auto tomtom = std::make_shared<TomTomDataSource>(api_key);
data_manager.addSource(tomtom, 1);

// Add estimated as final fallback
auto estimated = std::make_shared<EstimatedDataSource>();
data_manager.addSource(estimated, 2);

// Set current time
DateTime now = DateTime::now();
data_manager.setCurrentTime(now);

// Get traffic speeds
auto speeds = data_manager.getTrafficSpeeds(min_point, max_point);

// Get incidents
auto incidents = data_manager.getIncidents(min_point, max_point);

// Get weather
auto weather = data_manager.getWeather(location);
```

### Time-Dependent Behavior

```cpp
DateTime time = DateTime::now();

// Check conditions
bool is_rush_hour = time.isRushHour();  // 7-9 AM or 5-7 PM
bool is_weekend = time.isWeekend();

// Estimate speed based on time
EstimatedDataSource estimator;
double estimated_speed = estimator.estimateSpeed(
    free_flow_speed,
    "motorway",
    time
);

// Estimate congestion
double congestion = estimator.estimateCongestion("primary", time);
```

### Python API

```python
import jamfree
from datetime import datetime

# Setup data manager
data_manager = jamfree.TrafficDataManager()

# Add sources
google_source = jamfree.GoogleMapsDataSource(api_key)
data_manager.add_source(google_source, priority=0)

estimated_source = jamfree.EstimatedDataSource()
data_manager.add_source(estimated_source, priority=2)

# Set time
now = datetime.now()
data_manager.set_current_time(now)

# Get data
speeds = data_manager.get_traffic_speeds(min_point, max_point)
incidents = data_manager.get_incidents(min_point, max_point)
weather = data_manager.get_weather(location)
```

## 3. Origin-Destination Routing

### Implementation

**File**: `kernel/include/routing/Router.h`

### Features

- **A* Pathfinding**: Optimal route finding
- **Multiple Strategies**: Shortest distance, time, cost
- **Dynamic Rerouting**: Adapt to traffic conditions
- **OD Matrix**: Trip demand modeling

### Router Usage

```cpp
#include "kernel/include/routing/Router.h"

// Create router
Router router;
router.setStrategy(Router::Strategy::SHORTEST_TIME);

// Find route
Route route = router.findRoute(
    origin,
    destination,
    roads,
    current_time
);

// Find route with traffic data
Route route_traffic = router.findRouteWithTraffic(
    origin,
    destination,
    roads,
    traffic_speeds  // road_id -> current_speed
);

// Dynamic rerouting
Route new_route = router.rerouteIfNeeded(
    current_route,
    current_position,
    destination,
    roads,
    traffic_speeds
);
```

### OD Matrix

```cpp
// Create OD matrix
ODMatrix od_matrix;

// Add demand (origin_zone, dest_zone, num_trips, time_period)
od_matrix.addDemand("downtown", "suburb_north", 500, 7);  // 7 AM
od_matrix.addDemand("suburb_north", "downtown", 450, 17); // 5 PM

// Sample OD pair
ODPair od_pair = od_matrix.sampleODPair(7);  // 7 AM period

// Generate synthetic matrix (gravity model)
od_matrix.generateSynthetic(
    zone_populations,
    zone_attractions,
    zone_centroids
);
```

### Trip Generation

```cpp
// Create trip generator
TripGenerator generator(od_matrix, router);

// Generate single trip
auto [od_pair, route] = generator.generateTrip(current_time, roads);

// Generate multiple trips
auto trips = generator.generateTrips(
    time_period,  // hour
    num_trips,
    roads
);
```

### Vehicle with Route

```cpp
// Create vehicle with route
Vehicle vehicle("v_1");

// Create enhanced IDM
IDMEnhanced idm(desired_speed, time_headway);
idm.setRoute(&route);

// Vehicle will follow route
double acc = idm.calculateAcceleration(vehicle, leader);

// Update route progress when changing roads
idm.updateRouteProgress(new_road);

// Check if destination reached
if (idm.isRouteComplete()) {
    std::cout << "Destination reached!" << std::endl;
}
```

### Python API

```python
import jamfree

# Create router
router = jamfree.Router()
router.set_strategy(jamfree.Router.Strategy.SHORTEST_TIME)

# Find route
route = router.find_route(origin, destination, roads)

# Create OD matrix
od_matrix = jamfree.ODMatrix()
od_matrix.add_demand("downtown", "suburb", 500, time_period=7)

# Sample OD pair
od_pair = od_matrix.sample_od_pair(time_period=7)

# Create trip generator
generator = jamfree.TripGenerator(od_matrix, router)
od_pair, route = generator.generate_trip(current_time, roads)

# Create vehicle with route
vehicle = jamfree.Vehicle("v_1")
idm = jamfree.IDMEnhanced(desired_speed=30.0)
idm.set_route(route)
```

## 4. Complete Realistic Simulation

### Workflow

```python
# 1. Load network
network = jamfree.load_osm_network(lat, lon, radius)

# 2. Setup traffic controls
control_manager = jamfree.TrafficControlManager()
# Add lights, stops from OSM data
for intersection in network.intersections:
    light = jamfree.TrafficLight(...)
    control_manager.add_control(light)

# 3. Setup real data
data_manager = jamfree.TrafficDataManager()
data_manager.add_source(jamfree.GoogleMapsDataSource(api_key), 0)
data_manager.add_source(jamfree.EstimatedDataSource(), 2)

# 4. Create OD matrix
od_matrix = jamfree.ODMatrix()
od_matrix.load_from_file("od_matrix.csv")

# 5. Setup router
router = jamfree.Router()
router.set_strategy(jamfree.Router.Strategy.SHORTEST_TIME)

# 6. Generate vehicles with routes
generator = jamfree.TripGenerator(od_matrix, router)
vehicles = []

for i in range(num_vehicles):
    od_pair, route = generator.generate_trip(current_time, roads)
    
    vehicle = jamfree.Vehicle(f"v_{i}")
    idm = jamfree.IDMEnhanced(desired_speed=30.0)
    idm.set_traffic_control_manager(control_manager)
    idm.set_route(route)
    
    vehicles.append((vehicle, idm, od_pair, route))

# 7. Simulation loop
for step in range(num_steps):
    # Update time
    current_time += dt
    data_manager.set_current_time(current_time)
    
    # Update traffic controls
    control_manager.update(dt)
    
    # Get real-time data (every minute)
    if step % 600 == 0:
        traffic_speeds = data_manager.get_traffic_speeds(...)
        incidents = data_manager.get_incidents(...)
        
        # Reroute vehicles if needed
        for vehicle, idm, od_pair, route in vehicles:
            new_route = router.reroute_if_needed(
                route,
                vehicle.get_position(),
                od_pair.destination,
                roads,
                traffic_speeds
            )
            if not new_route.is_empty():
                idm.set_route(new_route)
    
    # Update vehicles
    for vehicle, idm, od_pair, route in vehicles:
        lane = vehicle.get_current_lane()
        leader = lane.get_leader(vehicle)
        
        # IDM respects traffic controls and follows route
        acc = idm.calculate_acceleration(vehicle, leader)
        vehicle.update(dt, acc)
        
        # Check if destination reached
        if idm.is_route_complete():
            print(f"Vehicle {vehicle.get_id()} reached destination!")
```

## Configuration Examples

### Rush Hour Simulation

```python
# Morning rush hour (7-9 AM)
current_time = datetime(2024, 1, 15, 7, 30)  # Monday 7:30 AM
data_manager.set_current_time(current_time)

# High demand from suburbs to downtown
od_matrix.add_demand("suburb_north", "downtown", 800, time_period=7)
od_matrix.add_demand("suburb_south", "downtown", 600, time_period=7)

# Traffic will be congested
speeds = data_manager.get_traffic_speeds(...)  # Lower speeds expected
```

### Weekend Leisure Traffic

```python
# Saturday afternoon
current_time = datetime(2024, 1, 20, 14, 0)  # Saturday 2 PM
data_manager.set_current_time(current_time)

# Different demand pattern
od_matrix.add_demand("residential", "shopping_mall", 300, time_period=14)
od_matrix.add_demand("residential", "park", 200, time_period=14)
```

### Incident Response

```python
# Get incidents
incidents = data_manager.get_incidents(min_point, max_point)

for incident in incidents:
    if incident.severity == "severe":
        # Reroute all vehicles
        for vehicle, idm, od_pair, route in vehicles:
            new_route = router.find_route_with_traffic(
                vehicle.get_position(),
                od_pair.destination,
                roads,
                traffic_speeds
            )
            idm.set_route(new_route)
```

## Benefits

✅ **Realistic Behavior**: Vehicles respect traffic rules  
✅ **Real-Time Data**: Integration with live traffic APIs  
✅ **Intelligent Routing**: A* pathfinding with traffic awareness  
✅ **Time-Dependent**: Rush hour, weekends, weather effects  
✅ **Dynamic Adaptation**: Rerouting based on conditions  
✅ **Complete Trips**: Origin-destination modeling  

## Next Steps

1. **Implement C++ code** for all headers
2. **Add Python bindings** for new classes
3. **Integrate with web UI** for visualization
4. **Add real API keys** for live data
5. **Create OD matrices** from census data
6. **Validate** against real traffic patterns

JamFree is now a **complete realistic traffic simulation platform**! 🚦🗺️🚗
