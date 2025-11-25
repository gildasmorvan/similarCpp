#ifndef JAMFREE_KERNEL_LEVELS_LEVEL_IDENTIFIERS_H
#define JAMFREE_KERNEL_LEVELS_LEVEL_IDENTIFIERS_H

#include "../agents/VehicleAgent.h"

namespace jamfree {
namespace kernel {
namespace levels {

/**
 * @brief Standard level identifiers for JamFree simulation.
 *
 * Following SIMILAR's multi-level architecture:
 * - MICROSCOPIC: Individual vehicle tracking (IDM, MOBIL)
 * - MACROSCOPIC: Continuum flow models (LWR, CTM)
 * - CONTROL: High-level control and coordination
 */
class LevelIdentifiers {
public:
  /**
   * @brief Microscopic level.
   *
   * Individual vehicle tracking with:
   * - IDM car-following
   * - MOBIL lane-changing
   * - Detailed collision detection
   */
  static const agents::LevelIdentifier MICROSCOPIC;

  /**
   * @brief Macroscopic level.
   *
   * Continuum flow models:
   * - LWR (Lighthill-Whitham-Richards)
   * - CTM (Cell Transmission Model)
   * - Density-based flow
   */
  static const agents::LevelIdentifier MACROSCOPIC;

  /**
   * @brief Control level.
   *
   * High-level coordination:
   * - Traffic signal control
   * - Ramp metering
   * - Variable speed limits
   * - Route guidance
   */
  static const agents::LevelIdentifier CONTROL;

private:
  // Prevent instantiation
  LevelIdentifiers() = delete;
};

} // namespace levels
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_LEVELS_LEVEL_IDENTIFIERS_H
