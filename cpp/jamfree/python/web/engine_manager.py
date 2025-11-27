"""
SimulationEngineManager - Wrapper for SIMILAR MultithreadedSimulationEngine.

This class manages the lifecycle of the C++ simulation engine and provides
a Python-friendly interface for the web UI.
"""

import jamfree
from typing import List, Dict, Any, Optional


class VisualizationProbe:
    """Probe to extract vehicle states for visualization."""
    
    def __init__(self, center_lat: float, center_lon: float):
        """
        Initialize visualization probe.
        
        Args:
            center_lat: Center latitude for coordinate conversion
            center_lon: Center longitude for coordinate conversion
        """
        self.center_lat = center_lat
        self.center_lon = center_lon
        self.vehicle_data = []
        self.step = 0
        
    def observe(self, simulation_time, dynamic_state):
        """
        Extract vehicle states from simulation.
        
        Args:
            simulation_time: Current simulation time
            dynamic_state: Dynamic state from engine
        """
        # For now, store step number
        # Full state extraction will be implemented when dynamic_state API is clear
        self.step = simulation_time.get_identifier() if hasattr(simulation_time, 'get_identifier') else 0
        
        # Placeholder: In future, extract vehicle states from dynamic_state
        # and convert positions to lat/lon for visualization
        pass
    
    def get_data(self) -> List[Dict[str, Any]]:
        """Get extracted vehicle data."""
        return self.vehicle_data
    
    def clear(self):
        """Clear stored data."""
        self.vehicle_data = []


class SimulationEngineManager:
    """Manages the SIMILAR MultithreadedSimulationEngine for traffic simulation."""
    
    def __init__(self, network, config: Dict[str, Any]):
        """
        Initialize engine manager.
        
        Args:
            network: OSM road network (jamfree.RoadNetwork)
            config: Simulation configuration dictionary
        """
        self.network = network
        self.config = config
        self.engine = None
        self.model = None
        self.vehicles = []
        self.current_time = jamfree.SimulationTimeStamp(0)
        self.final_time = jamfree.SimulationTimeStamp(10000)  # Default: 1000 seconds at 0.1s steps
        self.probe = None
        
        # Calculate network center for coordinate conversion
        self.center_lat = (network.min_lat + network.max_lat) / 2.0
        self.center_lon = (network.min_lon + network.max_lon) / 2.0
        
    def create_vehicle_agent(self, vehicle_id: str, initial_lane, config: Dict[str, Any]):
        """
        Create a VehicleAgent with public and private states.
        
        Note: Perception and decision models are not yet exposed in Python bindings,
        so this creates a minimal agent. The C++ engine will use default models.
        
        Args:
            vehicle_id: Unique vehicle identifier
            initial_lane: Starting lane for the vehicle (jamfree.Lane)
            config: Vehicle configuration (IDM/MOBIL parameters)
            
        Returns:
            Configured VehicleAgent
        """
        # 1. Create Perception Model
        perception_range = config.get('perception_range', 200.0)
        perception_model = jamfree.VehiclePerceptionModelMicro(perception_range)
        
        # 2. Create Decision Models (IDM & MOBIL)
        # IDM constructor: (desired_speed, time_headway, min_gap, max_accel, comfortable_decel, accel_exponent)
        idm = jamfree.IDM(
            config.get('desired_speed', 33.3),
            config.get('time_headway', 1.5),
            config.get('min_gap', 2.0),
            config.get('max_accel', 3.0),
            config.get('comfort_decel', 2.0),
            config.get('accel_exponent', 4.0)
        )
        
        # MOBIL constructor: (politeness, threshold, max_safe_decel, bias_right)
        mobil = jamfree.MOBIL(
            config.get('politeness', 0.5),
            config.get('lc_threshold', 0.1),
            config.get('max_safe_decel', 4.0),
            config.get('bias_right', 0.1)
        )
        
        # 3. Create Decision Micro Submodels (DMS)
        accel_dms = jamfree.ForwardAccelerationDMS(idm)
        lane_change_dms = jamfree.LaneChangeDMS(mobil, idm)
        
        # 4. Create Composite DMS (Conjunction)
        conjunction_dms = jamfree.ConjunctionDMS()
        conjunction_dms.add_submodel(accel_dms)
        conjunction_dms.add_submodel(lane_change_dms)
        
        # 5. Create Vehicle Decision Model
        decision_model = jamfree.VehicleDecisionModelMicro(conjunction_dms)
        
        # 6. Create Vehicle Agent
        agent = jamfree.VehicleAgent(vehicle_id)
        
        # 7. Create States
        public_state = jamfree.VehiclePublicLocalStateMicro(vehicle_id)
        public_state.set_speed(config.get('initial_speed', 30.0))
        public_state.set_lane_position(config.get('initial_position', 0.0))
        public_state.set_lane_index(initial_lane.get_index())
        public_state.set_active(True)
        public_state.set_length(config.get('length', 5.0))
        
        private_state = jamfree.VehiclePrivateLocalStateMicro(vehicle_id)
        # Only set parameters that have setters available in the bindings
        private_state.set_desired_speed(config.get('desired_speed', 33.3))
        private_state.set_time_headway(config.get('time_headway', 1.5))
        private_state.set_max_acceleration(config.get('max_accel', 3.0))
        private_state.set_comfortable_deceleration(config.get('comfort_decel', 2.0))
        private_state.set_politeness(config.get('politeness', 0.5))
        
        # 8. Set Models and States
        # "Microscopic" is the level identifier
        agent.set_models("Microscopic", perception_model, decision_model)
        agent.set_states("Microscopic", public_state, private_state)
        
        return agent
        
    def initialize(self) -> bool:
        """
        Initialize the simulation engine.
        
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            # Create SimulationEngine (JamFree Kernel)
            # We use the sequential engine for now to support step-by-step execution
            self.engine = jamfree.SimulationEngine(0.1)  # dt = 0.1s
            
            # Create Microscopic Reaction Model
            reaction_model = jamfree.MicroscopicReactionModel(0.1)
            reaction_model.set_simulation_engine(self.engine)
            
            # Set reaction model for Microscopic level
            self.engine.set_reaction_model("Microscopic", reaction_model)
            
            # Create vehicles
            # We need to know the initial lane for each vehicle
            # For this demo, we'll put them on the first lane of the first road
            if not self.network or not self.network.roads:
                print("Error: No road network loaded")
                return False
                
            first_road = self.network.roads[0]
            if first_road.get_num_lanes() == 0:
                print("Error: First road has no lanes")
                return False
                
            first_lane = first_road.get_lane(0)
            
            # Create some test vehicles
            num_vehicles = self.config.get('num_vehicles', 10)
            for i in range(num_vehicles):
                vehicle_id = f"veh_{i}"
                vehicle_config = self.config.copy()
                vehicle_config['initial_position'] = i * 20.0  # Space them out
                
                agent = self.create_vehicle_agent(vehicle_id, first_lane, vehicle_config)
                self.engine.add_agent(agent)
                self.vehicles.append(agent)
            
            # Create visualization probe
            self.probe = VisualizationProbe(self.center_lat, self.center_lon)
            
            print(f"Engine initialized with {len(self.vehicles)} vehicles")
            
            return True
            
        except Exception as e:
            print(f"Error initializing engine: {e}")
            import traceback
            traceback.print_exc()
            return False
    
    def step(self) -> Dict[str, Any]:
        """
        Execute one simulation step.
        
        Returns:
            Dictionary with visualization data:
            {
                'step': int,
                'vehicles': [{'id', 'lat', 'lon', 'speed', 'angle', 'lane_id'}, ...],
                'performance': {'update_time_ms': float}
            }
        """
        if self.engine is None:
            return {'error': 'Engine not initialized'}
        
        import time
        start_time = time.time()
        
        try:
            # Advance simulation by one time step
            self.engine.step()
            
            # Update current time
            self.current_time = jamfree.SimulationTimeStamp(
                int(self.engine.get_current_time() * 10) # Assuming 0.1s step
            )
            
            # Extract vehicle data
            vehicles_data = []
            for agent in self.vehicles:
                # Get public state for Microscopic level
                # We need to cast to VehiclePublicLocalStateMicro to access specific methods
                # But bindings should handle polymorphism if configured correctly
                # For now, we assume the object returned has the methods we need
                
                # Note: get_public_local_state returns ILocalState
                # We might need to cast it in C++ or use a helper
                # But let's try accessing methods directly (duck typing in Python)
                state = agent.get_public_local_state("Microscopic")
                if state:
                    # We need to cast to VehiclePublicLocalStateMicro
                    # Since we don't have a cast method exposed, we rely on pybind11
                    # correctly identifying the type if it was created as such.
                    # Since we created it as VehiclePublicLocalStateMicro in create_vehicle_agent,
                    # it should retain its type.
                    
                    # Convert position to lat/lon
                    # Position is a Point2D (x, y) in meters
                    pos = state.get_position()
                    lat_lon = jamfree.OSMParser.meters_to_lat_lon(
                        pos.x, pos.y, self.center_lat, self.center_lon
                    )
                    
                    vehicles_data.append({
                        'id': agent.get_id(),
                        'lat': lat_lon.x, # Point2D uses x/y, but here it represents lat/lon
                        'lon': lat_lon.y,
                        'speed': state.get_speed() * 3.6, # m/s to km/h
                        'angle': state.get_heading(),
                        'lane_id': f"lane_{state.get_lane_index()}"
                    })
            
            # Calculate performance metrics
            end_time = time.time()
            update_time_ms = (end_time - start_time) * 1000
            
            return {
                'step': self.engine.get_step_count(),
                'vehicles': vehicles_data,
                'performance': {'update_time_ms': update_time_ms}
            }
            
        except Exception as e:
            print(f"Error in simulation step: {e}")
            import traceback
            traceback.print_exc()
            return {'error': str(e)}
            update_time_ms = (time.time() - start_time) * 1000.0
            
            return {
                'step': self.current_time.get_identifier(),
                'vehicles': vehicles_data,
                'performance': {
                    'update_time_ms': update_time_ms
                }
            }
            
        except Exception as e:
            print(f"Error in simulation step: {e}")
            return {'error': str(e)}
    
    def stop(self):
        """Clean up engine resources."""
        if self.engine:
            # TODO: Proper cleanup if needed
            self.engine = None
        self.model = None
        self.vehicles = []
        self.probe = None
    
    def get_vehicle_count(self) -> int:
        """Get current number of vehicles in simulation."""
        return len(self.vehicles)
    
    def is_initialized(self) -> bool:
        """Check if engine is initialized."""
        return self.engine is not None
