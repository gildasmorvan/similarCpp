#ifndef JAMFREE_REALDATA_OSM_PARSER_H
#define JAMFREE_REALDATA_OSM_PARSER_H

#include "../../kernel/include/model/Lane.h"
#include "../../kernel/include/model/Point2D.h"
#include "../../kernel/include/model/Road.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace realdata {
namespace osm {

/**
 * @brief OSM Node - represents a point in the map
 */
struct OSMNode {
  long long id;
  double lat;
  double lon;
  std::map<std::string, std::string> tags;
};

/**
 * @brief OSM Way - represents a road or path
 */
struct OSMWay {
  long long id;
  std::vector<long long> node_refs;
  std::map<std::string, std::string> tags;

  // Parsed attributes
  std::string highway_type; // motorway, primary, secondary, etc.
  int lanes = 1;
  double max_speed = 50.0; // km/h
  bool oneway = false;
  std::string name;
};

/**
 * @brief Road network extracted from OSM data
 */
struct RoadNetwork {
  std::map<long long, OSMNode> nodes;
  std::vector<OSMWay> ways;
  std::vector<std::shared_ptr<kernel::model::Road>> roads;

  // Bounding box
  double min_lat, max_lat;
  double min_lon, max_lon;
};

/**
 * @brief Parser for OpenStreetMap XML files
 */
class OSMParser {
public:
  /**
   * @brief Parse OSM XML file
   *
   * @param filename Path to .osm file
   * @return Parsed road network
   */
  static RoadNetwork parseFile(const std::string &filename);

  /**
   * @brief Parse OSM XML string
   *
   * @param xml_content OSM XML content
   * @return Parsed road network
   */
  static RoadNetwork parseString(const std::string &xml_content);

  /**
   * @brief Convert lat/lon to local coordinates
   *
   * Uses simple equirectangular projection centered on the network.
   *
   * @param lat Latitude
   * @param lon Longitude
   * @param center_lat Center latitude
   * @param center_lon Center longitude
   * @return Point in meters
   */
  static kernel::model::Point2D
  latLonToMeters(double lat, double lon, double center_lat, double center_lon);

  /**
   * @brief Convert local meters to lat/lon
   *
   * Inverse of latLonToMeters.
   *
   * @param x X coordinate (meters)
   * @param y Y coordinate (meters)
   * @param center_lat Center latitude
   * @param center_lon Center longitude
   * @return Point2D where x=lat, y=lon
   */
  static kernel::model::Point2D
  metersToLatLon(double x, double y, double center_lat, double center_lon);

  /**
   * @brief Get default lane count for highway type
   *
   * @param highway_type OSM highway type
   * @return Number of lanes
   */
  static int getDefaultLanes(const std::string &highway_type);

  /**
   * @brief Get default speed limit for highway type
   *
   * @param highway_type OSM highway type
   * @param country Country code (default: "FR")
   * @return Speed limit in km/h
   */
  static double getDefaultSpeedLimit(const std::string &highway_type,
                                     const std::string &country = "FR");

private:
  static void parseNodes(const std::string &xml, RoadNetwork &network);
  static void parseWays(const std::string &xml, RoadNetwork &network);
  static void extractWayAttributes(OSMWay &way);
  static void createRoads(RoadNetwork &network);
};

/**
 * @brief Filter for selecting roads from OSM data
 */
class RoadFilter {
public:
  /**
   * @brief Check if a way should be included
   *
   * @param way OSM way
   * @return True if way should be included
   */
  virtual bool accept(const OSMWay &way) const = 0;

  virtual ~RoadFilter() = default;
};

/**
 * @brief Filter by highway type
 */
class HighwayTypeFilter : public RoadFilter {
public:
  HighwayTypeFilter(const std::vector<std::string> &types) : m_types(types) {}

  bool accept(const OSMWay &way) const override {
    for (const auto &type : m_types) {
      if (way.highway_type == type) {
        return true;
      }
    }
    return false;
  }

private:
  std::vector<std::string> m_types;
};

/**
 * @brief Filter motorways only
 */
class MotorwayFilter : public HighwayTypeFilter {
public:
  MotorwayFilter()
      : HighwayTypeFilter(
            {"motorway", "motorway_link", "trunk", "trunk_link"}) {}
};

/**
 * @brief Filter major roads
 */
class MajorRoadsFilter : public HighwayTypeFilter {
public:
  MajorRoadsFilter()
      : HighwayTypeFilter({"motorway", "motorway_link", "trunk", "trunk_link",
                           "primary", "primary_link", "secondary",
                           "secondary_link"}) {}
};

} // namespace osm
} // namespace realdata
} // namespace jamfree

#endif // JAMFREE_REALDATA_OSM_PARSER_H
