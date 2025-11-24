#ifndef ENGINESTATE_H
#define ENGINESTATE_H

#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {
namespace control {

/**
 * Represents the different states of the simulation engine.
 */
enum class EngineState {
  /** The engine is idle and ready to start a new simulation */
  IDLE,
  /** A simulation run has been requested but not yet started */
  RUN_PLANNED,
  /** The simulation is being initialized */
  INITIALIZING,
  /** The simulation is currently running */
  RUN,
  /** The simulation is paused */
  PAUSED,
  /** An abortion of the simulation has been requested */
  ABORT_REQUESTED,
  /** The simulation is being aborted */
  ABORTING,
  /** A shutdown of the server has been requested */
  SHUTDOWN_REQUESTED,
  /** The engine is inactive (after error or shutdown) */
  INACTIVE
};

/**
 * Utility class for EngineState operations.
 */
class EngineStateUtil {
public:
  /**
   * Checks if the engine can start a new simulation run.
   */
  static bool allowsNewRun(EngineState state) {
    return state == EngineState::IDLE;
  }

  /**
   * Checks if the engine can be paused or resumed.
   */
  static bool allowsPause(EngineState state) {
    return state == EngineState::RUN || state == EngineState::PAUSED;
  }

  /**
   * Checks if the simulation can be aborted.
   */
  static bool allowsAbort(EngineState state) {
    return state == EngineState::RUN || state == EngineState::PAUSED ||
           state == EngineState::INITIALIZING;
  }

  /**
   * Checks if the server can be shut down (ejected).
   */
  static bool allowsEject(EngineState state) {
    return state == EngineState::IDLE || state == EngineState::RUN ||
           state == EngineState::PAUSED;
  }

  /**
   * Checks if the simulation is in an aborting state.
   */
  static bool isAborting(EngineState state) {
    return state == EngineState::ABORT_REQUESTED ||
           state == EngineState::ABORTING;
  }

  /**
   * Checks if the server is shutting down.
   */
  static bool isShuttingDown(EngineState state) {
    return state == EngineState::SHUTDOWN_REQUESTED;
  }

  /**
   * Converts engine state to string.
   */
  static std::string toString(EngineState state) {
    switch (state) {
    case EngineState::IDLE:
      return "IDLE";
    case EngineState::RUN_PLANNED:
      return "RUN_PLANNED";
    case EngineState::INITIALIZING:
      return "INITIALIZING";
    case EngineState::RUN:
      return "RUN";
    case EngineState::PAUSED:
      return "PAUSED";
    case EngineState::ABORT_REQUESTED:
      return "ABORT_REQUESTED";
    case EngineState::ABORTING:
      return "ABORTING";
    case EngineState::SHUTDOWN_REQUESTED:
      return "SHUTDOWN_REQUESTED";
    case EngineState::INACTIVE:
      return "INACTIVE";
    default:
      return "UNKNOWN";
    }
  }
};

} // namespace control
} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ENGINESTATE_H
