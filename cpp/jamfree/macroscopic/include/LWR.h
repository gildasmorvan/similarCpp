#ifndef JAMFREE_MACROSCOPIC_MODELS_LWR_H
#define JAMFREE_MACROSCOPIC_MODELS_LWR_H

#include "../../kernel/include/tools/MathTools.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace jamfree {
namespace macroscopic {
namespace models {

/**
 * @brief Lighthill-Whitham-Richards (LWR) traffic flow model.
 *
 * The LWR model is a first-order macroscopic model based on conservation
 * of vehicles. It describes traffic flow using:
 * - Density (ρ): vehicles per unit length
 * - Flow (q): vehicles per unit time
 * - Speed (v): average speed
 *
 * The fundamental relationship: q = ρ * v
 *
 * Reference: Lighthill, M. J., & Whitham, G. B. (1955).
 * "On kinematic waves II. A theory of traffic flow on long crowded roads."
 */
class LWR {
public:
  /**
   * @brief Constructor with parameters.
   *
   * @param free_flow_speed Maximum speed when density is zero (m/s)
   * @param jam_density Maximum density (vehicles/m)
   * @param road_length Length of road segment (m)
   * @param num_cells Number of discretization cells
   */
  LWR(double free_flow_speed = 33.3, // 120 km/h
      double jam_density = 0.15,     // ~150 vehicles/km
      double road_length = 1000.0,   // 1 km
      int num_cells = 100)
      : m_free_flow_speed(free_flow_speed), m_jam_density(jam_density),
        m_road_length(road_length), m_num_cells(num_cells),
        m_cell_length(road_length / num_cells) {
    // Initialize density array
    m_density.resize(num_cells, 0.0);
    m_density_new.resize(num_cells, 0.0);
  }

  /**
   * @brief Fundamental diagram: speed as function of density.
   *
   * Greenshields model: v(ρ) = v_f * (1 - ρ/ρ_jam)
   *
   * @param density Current density (vehicles/m)
   * @return Speed (m/s)
   */
  double speedFromDensity(double density) const {
    if (density >= m_jam_density) {
      return 0.0;
    }
    return m_free_flow_speed * (1.0 - density / m_jam_density);
  }

  /**
   * @brief Fundamental diagram: flow as function of density.
   *
   * q(ρ) = ρ * v(ρ)
   *
   * @param density Current density (vehicles/m)
   * @return Flow (vehicles/s)
   */
  double flowFromDensity(double density) const {
    return density * speedFromDensity(density);
  }

  /**
   * @brief Calculate critical density (maximum flow).
   *
   * @return Critical density (vehicles/m)
   */
  double getCriticalDensity() const { return m_jam_density / 2.0; }

  /**
   * @brief Calculate maximum flow (capacity).
   *
   * @return Maximum flow (vehicles/s)
   */
  double getMaxFlow() const {
    double rho_c = getCriticalDensity();
    return flowFromDensity(rho_c);
  }

  /**
   * @brief Update traffic state using Godunov scheme.
   *
   * Solves the conservation equation:
   * ∂ρ/∂t + ∂q/∂x = 0
   *
   * @param dt Time step (seconds)
   */
  void update(double dt) {
    // Godunov scheme (upwind)
    for (int i = 0; i < m_num_cells; ++i) {
      int i_prev = (i - 1 + m_num_cells) % m_num_cells; // Periodic BC
      int i_next = (i + 1) % m_num_cells;

      // Calculate fluxes at cell boundaries
      double flux_left = calculateFlux(m_density[i_prev], m_density[i]);
      double flux_right = calculateFlux(m_density[i], m_density[i_next]);

      // Update density using conservation law
      m_density_new[i] =
          m_density[i] - (dt / m_cell_length) * (flux_right - flux_left);

      // Ensure physical bounds
      m_density_new[i] =
          std::max(0.0, std::min(m_jam_density, m_density_new[i]));
    }

    // Swap buffers
    m_density.swap(m_density_new);
  }

  /**
   * @brief Set density at a specific cell.
   *
   * @param cell_index Cell index
   * @param density Density value (vehicles/m)
   */
  void setDensity(int cell_index, double density) {
    if (cell_index >= 0 && cell_index < m_num_cells) {
      m_density[cell_index] = std::max(0.0, std::min(m_jam_density, density));
    }
  }

  /**
   * @brief Get density at a specific cell.
   *
   * @param cell_index Cell index
   * @return Density (vehicles/m)
   */
  double getDensity(int cell_index) const {
    if (cell_index >= 0 && cell_index < m_num_cells) {
      return m_density[cell_index];
    }
    return 0.0;
  }

  /**
   * @brief Get flow at a specific cell.
   *
   * @param cell_index Cell index
   * @return Flow (vehicles/s)
   */
  double getFlow(int cell_index) const {
    return flowFromDensity(getDensity(cell_index));
  }

  /**
   * @brief Get speed at a specific cell.
   *
   * @param cell_index Cell index
   * @return Speed (m/s)
   */
  double getSpeed(int cell_index) const {
    return speedFromDensity(getDensity(cell_index));
  }

  // Getters
  int getNumCells() const { return m_num_cells; }
  double getRoadLength() const { return m_road_length; }
  double getCellLength() const { return m_cell_length; }
  double getFreeFlowSpeed() const { return m_free_flow_speed; }
  double getJamDensity() const { return m_jam_density; }

  /**
   * @brief Get all density values.
   *
   * @return Vector of densities
   */
  const std::vector<double> &getDensities() const { return m_density; }

private:
  double m_free_flow_speed; ///< Free-flow speed (m/s)
  double m_jam_density;     ///< Jam density (vehicles/m)
  double m_road_length;     ///< Road length (m)
  int m_num_cells;          ///< Number of cells
  double m_cell_length;     ///< Cell length (m)

  std::vector<double> m_density;     ///< Current density state
  std::vector<double> m_density_new; ///< Next density state

  /**
   * @brief Calculate Godunov flux between two cells.
   *
   * @param rho_left Density in left cell
   * @param rho_right Density in right cell
   * @return Flux (vehicles/s)
   */
  double calculateFlux(double rho_left, double rho_right) const {
    double rho_c = getCriticalDensity();

    // Godunov flux (exact Riemann solver for LWR)
    if (rho_left <= rho_c && rho_right <= rho_c) {
      // Both in free-flow regime
      return std::min(flowFromDensity(rho_left), flowFromDensity(rho_right));
    } else if (rho_left >= rho_c && rho_right >= rho_c) {
      // Both in congested regime
      return std::min(flowFromDensity(rho_left), flowFromDensity(rho_right));
    } else if (rho_left < rho_c && rho_right > rho_c) {
      // Shock wave
      return flowFromDensity(rho_c);
    } else {
      // Rarefaction wave
      return flowFromDensity(rho_c);
    }
  }
};

} // namespace models
} // namespace macroscopic
} // namespace jamfree

#endif // JAMFREE_MACROSCOPIC_MODELS_LWR_H
