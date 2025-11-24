#ifndef JAMFREE_KERNEL_ROUTING_ROUTER_H
#define JAMFREE_KERNEL_ROUTING_ROUTER_H

#include "../model/Lane.h"
#include "../model/Point2D.h"
#include "../model/Road.h"
#include <limits>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace jamfree {
namespace kernel {
namespace routing {

/**
 * @brief Route between origin and destination.
 */
struct Route {
  std::vector<std::shared_ptr<model::Road>> roads;
  std::vector<int> lane_indices; // Which lane to use on each road
  double total_distance;
  double estimated_time;
  double cost; // Generalized cost (time + distance + ...)

  bool isEmpty() const { return roads.empty(); }
};

/**
 * @brief Origin-Destination pair.
 */
struct ODPair {
  model::Point2D origin;
  model::Point2D destination;
  std::string origin_id;
  std::string destination_id;

  // Trip characteristics
  double departure_time;       // Seconds since midnight
  double desired_arrival_time; // Seconds since midnight
  int trip_purpose;            // 0=work, 1=shopping, 2=leisure, etc.
};

/**
 * @brief Router for finding optimal paths.
 *
 * Uses A* algorithm with customizable cost functions.
 */
class Router {
public:
  /**
   * @brief Routing strategy.
   */
  enum class Strategy {
    SHORTEST_DISTANCE, // Minimize distance
    SHORTEST_TIME,     // Minimize travel time
    LEAST_COST,        // Minimize generalized cost
    AVOID_HIGHWAYS,    // Prefer local roads
    AVOID_TOLLS        // Avoid toll roads
  };

  /**
   * @brief Constructor.
   */
  Router() : m_strategy(Strategy::SHORTEST_TIME) {}

  /**
   * @brief Set routing strategy.
   */
  void setStrategy(Strategy strategy) { m_strategy = strategy; }

  /**
   * @brief Find route between origin and destination.
   *
   * @param origin Origin point
   * @param destination Destination point
   * @param roads Available roads
   * @param current_time Current time (for time-dependent routing)
   * @return Optimal route
   */
  Route findRoute(const model::Point2D &origin,
                  const model::Point2D &destination,
                  const std::vector<std::shared_ptr<model::Road>> &roads,
                  double current_time = 0.0);

  /**
   * @brief Find route with real-time traffic data.
   *
   * @param origin Origin point
   * @param destination Destination point
   * @param roads Available roads
   * @param traffic_speeds Current speeds on roads (road_id -> speed)
   * @return Optimal route considering current traffic
   */
  Route findRouteWithTraffic(
      const model::Point2D &origin, const model::Point2D &destination,
      const std::vector<std::shared_ptr<model::Road>> &roads,
      const std::unordered_map<std::string, double> &traffic_speeds);

  /**
   * @brief Reroute if current route is no longer optimal.
   *
   * @param current_route Current route
   * @param current_position Current position on route
   * @param destination Destination
   * @param roads Available roads
   * @param traffic_speeds Current traffic speeds
   * @return New route if rerouting recommended, empty route otherwise
   */
  Route rerouteIfNeeded(
      const Route &current_route, const model::Point2D &current_position,
      const model::Point2D &destination,
      const std::vector<std::shared_ptr<model::Road>> &roads,
      const std::unordered_map<std::string, double> &traffic_speeds);

private:
  Strategy m_strategy;

  /**
   * @brief Calculate cost for a road segment.
   */
  double calculateCost(const std::shared_ptr<model::Road> &road,
                       double current_speed, double current_time) const;

  /**
   * @brief Heuristic for A* (straight-line distance).
   */
  double heuristic(const model::Point2D &from, const model::Point2D &to) const;

  /**
   * @brief Build road graph for routing.
   */
  struct RoadNode {
    std::shared_ptr<model::Road> road;
    model::Point2D position;
    std::vector<std::shared_ptr<RoadNode>> neighbors;
    double g_cost; // Cost from start
    double h_cost; // Heuristic cost to goal
    double f_cost; // Total cost
    std::shared_ptr<RoadNode> parent;

    RoadNode(std::shared_ptr<model::Road> r, const model::Point2D &pos)
        : road(r), position(pos),
          g_cost(std::numeric_limits<double>::infinity()), h_cost(0.0),
          f_cost(std::numeric_limits<double>::infinity()), parent(nullptr) {}
  };
};

/**
 * @brief Origin-Destination Matrix.
 *
 * Stores trip demand between zones.
 */
class ODMatrix {
public:
  /**
   * @brief Add OD pair with demand.
   *
   * @param origin_zone Origin zone ID
   * @param dest_zone Destination zone ID
   * @param demand Number of trips
   * @param time_period Time period (hour of day)
   */
  void addDemand(const std::string &origin_zone, const std::string &dest_zone,
                 double demand, int time_period = 0);

  /**
   * @brief Get demand between zones.
   */
  double getDemand(const std::string &origin_zone, const std::string &dest_zone,
                   int time_period = 0) const;

  /**
   * @brief Sample OD pair based on demand distribution.
   *
   * @param time_period Time period
   * @return Random OD pair weighted by demand
   */
  ODPair sampleODPair(int time_period = 0) const;

  /**
   * @brief Load OD matrix from file.
   *
   * Supports formats: CSV, JSON, VISUM
   */
  bool loadFromFile(const std::string &filename);

  /**
   * @brief Generate synthetic OD matrix.
   *
   * Uses gravity model: T_ij = k * P_i * A_j / d_ij^2
   */
  void generateSynthetic(
      const std::unordered_map<std::string, double> &zone_populations,
      const std::unordered_map<std::string, double> &zone_attractions,
      const std::unordered_map<std::string, model::Point2D> &zone_centroids);

private:
  // OD matrix: origin -> destination -> time_period -> demand
  std::unordered_map<
      std::string,
      std::unordered_map<std::string, std::unordered_map<int, double>>>
      m_matrix;

  // Zone centroids for sampling
  std::unordered_map<std::string, model::Point2D> m_zone_centroids;
};

/**
 * @brief Trip generator using OD matrix.
 */
class TripGenerator {
public:
  TripGenerator(const ODMatrix &od_matrix, Router &router)
      : m_od_matrix(od_matrix), m_router(router) {}

  /**
   * @brief Generate trip for a vehicle.
   *
   * @param current_time Current simulation time
   * @param roads Available roads
   * @return OD pair and route
   */
  std::pair<ODPair, Route>
  generateTrip(double current_time,
               const std::vector<std::shared_ptr<model::Road>> &roads);

  /**
   * @brief Generate multiple trips for time period.
   *
   * @param time_period Time period (hour)
   * @param num_trips Number of trips to generate
   * @param roads Available roads
   * @return Vector of OD pairs and routes
   */
  std::vector<std::pair<ODPair, Route>>
  generateTrips(int time_period, int num_trips,
                const std::vector<std::shared_ptr<model::Road>> &roads);

private:
  const ODMatrix &m_od_matrix;
  Router &m_router;
};

} // namespace routing
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_ROUTING_ROUTER_H
