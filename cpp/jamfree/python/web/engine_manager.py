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
        idm = jamfree.IDM()
        idm.setDesiredSpeed(config.get('desired_speed', 33.3))
        idm.setMinGap(config.get('min_gap', 2.0))
        idm.setTimeHeadway(config.get('time_headway', 1.5))
        idm.setMaxAccel(config.get('max_accel', 3.0))
        idm.setComfortableDecel(config.get('comfort_decel', 2.0))
        idm.setAccelExponent(config.get('accel_exponent', 4.0))
        
        mobil = jamfree.MOBIL()
        mobil.setPoliteness(config.get('politeness', 0.5))
        mobil.setThreshold(config.get('lc_threshold', 0.1))
        mobil.setMaxSafeDecel(config.get('max_safe_decel', 4.0))
        mobil.setBiasRight(config.get('bias_right', 0.1))
        
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
        public_state.setSpeed(config.get('initial_speed', 30.0))
        public_state.setLanePosition(config.get('initial_position', 0.0))
        public_state.setCurrentLane(initial_lane)
        public_state.setActive(True)
        public_state.setLength(config.get('length', 5.0))
        public_state.setWidth(config.get('width', 2.0))
        
        private_state = jamfree.VehiclePrivateLocalStateMicro(vehicle_id)
        # Note: Parameters are also set in IDM/MOBIL, but state stores them for reference/other uses
        private_state.setDesiredSpeed(config.get('desired_speed', 33.3))
        private_state.setMinGap(config.get('min_gap', 2.0))
        private_state.setTimeHeadway(config.get('time_headway', 1.5))
        private_state.setMaxAcceleration(config.get('max_accel', 3.0))
        private_state.setComfortableDeceleration(config.get('comfort_decel', 2.0))
        private_state.setPoliteness(config.get('politeness', 0.5))
        private_state.setLaneChangingThreshold(config.get('lc_threshold', 0.1))
        
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
            # Note: Simplified initialization without full vehicle creation
            # Full implementation requires perception/decision model bindings
            
            # Create MultithreadedSimulationEngine
            num_threads = self.config.get('num_threads', 0)  # 0 = auto-detect
            self.engine = jamfree.MultithreadedSimulationEngine(num_threads)
            
            # Create visualization probe
            self.probe = VisualizationProbe(self.center_lat, self.center_lon)
            
            # TODO: Full vehicle creation and model setup
            # This requires:
            # 1. Create vehicles using create_vehicle_agent()
            # 2. Create TrafficSimulationModel with vehicles
            # 3. Register probe with engine
            
            print(f"Engine initialized with {num_threads} threads")
            print("Note: Full vehicle simulation not yet implemented")
            
            return True
            
        except Exception as e:
            print(f"Error initializing engine: {e}")
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
            next_time = jamfree.SimulationTimeStamp(
                self.current_time.get_identifier() + 1
            )
            
            # TODO: Run simulation step when model is ready
            # self.engine.runNewSimulation(self.model, next_time)
            # For now, just increment time
            
            self.current_time = next_time
            
            # Extract vehicle data from probe
            vehicles_data = self.probe.get_data() if self.probe else []
            
            # Calculate performance metrics
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
