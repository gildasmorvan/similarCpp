#ifndef JAMFREE_GPU_METAL_IDM_KERNEL_H
#define JAMFREE_GPU_METAL_IDM_KERNEL_H

#include <metal_stdlib>
using namespace metal;

/**
 * @brief Vehicle state structure for GPU processing.
 */
struct VehicleState {
    float position;        // Lane position (m)
    float speed;           // Current speed (m/s)
    float acceleration;    // Current acceleration (m/s²)
    int leader_index;      // Index of leader vehicle (-1 if none)
    float gap;             // Gap to leader (m)
    float relative_speed;  // Relative speed to leader (m/s)
};

/**
 * @brief IDM parameters structure.
 */
struct IDMParams {
    float desired_speed;      // v₀ (m/s)
    float time_headway;       // T (s)
    float min_gap;            // s₀ (m)
    float max_accel;          // a (m/s²)
    float comfortable_decel;  // b (m/s²)
    float accel_exponent;     // δ
};

/**
 * @brief Calculate desired gap for IDM.
 */
inline float calculate_desired_gap(float speed, float speed_diff,
                                   constant IDMParams& params) {
    float s0 = params.min_gap;
    float T = params.time_headway;
    float a = params.max_accel;
    float b = params.comfortable_decel;
    
    // Interaction term
    float sqrt_ab = sqrt(a * b);
    float interaction = (speed * speed_diff) / (2.0f * sqrt_ab);
    
    // Desired gap
    return s0 + speed * T + interaction;
}

/**
 * @brief IDM acceleration kernel.
 *
 * Computes acceleration for all vehicles in parallel.
 *
 * @param vehicles Input/output vehicle states
 * @param params IDM parameters
 * @param num_vehicles Number of vehicles
 * @param thread_id Thread identifier
 */
kernel void idm_acceleration_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant IDMParams& params [[buffer(1)]],
    constant uint& num_vehicles [[buffer(2)]],
    uint thread_id [[thread_position_in_grid]])
{
    // Bounds check
    if (thread_id >= num_vehicles) {
        return;
    }
    
    // Get vehicle state
    device VehicleState& vehicle = vehicles[thread_id];
    float v = vehicle.speed;
    float v0 = params.desired_speed;
    float a = params.max_accel;
    float delta = params.accel_exponent;
    
    // Free-flow acceleration term
    float accel_free = a * (1.0f - pow(v / v0, delta));
    
    // Check if vehicle has a leader
    if (vehicle.leader_index < 0) {
        // No leader - just free-flow acceleration
        vehicle.acceleration = accel_free;
        return;
    }
    
    // Get gap and relative speed
    float s = vehicle.gap;
    float dv = vehicle.relative_speed;
    
    // Avoid division by zero
    if (s < 0.1f) {
        s = 0.1f;
    }
    
    // Calculate desired gap
    float s_star = calculate_desired_gap(v, dv, params);
    
    // Interaction term
    float ratio = s_star / s;
    float accel_interaction = -a * ratio * ratio;
    
    // Total acceleration
    vehicle.acceleration = accel_free + accel_interaction;
    
    // Clamp to reasonable bounds
    vehicle.acceleration = clamp(vehicle.acceleration, -10.0f, 5.0f);
}

/**
 * @brief Update vehicle positions kernel.
 *
 * Integrates acceleration to update speed and position.
 *
 * @param vehicles Input/output vehicle states
 * @param dt Time step (s)
 * @param num_vehicles Number of vehicles
 * @param thread_id Thread identifier
 */
kernel void update_positions_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant float& dt [[buffer(1)]],
    constant uint& num_vehicles [[buffer(2)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_vehicles) {
        return;
    }
    
    device VehicleState& vehicle = vehicles[thread_id];
    
    // Update speed
    vehicle.speed += vehicle.acceleration * dt;
    
    // Clamp speed to non-negative
    vehicle.speed = max(0.0f, vehicle.speed);
    
    // Update position
    vehicle.position += vehicle.speed * dt;
}

/**
 * @brief Calculate gaps and relative speeds kernel.
 *
 * Computes gap and relative speed to leader for each vehicle.
 *
 * @param vehicles Input/output vehicle states
 * @param num_vehicles Number of vehicles
 * @param thread_id Thread identifier
 */
kernel void calculate_gaps_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant uint& num_vehicles [[buffer(1)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_vehicles) {
        return;
    }
    
    device VehicleState& vehicle = vehicles[thread_id];
    
    if (vehicle.leader_index >= 0 && 
        vehicle.leader_index < (int)num_vehicles) {
        device VehicleState& leader = vehicles[vehicle.leader_index];
        
        // Calculate gap (leader position - vehicle position - vehicle length)
        const float vehicle_length = 5.0f; // Assume 5m vehicle length
        vehicle.gap = leader.position - vehicle.position - vehicle_length;
        
        // Calculate relative speed (vehicle speed - leader speed)
        vehicle.relative_speed = vehicle.speed - leader.speed;
    } else {
        // No leader
        vehicle.gap = 1000.0f; // Large gap
        vehicle.relative_speed = 0.0f;
    }
}

/**
 * @brief Macroscopic LWR update kernel.
 *
 * Updates density using Godunov scheme for LWR model.
 *
 * @param density Input density array
 * @param density_new Output density array
 * @param num_cells Number of cells
 * @param dt Time step
 * @param dx Cell length
 * @param free_flow_speed Free-flow speed
 * @param jam_density Jam density
 * @param thread_id Thread identifier
 */
kernel void lwr_update_kernel(
    device const float* density [[buffer(0)]],
    device float* density_new [[buffer(1)]],
    constant uint& num_cells [[buffer(2)]],
    constant float& dt [[buffer(3)]],
    constant float& dx [[buffer(4)]],
    constant float& free_flow_speed [[buffer(5)]],
    constant float& jam_density [[buffer(6)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_cells) {
        return;
    }
    
    // Get current cell density
    float rho = density[thread_id];
    
    // Get neighbor densities (periodic boundary conditions)
    uint i_prev = (thread_id == 0) ? num_cells - 1 : thread_id - 1;
    uint i_next = (thread_id + 1) % num_cells;
    
    float rho_prev = density[i_prev];
    float rho_next = density[i_next];
    
    // Calculate critical density
    float rho_c = jam_density / 2.0f;
    
    // Speed from density (Greenshields)
    auto speed_from_density = [](float rho, float v_f, float rho_j) -> float {
        if (rho >= rho_j) return 0.0f;
        return v_f * (1.0f - rho / rho_j);
    };
    
    // Flow from density
    auto flow_from_density = [&](float rho) -> float {
        return rho * speed_from_density(rho, free_flow_speed, jam_density);
    };
    
    // Godunov flux calculation
    auto godunov_flux = [&](float rho_l, float rho_r) -> float {
        if (rho_l <= rho_c && rho_r <= rho_c) {
            return min(flow_from_density(rho_l), flow_from_density(rho_r));
        } else if (rho_l >= rho_c && rho_r >= rho_c) {
            return min(flow_from_density(rho_l), flow_from_density(rho_r));
        } else {
            return flow_from_density(rho_c);
        }
    };
    
    // Calculate fluxes
    float flux_left = godunov_flux(rho_prev, rho);
    float flux_right = godunov_flux(rho, rho_next);
    
    // Update density
    density_new[thread_id] = rho - (dt / dx) * (flux_right - flux_left);
    
    // Clamp to physical bounds
    density_new[thread_id] = clamp(density_new[thread_id], 0.0f, jam_density);
}

#endif // JAMFREE_GPU_METAL_IDM_KERNEL_H
