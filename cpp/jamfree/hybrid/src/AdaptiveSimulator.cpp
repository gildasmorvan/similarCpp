#include "../include/AdaptiveSimulator.h"
#include <chrono>
#include <iostream>

namespace jamfree {
namespace hybrid {

void AdaptiveSimulator::registerLane(
    const std::shared_ptr<kernel::model::Lane> &lane, bool is_critical) {

  LaneState state;
  state.lane = lane;
  state.mode = SimulationMode::MICROSCOPIC; // Start with microscopic
  state.current_density = 0.0;
  state.avg_speed = 0.0;
  state.flow = 0.0;
  state.vehicle_count = 0;
  state.last_update_time_ms = 0.0;
  state.is_critical_area = is_critical || isCriticalArea(lane);
  state.frames_since_transition = 0;

  // Get existing vehicles
  state.vehicles = lane->getVehicles();

  m_lane_states[lane->getId()] = std::move(state);
}

void AdaptiveSimulator::update(double dt, const microscopic::models::IDM &idm) {
  for (auto &[lane_id, state] : m_lane_states) {
    auto start = std::chrono::high_resolution_clock::now();

    // Update metrics
    updateMetrics(state);

    // Check if mode switch needed
    if (shouldSwitchMode(state)) {
      if (state.mode == SimulationMode::MICROSCOPIC) {
        transitionToMacro(state);
      } else if (state.mode == SimulationMode::MACROSCOPIC) {
        transitionToMicro(state);
      }
    }

    // Update based on current mode
    if (state.mode == SimulationMode::MICROSCOPIC) {
      updateMicroscopic(state, dt, idm);
    } else if (state.mode == SimulationMode::MACROSCOPIC) {
      updateMacroscopic(state, dt);
    }

    // Track update time
    auto end = std::chrono::high_resolution_clock::now();
    state.last_update_time_ms =
        std::chrono::duration<double, std::milli>(end - start).count();

    state.frames_since_transition++;
  }
}

bool AdaptiveSimulator::shouldSwitchMode(LaneState &state) {
  // Never switch critical areas if configured
  if (state.is_critical_area && m_config.force_micro_intersections) {
    return false;
  }

  // Prevent rapid oscillation (hysteresis)
  if (state.frames_since_transition < 30) { // ~3 seconds at 10 FPS
    return false;
  }

  // Check density-based switching
  if (state.mode == SimulationMode::MICROSCOPIC) {
    // Switch to macro if density too high
    bool high_density = state.current_density > m_config.micro_to_macro_density;
    bool too_many_vehicles =
        state.vehicle_count > m_config.micro_to_macro_count;
    bool slow_update = state.last_update_time_ms > m_config.max_micro_time_ms;

    return high_density || too_many_vehicles || slow_update;

  } else if (state.mode == SimulationMode::MACROSCOPIC) {
    // Switch to micro if density low enough (with hysteresis)
    bool low_density = state.current_density < m_config.macro_to_micro_density /
                                                   m_config.hysteresis_factor;
    bool few_vehicles = state.vehicle_count < m_config.macro_to_micro_count;

    return low_density && few_vehicles;
  }

  return false;
}

void AdaptiveSimulator::transitionToMacro(LaneState &state) {
  std::cout << "Lane " << state.lane->getId()
            << ": Transitioning to MACROSCOPIC (density="
            << state.current_density << ", vehicles=" << state.vehicle_count
            << ")" << std::endl;

  // Create LWR model
  state.lwr_model = std::make_unique<macroscopic::models::LWR>(
      state.lane->getSpeedLimit(),
      0.15, // jam_density
      state.lane->getLength(), m_config.macro_num_cells);

  // Initialize LWR from microscopic state
  macroscopic::models::MicroMacroBridge::initializeLWRFromLane(
      state.lane, *state.lwr_model);

  // Remove individual vehicles from lane
  // (They're now represented as density)
  for (auto &vehicle : state.vehicles) {
    state.lane->removeVehicle(vehicle);
  }
  state.vehicles.clear();

  state.mode = SimulationMode::MACROSCOPIC;
  state.frames_since_transition = 0;
}

void AdaptiveSimulator::transitionToMicro(LaneState &state) {
  std::cout << "Lane " << state.lane->getId()
            << ": Transitioning to MICROSCOPIC (density="
            << state.current_density << ", vehicles=" << state.vehicle_count
            << ")" << std::endl;

  if (!state.lwr_model) {
    std::cerr << "Error: No LWR model to transition from" << std::endl;
    return;
  }

  // Generate vehicles from macroscopic density
  int num_cells = state.lwr_model->getNumCells();
  double cell_length = state.lwr_model->getCellLength();

  int vehicle_id = 0;
  for (int i = 0; i < num_cells; ++i) {
    double density = state.lwr_model->getDensity(i);
    double speed = state.lwr_model->getSpeed(i);

    // Calculate number of vehicles in this cell
    int num_vehicles_in_cell = static_cast<int>(density * cell_length + 0.5);

    // Create vehicles
    for (int j = 0; j < num_vehicles_in_cell; ++j) {
      auto vehicle = std::make_shared<kernel::model::Vehicle>(
          state.lane->getId() + "_v" + std::to_string(vehicle_id++));

      // Position within cell
      double position =
          i * cell_length + (j + 0.5) * cell_length / num_vehicles_in_cell;

      vehicle->setCurrentLane(state.lane);
      vehicle->setLanePosition(position);
      vehicle->setSpeed(speed);

      state.lane->addVehicle(vehicle);
      state.vehicles.push_back(vehicle);
    }
  }

  // Clear macroscopic model
  state.lwr_model.reset();

  state.mode = SimulationMode::MICROSCOPIC;
  state.frames_since_transition = 0;
}

void AdaptiveSimulator::updateMicroscopic(LaneState &state, double dt,
                                          const microscopic::models::IDM &idm) {

  // Update each vehicle using IDM
  for (auto &vehicle : state.vehicles) {
    auto leader = state.lane->getLeader(*vehicle);
    double acc = idm.calculateAcceleration(*vehicle, leader);
    vehicle->update(dt, acc);
  }

  // Sync with lane's vehicle list
  state.vehicles = state.lane->getVehicles();
}

void AdaptiveSimulator::updateMacroscopic(LaneState &state, double dt) {
  if (!state.lwr_model) {
    std::cerr << "Error: No LWR model for macroscopic update" << std::endl;
    return;
  }

  // Update LWR model
  state.lwr_model->update(dt);
}

void AdaptiveSimulator::updateMetrics(LaneState &state) {
  if (state.mode == SimulationMode::MICROSCOPIC) {
    // Calculate from individual vehicles
    auto stats = macroscopic::models::MicroMacroBridge::calculateAggregateStats(
        state.lane);

    state.current_density = stats.avg_density;
    state.avg_speed = stats.avg_speed;
    state.flow = stats.avg_flow;
    state.vehicle_count = stats.num_vehicles;

  } else if (state.mode == SimulationMode::MACROSCOPIC) {
    // Calculate from LWR model
    if (state.lwr_model) {
      int num_cells = state.lwr_model->getNumCells();
      double total_density = 0.0;
      double total_speed = 0.0;
      double total_flow = 0.0;
      int total_vehicles = 0;

      for (int i = 0; i < num_cells; ++i) {
        double density = state.lwr_model->getDensity(i);
        double speed = state.lwr_model->getSpeed(i);
        double flow = state.lwr_model->getFlow(i);

        total_density += density;
        total_speed += speed;
        total_flow += flow;
        total_vehicles +=
            static_cast<int>(density * state.lwr_model->getCellLength());
      }

      state.current_density = total_density / num_cells;
      state.avg_speed = total_speed / num_cells;
      state.flow = total_flow / num_cells;
      state.vehicle_count = total_vehicles;
    }
  }
}

bool AdaptiveSimulator::isCriticalArea(
    const std::shared_ptr<kernel::model::Lane> &lane) const {
  // Check if lane is part of intersection or ramp
  // This would typically check road network topology
  // For now, simple heuristic based on lane properties

  auto parent_road = lane->getParentRoad();
  if (!parent_road) {
    return false;
  }

  // Short roads are likely intersections
  if (parent_road->getLength() < 50.0) {
    return m_config.force_micro_intersections;
  }

  // Could add more sophisticated checks here
  return false;
}

SimulationMode AdaptiveSimulator::getMode(const std::string &lane_id) const {
  auto it = m_lane_states.find(lane_id);
  if (it != m_lane_states.end()) {
    return it->second.mode;
  }
  return SimulationMode::MICROSCOPIC;
}

const AdaptiveSimulator::LaneState *
AdaptiveSimulator::getLaneState(const std::string &lane_id) const {
  auto it = m_lane_states.find(lane_id);
  if (it != m_lane_states.end()) {
    return &it->second;
  }
  return nullptr;
}

AdaptiveSimulator::Statistics AdaptiveSimulator::getStatistics() const {
  Statistics stats{};
  stats.total_lanes = m_lane_states.size();
  stats.micro_lanes = 0;
  stats.macro_lanes = 0;
  stats.transitioning_lanes = 0;
  stats.total_vehicles = 0;
  stats.avg_density = 0.0;
  stats.total_update_time_ms = 0.0;

  for (const auto &[lane_id, state] : m_lane_states) {
    switch (state.mode) {
    case SimulationMode::MICROSCOPIC:
      stats.micro_lanes++;
      break;
    case SimulationMode::MACROSCOPIC:
      stats.macro_lanes++;
      break;
    case SimulationMode::TRANSITIONING:
      stats.transitioning_lanes++;
      break;
    }

    stats.total_vehicles += state.vehicle_count;
    stats.avg_density += state.current_density;
    stats.total_update_time_ms += state.last_update_time_ms;
  }

  if (stats.total_lanes > 0) {
    stats.avg_density /= stats.total_lanes;
  }

  // Estimate speedup (macro is ~50x faster than micro)
  double equivalent_micro_time =
      stats.micro_lanes * 1.0 + stats.macro_lanes * 50.0;
  double actual_time = stats.total_lanes * 1.0;
  stats.speedup_factor = equivalent_micro_time / actual_time;

  return stats;
}

void AdaptiveSimulator::forceMicroscopic(const std::string &lane_id) {
  auto it = m_lane_states.find(lane_id);
  if (it != m_lane_states.end()) {
    if (it->second.mode == SimulationMode::MACROSCOPIC) {
      transitionToMicro(it->second);
    }
    it->second.is_critical_area = true; // Prevent auto-switch
  }
}

void AdaptiveSimulator::forceMacroscopic(const std::string &lane_id) {
  auto it = m_lane_states.find(lane_id);
  if (it != m_lane_states.end()) {
    if (it->second.mode == SimulationMode::MICROSCOPIC) {
      transitionToMacro(it->second);
    }
  }
}

void AdaptiveSimulator::allowAutomatic(const std::string &lane_id) {
  auto it = m_lane_states.find(lane_id);
  if (it != m_lane_states.end()) {
    it->second.is_critical_area = isCriticalArea(it->second.lane);
  }
}

} // namespace hybrid
} // namespace jamfree
