#ifndef JAMFREE_GPU_METAL_COMPUTE_H
#define JAMFREE_GPU_METAL_COMPUTE_H

#ifdef __APPLE__

#include "../../kernel/include/model/Vehicle.h"
#include <Metal/Metal.h>
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace gpu {
namespace metal {

/**
 * @brief Vehicle state for GPU processing.
 */
struct GPUVehicleState {
  float position;
  float speed;
  float acceleration;
  int leader_index;
  float gap;
  float relative_speed;
};

/**
 * @brief IDM parameters for GPU.
 */
struct GPUIDMParams {
  float desired_speed;
  float time_headway;
  float min_gap;
  float max_accel;
  float comfortable_decel;
  float accel_exponent;
};

/**
 * @brief Metal compute engine for traffic simulation.
 *
 * Provides GPU-accelerated traffic simulation using Apple Metal.
 * Achieves 10-100x speedup for large-scale simulations (10,000+ vehicles).
 */
class MetalCompute {
public:
  /**
   * @brief Constructor - initializes Metal device and pipeline.
   */
  MetalCompute();

  /**
   * @brief Destructor - releases Metal resources.
   */
  ~MetalCompute();

  /**
   * @brief Check if Metal is available.
   *
   * @return True if Metal is supported
   */
  static bool isAvailable();

  /**
   * @brief Initialize Metal compute pipeline.
   *
   * @param shader_path Path to compiled Metal shader library
   * @return True if initialization successful
   */
  bool initialize(const std::string &shader_path);

  /**
   * @brief Upload vehicle states to GPU.
   *
   * @param vehicles Vector of vehicles
   */
  void uploadVehicles(
      const std::vector<std::shared_ptr<kernel::model::Vehicle>> &vehicles);

  /**
   * @brief Download vehicle states from GPU.
   *
   * @param vehicles Vector of vehicles to update
   */
  void downloadVehicles(
      std::vector<std::shared_ptr<kernel::model::Vehicle>> &vehicles);

  /**
   * @brief Set IDM parameters.
   *
   * @param desired_speed Desired speed (m/s)
   * @param time_headway Time headway (s)
   * @param min_gap Minimum gap (m)
   * @param max_accel Maximum acceleration (m/s²)
   * @param comfortable_decel Comfortable deceleration (m/s²)
   * @param accel_exponent Acceleration exponent
   */
  void setIDMParams(double desired_speed, double time_headway, double min_gap,
                    double max_accel, double comfortable_decel,
                    double accel_exponent);

  /**
   * @brief Compute IDM accelerations on GPU.
   *
   * @param num_vehicles Number of vehicles
   */
  void computeIDMAccelerations(size_t num_vehicles);

  /**
   * @brief Update vehicle positions on GPU.
   *
   * @param num_vehicles Number of vehicles
   * @param dt Time step (s)
   */
  void updatePositions(size_t num_vehicles, double dt);

  /**
   * @brief Calculate gaps and relative speeds on GPU.
   *
   * @param num_vehicles Number of vehicles
   */
  void calculateGaps(size_t num_vehicles);

  /**
   * @brief Run complete simulation step on GPU.
   *
   * Combines gap calculation, IDM acceleration, and position update.
   *
   * @param num_vehicles Number of vehicles
   * @param dt Time step (s)
   */
  void simulationStep(size_t num_vehicles, double dt);

  /**
   * @brief Update LWR macroscopic model on GPU.
   *
   * @param density Input density array
   * @param density_new Output density array
   * @param num_cells Number of cells
   * @param dt Time step
   * @param dx Cell length
   * @param free_flow_speed Free-flow speed
   * @param jam_density Jam density
   */
  void updateLWR(const std::vector<double> &density,
                 std::vector<double> &density_new, size_t num_cells, double dt,
                 double dx, double free_flow_speed, double jam_density);

  /**
   * @brief Get GPU device name.
   *
   * @return Device name string
   */
  std::string getDeviceName() const;

  /**
   * @brief Get maximum threads per threadgroup.
   *
   * @return Max threads
   */
  size_t getMaxThreadsPerThreadgroup() const;

private:
  id<MTLDevice> m_device;
  id<MTLCommandQueue> m_command_queue;
  id<MTLLibrary> m_library;

  // Compute pipelines
  id<MTLComputePipelineState> m_idm_pipeline;
  id<MTLComputePipelineState> m_update_pipeline;
  id<MTLComputePipelineState> m_gaps_pipeline;
  id<MTLComputePipelineState> m_lwr_pipeline;

  // Buffers
  id<MTLBuffer> m_vehicle_buffer;
  id<MTLBuffer> m_params_buffer;
  id<MTLBuffer> m_density_buffer;
  id<MTLBuffer> m_density_new_buffer;

  size_t m_vehicle_buffer_size;
  size_t m_density_buffer_size;

  /**
   * @brief Create compute pipeline for kernel.
   *
   * @param kernel_name Name of kernel function
   * @return Pipeline state
   */
  id<MTLComputePipelineState> createPipeline(const std::string &kernel_name);

  /**
   * @brief Dispatch compute kernel.
   *
   * @param pipeline Pipeline to execute
   * @param num_threads Number of threads
   */
  void dispatchKernel(id<MTLComputePipelineState> pipeline, size_t num_threads);
};

} // namespace metal
} // namespace gpu
} // namespace jamfree

#endif // __APPLE__
#endif // JAMFREE_GPU_METAL_COMPUTE_H
