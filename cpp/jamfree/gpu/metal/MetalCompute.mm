#ifdef __APPLE__

#include "MetalCompute.h"
#include <Foundation/Foundation.h>
#include <iostream>

namespace jamfree {
namespace gpu {
namespace metal {

MetalCompute::MetalCompute()
    : m_device(nil), m_command_queue(nil), m_library(nil), m_idm_pipeline(nil),
      m_update_pipeline(nil), m_gaps_pipeline(nil), m_lwr_pipeline(nil),
      m_vehicle_buffer(nil), m_params_buffer(nil), m_density_buffer(nil),
      m_density_new_buffer(nil), m_vehicle_buffer_size(0),
      m_density_buffer_size(0) {}

MetalCompute::~MetalCompute() {
  // Metal uses ARC, objects will be released automatically
}

bool MetalCompute::isAvailable() {
  @autoreleasepool {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    return device != nil;
  }
}

// Embedded Metal shader source
const char *EMBEDDED_SHADER_SOURCE = R"(
#include <metal_stdlib>
using namespace metal;

struct VehicleState {
    float position;
    float speed;
    float acceleration;
    int leader_index;
    float gap;
    float relative_speed;
};

struct IDMParams {
    float desired_speed;
    float time_headway;
    float min_gap;
    float max_accel;
    float comfortable_decel;
    float accel_exponent;
};

inline float calculate_desired_gap(float speed, float speed_diff, constant IDMParams& params) {
    float s0 = params.min_gap;
    float T = params.time_headway;
    float a = params.max_accel;
    float b = params.comfortable_decel;
    float sqrt_ab = sqrt(a * b);
    float interaction = (speed * speed_diff) / (2.0f * sqrt_ab);
    return s0 + speed * T + interaction;
}

kernel void idm_acceleration_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant IDMParams& params [[buffer(1)]],
    constant uint& num_vehicles [[buffer(2)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_vehicles) return;
    
    device VehicleState& vehicle = vehicles[thread_id];
    float v = vehicle.speed;
    float v0 = params.desired_speed;
    float a = params.max_accel;
    float delta = params.accel_exponent;
    
    float accel_free = a * (1.0f - pow(v / v0, delta));
    
    if (vehicle.leader_index < 0) {
        vehicle.acceleration = accel_free;
        return;
    }
    
    float s = vehicle.gap;
    float dv = vehicle.relative_speed;
    if (s < 0.1f) s = 0.1f;
    
    float s_star = calculate_desired_gap(v, dv, params);
    float ratio = s_star / s;
    float accel_interaction = -a * ratio * ratio;
    
    vehicle.acceleration = accel_free + accel_interaction;
    vehicle.acceleration = clamp(vehicle.acceleration, -10.0f, 5.0f);
}

kernel void update_positions_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant float& dt [[buffer(1)]],
    constant uint& num_vehicles [[buffer(2)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_vehicles) return;
    device VehicleState& vehicle = vehicles[thread_id];
    vehicle.speed += vehicle.acceleration * dt;
    vehicle.speed = max(0.0f, vehicle.speed);
    vehicle.position += vehicle.speed * dt;
}

kernel void calculate_gaps_kernel(
    device VehicleState* vehicles [[buffer(0)]],
    constant uint& num_vehicles [[buffer(1)]],
    uint thread_id [[thread_position_in_grid]])
{
    if (thread_id >= num_vehicles) return;
    device VehicleState& vehicle = vehicles[thread_id];
    
    if (vehicle.leader_index >= 0 && vehicle.leader_index < (int)num_vehicles) {
        device VehicleState& leader = vehicles[vehicle.leader_index];
        const float vehicle_length = 5.0f;
        vehicle.gap = leader.position - vehicle.position - vehicle_length;
        vehicle.relative_speed = vehicle.speed - leader.speed;
    } else {
        vehicle.gap = 1000.0f;
        vehicle.relative_speed = 0.0f;
    }
}

// Helper functions for LWR
inline float calculate_speed(float rho, float v_f, float rho_j) {
    if (rho >= rho_j) return 0.0f;
    return v_f * (1.0f - rho / rho_j);
}

inline float calculate_flow(float rho, float v_f, float rho_j) {
    return rho * calculate_speed(rho, v_f, rho_j);
}

inline float calculate_flux(float rho_l, float rho_r, float v_f, float rho_j) {
    float rho_c = rho_j / 2.0f;
    if (rho_l <= rho_c && rho_r <= rho_c) {
        return min(calculate_flow(rho_l, v_f, rho_j), calculate_flow(rho_r, v_f, rho_j));
    } else if (rho_l >= rho_c && rho_r >= rho_c) {
        return min(calculate_flow(rho_l, v_f, rho_j), calculate_flow(rho_r, v_f, rho_j));
    } else {
        return calculate_flow(rho_c, v_f, rho_j);
    }
}

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
    if (thread_id >= num_cells) return;
    
    float rho = density[thread_id];
    uint i_prev = (thread_id == 0) ? num_cells - 1 : thread_id - 1;
    uint i_next = (thread_id + 1) % num_cells;
    float rho_prev = density[i_prev];
    float rho_next = density[i_next];
    
    float flux_left = calculate_flux(rho_prev, rho, free_flow_speed, jam_density);
    float flux_right = calculate_flux(rho, rho_next, free_flow_speed, jam_density);
    
    density_new[thread_id] = rho - (dt / dx) * (flux_right - flux_left);
    density_new[thread_id] = clamp(density_new[thread_id], 0.0f, jam_density);
}
)";

bool MetalCompute::initialize(const std::string &shader_path) {
  @autoreleasepool {
    // Get default Metal device
    m_device = MTLCreateSystemDefaultDevice();
    if (!m_device) {
      std::cerr << "Metal is not supported on this device" << std::endl;
      return false;
    }

    // Create command queue
    m_command_queue = [m_device newCommandQueue];
    if (!m_command_queue) {
      std::cerr << "Failed to create Metal command queue" << std::endl;
      return false;
    }

    // Load shader library
    NSError *error = nil;
    m_library = nil;

    // 1. Try loading from file if path provided
    if (!shader_path.empty()) {
      NSString *path = [NSString stringWithUTF8String:shader_path.c_str()];
      m_library = [m_device newLibraryWithFile:path error:&error];
      if (!m_library && error) {
        std::cerr << "Warning: Failed to load Metal library from file: " <<
            [[error localizedDescription] UTF8String] << std::endl;
      }
    }

    // 2. Try loading default library (if in app bundle)
    if (!m_library) {
      m_library = [m_device newDefaultLibrary];
    }

    // 3. Fallback: Compile from embedded source
    if (!m_library) {
      std::cout << "Compiling embedded Metal shaders..." << std::endl;
      NSString *source = [NSString stringWithUTF8String:EMBEDDED_SHADER_SOURCE];
      MTLCompileOptions *options = [[MTLCompileOptions alloc] init];
      options.languageVersion = MTLLanguageVersion2_0;

      m_library = [m_device newLibraryWithSource:source
                                         options:options
                                           error:&error];

      if (!m_library) {
        std::cerr << "Failed to compile Metal shaders: "
                  << (error ? [[error localizedDescription] UTF8String]
                            : "Unknown error")
                  << std::endl;
        return false;
      }
    }

    // Create compute pipelines
    m_idm_pipeline = createPipeline("idm_acceleration_kernel");
    m_update_pipeline = createPipeline("update_positions_kernel");
    m_gaps_pipeline = createPipeline("calculate_gaps_kernel");
    m_lwr_pipeline = createPipeline("lwr_update_kernel");

    if (!m_idm_pipeline || !m_update_pipeline || !m_gaps_pipeline) {
      std::cerr << "Failed to create compute pipelines" << std::endl;
      return false;
    }

    std::cout << "Metal compute initialized on: " << getDeviceName()
              << std::endl;
    return true;
  }
}

void MetalCompute::uploadVehicles(
    const std::vector<std::shared_ptr<kernel::model::Vehicle>> &vehicles) {
  @autoreleasepool {
    size_t num_vehicles = vehicles.size();
    size_t buffer_size = num_vehicles * sizeof(GPUVehicleState);

    // Create or resize buffer if needed
    if (!m_vehicle_buffer || m_vehicle_buffer_size < buffer_size) {
      m_vehicle_buffer =
          [m_device newBufferWithLength:buffer_size
                                options:MTLResourceStorageModeShared];
      m_vehicle_buffer_size = buffer_size;
    }

    // Copy vehicle data to GPU
    GPUVehicleState *gpu_vehicles =
        (GPUVehicleState *)[m_vehicle_buffer contents];

    for (size_t i = 0; i < num_vehicles; ++i) {
      const auto &vehicle = vehicles[i];
      gpu_vehicles[i].position = vehicle->getLanePosition();
      gpu_vehicles[i].speed = vehicle->getSpeed();
      gpu_vehicles[i].acceleration = 0.0f;
      gpu_vehicles[i].leader_index = -1; // Will be set by calculateGaps
      gpu_vehicles[i].gap = 0.0f;
      gpu_vehicles[i].relative_speed = 0.0f;
    }
  }
}

void MetalCompute::downloadVehicles(
    std::vector<std::shared_ptr<kernel::model::Vehicle>> &vehicles) {
  @autoreleasepool {
    if (!m_vehicle_buffer)
      return;

    GPUVehicleState *gpu_vehicles =
        (GPUVehicleState *)[m_vehicle_buffer contents];

    for (size_t i = 0; i < vehicles.size(); ++i) {
      vehicles[i]->setLanePosition(gpu_vehicles[i].position);
      vehicles[i]->setSpeed(gpu_vehicles[i].speed);
    }
  }
}

void MetalCompute::setIDMParams(double desired_speed, double time_headway,
                                double min_gap, double max_accel,
                                double comfortable_decel,
                                double accel_exponent) {
  @autoreleasepool {
    // Create params buffer if needed
    if (!m_params_buffer) {
      m_params_buffer =
          [m_device newBufferWithLength:sizeof(GPUIDMParams)
                                options:MTLResourceStorageModeShared];
    }

    GPUIDMParams *params = (GPUIDMParams *)[m_params_buffer contents];
    params->desired_speed = desired_speed;
    params->time_headway = time_headway;
    params->min_gap = min_gap;
    params->max_accel = max_accel;
    params->comfortable_decel = comfortable_decel;
    params->accel_exponent = accel_exponent;
  }
}

void MetalCompute::computeIDMAccelerations(size_t num_vehicles) {
  @autoreleasepool {
    id<MTLCommandBuffer> command_buffer = [m_command_queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder =
        [command_buffer computeCommandEncoder];

    [encoder setComputePipelineState:m_idm_pipeline];
    [encoder setBuffer:m_vehicle_buffer offset:0 atIndex:0];
    [encoder setBuffer:m_params_buffer offset:0 atIndex:1];
    [encoder setBytes:&num_vehicles length:sizeof(uint32_t) atIndex:2];

    // Calculate thread configuration
    NSUInteger thread_group_size = m_idm_pipeline.maxTotalThreadsPerThreadgroup;
    MTLSize threads_per_group = MTLSizeMake(thread_group_size, 1, 1);
    MTLSize num_thread_groups = MTLSizeMake(
        (num_vehicles + thread_group_size - 1) / thread_group_size, 1, 1);

    [encoder dispatchThreadgroups:num_thread_groups
            threadsPerThreadgroup:threads_per_group];
    [encoder endEncoding];

    [command_buffer commit];
    [command_buffer waitUntilCompleted];
  }
}

void MetalCompute::updatePositions(size_t num_vehicles, double dt) {
  @autoreleasepool {
    float dt_float = static_cast<float>(dt);

    id<MTLCommandBuffer> command_buffer = [m_command_queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder =
        [command_buffer computeCommandEncoder];

    [encoder setComputePipelineState:m_update_pipeline];
    [encoder setBuffer:m_vehicle_buffer offset:0 atIndex:0];
    [encoder setBytes:&dt_float length:sizeof(float) atIndex:1];
    [encoder setBytes:&num_vehicles length:sizeof(uint32_t) atIndex:2];

    NSUInteger thread_group_size =
        m_update_pipeline.maxTotalThreadsPerThreadgroup;
    MTLSize threads_per_group = MTLSizeMake(thread_group_size, 1, 1);
    MTLSize num_thread_groups = MTLSizeMake(
        (num_vehicles + thread_group_size - 1) / thread_group_size, 1, 1);

    [encoder dispatchThreadgroups:num_thread_groups
            threadsPerThreadgroup:threads_per_group];
    [encoder endEncoding];

    [command_buffer commit];
    [command_buffer waitUntilCompleted];
  }
}

void MetalCompute::calculateGaps(size_t num_vehicles) {
  @autoreleasepool {
    id<MTLCommandBuffer> command_buffer = [m_command_queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder =
        [command_buffer computeCommandEncoder];

    [encoder setComputePipelineState:m_gaps_pipeline];
    [encoder setBuffer:m_vehicle_buffer offset:0 atIndex:0];
    [encoder setBytes:&num_vehicles length:sizeof(uint32_t) atIndex:1];

    NSUInteger thread_group_size =
        m_gaps_pipeline.maxTotalThreadsPerThreadgroup;
    MTLSize threads_per_group = MTLSizeMake(thread_group_size, 1, 1);
    MTLSize num_thread_groups = MTLSizeMake(
        (num_vehicles + thread_group_size - 1) / thread_group_size, 1, 1);

    [encoder dispatchThreadgroups:num_thread_groups
            threadsPerThreadgroup:threads_per_group];
    [encoder endEncoding];

    [command_buffer commit];
    [command_buffer waitUntilCompleted];
  }
}

void MetalCompute::simulationStep(size_t num_vehicles, double dt) {
  // Run all kernels in sequence
  calculateGaps(num_vehicles);
  computeIDMAccelerations(num_vehicles);
  updatePositions(num_vehicles, dt);
}

void MetalCompute::updateLWR(const std::vector<double> &density,
                             std::vector<double> &density_new, size_t num_cells,
                             double dt, double dx, double free_flow_speed,
                             double jam_density) {
  @autoreleasepool {
    // Convert to float
    std::vector<float> density_f(density.begin(), density.end());
    std::vector<float> density_new_f(num_cells);

    size_t buffer_size = num_cells * sizeof(float);

    // Create or resize buffers
    if (!m_density_buffer || m_density_buffer_size < buffer_size) {
      m_density_buffer =
          [m_device newBufferWithLength:buffer_size
                                options:MTLResourceStorageModeShared];
      m_density_new_buffer =
          [m_device newBufferWithLength:buffer_size
                                options:MTLResourceStorageModeShared];
      m_density_buffer_size = buffer_size;
    }

    // Upload density
    memcpy([m_density_buffer contents], density_f.data(), buffer_size);

    // Run kernel
    id<MTLCommandBuffer> command_buffer = [m_command_queue commandBuffer];
    id<MTLComputeCommandEncoder> encoder =
        [command_buffer computeCommandEncoder];

    [encoder setComputePipelineState:m_lwr_pipeline];
    [encoder setBuffer:m_density_buffer offset:0 atIndex:0];
    [encoder setBuffer:m_density_new_buffer offset:0 atIndex:1];

    uint32_t num_cells_u32 = static_cast<uint32_t>(num_cells);
    float dt_f = static_cast<float>(dt);
    float dx_f = static_cast<float>(dx);
    float v_f = static_cast<float>(free_flow_speed);
    float rho_j = static_cast<float>(jam_density);

    [encoder setBytes:&num_cells_u32 length:sizeof(uint32_t) atIndex:2];
    [encoder setBytes:&dt_f length:sizeof(float) atIndex:3];
    [encoder setBytes:&dx_f length:sizeof(float) atIndex:4];
    [encoder setBytes:&v_f length:sizeof(float) atIndex:5];
    [encoder setBytes:&rho_j length:sizeof(float) atIndex:6];

    NSUInteger thread_group_size = m_lwr_pipeline.maxTotalThreadsPerThreadgroup;
    MTLSize threads_per_group = MTLSizeMake(thread_group_size, 1, 1);
    MTLSize num_thread_groups = MTLSizeMake(
        (num_cells + thread_group_size - 1) / thread_group_size, 1, 1);

    [encoder dispatchThreadgroups:num_thread_groups
            threadsPerThreadgroup:threads_per_group];
    [encoder endEncoding];

    [command_buffer commit];
    [command_buffer waitUntilCompleted];

    // Download result
    memcpy(density_new_f.data(), [m_density_new_buffer contents], buffer_size);
    density_new.assign(density_new_f.begin(), density_new_f.end());
  }
}

std::string MetalCompute::getDeviceName() const {
  @autoreleasepool {
    if (m_device) {
      return std::string([[m_device name] UTF8String]);
    }
    return "No device";
  }
}

size_t MetalCompute::getMaxThreadsPerThreadgroup() const {
  @autoreleasepool {
    if (m_idm_pipeline) {
      return m_idm_pipeline.maxTotalThreadsPerThreadgroup;
    }
    return 0;
  }
}

id<MTLComputePipelineState>
MetalCompute::createPipeline(const std::string &kernel_name) {
  @autoreleasepool {
    NSString *name = [NSString stringWithUTF8String:kernel_name.c_str()];
    id<MTLFunction> function = [m_library newFunctionWithName:name];

    if (!function) {
      std::cerr << "Failed to find kernel function: " << kernel_name
                << std::endl;
      return nil;
    }

    NSError *error = nil;
    id<MTLComputePipelineState> pipeline =
        [m_device newComputePipelineStateWithFunction:function error:&error];

    if (!pipeline) {
      if (error) {
        std::cerr << "Failed to create pipeline for " << kernel_name << ": "
                  << [[error localizedDescription] UTF8String] << std::endl;
      }
      return nil;
    }

    return pipeline;
  }
}

} // namespace metal
} // namespace gpu
} // namespace jamfree

#endif // __APPLE__
