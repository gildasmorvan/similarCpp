#include "../include/OSMParser.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace jamfree {
namespace realdata {
namespace osm {

// Earth radius in meters
constexpr double EARTH_RADIUS = 6371000.0;

RoadNetwork OSMParser::parseFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return parseString(buffer.str());
}

RoadNetwork OSMParser::parseString(const std::string &xml_content) {
  RoadNetwork network;

  // Parse nodes
  parseNodes(xml_content, network);

  // Parse ways
  parseWays(xml_content, network);

  // Create roads from ways
  createRoads(network);

  return network;
}

kernel::model::Point2D OSMParser::latLonToMeters(double lat, double lon,
                                                 double center_lat,
                                                 double center_lon) {

  // Simple equirectangular projection
  double lat_rad = lat * M_PI / 180.0;
  double lon_rad = lon * M_PI / 180.0;
  double center_lat_rad = center_lat * M_PI / 180.0;
  double center_lon_rad = center_lon * M_PI / 180.0;

  double x =
      (lon_rad - center_lon_rad) * std::cos(center_lat_rad) * EARTH_RADIUS;
  double y = (lat_rad - center_lat_rad) * EARTH_RADIUS;

  return kernel::model::Point2D(x, y);
}

kernel::model::Point2D OSMParser::metersToLatLon(double x, double y,
                                                 double center_lat,
                                                 double center_lon) {
  double center_lat_rad = center_lat * M_PI / 180.0;
  double center_lon_rad = center_lon * M_PI / 180.0;

  double lat_rad = y / EARTH_RADIUS + center_lat_rad;
  double lon_rad =
      x / (std::cos(center_lat_rad) * EARTH_RADIUS) + center_lon_rad;

  double lat = lat_rad * 180.0 / M_PI;
  double lon = lon_rad * 180.0 / M_PI;

  return kernel::model::Point2D(lat, lon);
}

int OSMParser::getDefaultLanes(const std::string &highway_type) {
  if (highway_type == "motorway")
    return 3;
  if (highway_type == "motorway_link")
    return 1;
  if (highway_type == "trunk")
    return 2;
  if (highway_type == "trunk_link")
    return 1;
  if (highway_type == "primary")
    return 2;
  if (highway_type == "primary_link")
    return 1;
  if (highway_type == "secondary")
    return 2;
  if (highway_type == "tertiary")
    return 1;
  if (highway_type == "residential")
    return 1;
  return 1;
}

double OSMParser::getDefaultSpeedLimit(const std::string &highway_type,
                                       const std::string &country) {

  // France defaults
  if (country == "FR") {
    if (highway_type == "motorway")
      return 130.0;
    if (highway_type == "trunk")
      return 110.0;
    if (highway_type == "primary")
      return 90.0;
    if (highway_type == "secondary")
      return 80.0;
    if (highway_type == "tertiary")
      return 80.0;
    if (highway_type == "residential")
      return 50.0;
  }

  // Generic defaults
  if (highway_type == "motorway")
    return 120.0;
  if (highway_type == "trunk")
    return 100.0;
  if (highway_type == "primary")
    return 80.0;
  if (highway_type == "secondary")
    return 60.0;
  return 50.0;
}

void OSMParser::parseNodes(const std::string &xml, RoadNetwork &network) {
  // Simple XML parsing (in production, use a proper XML library)
  size_t pos = 0;

  network.min_lat = 90.0;
  network.max_lat = -90.0;
  network.min_lon = 180.0;
  network.max_lon = -180.0;

  while ((pos = xml.find("<node", pos)) != std::string::npos) {
    size_t end = xml.find("/>", pos);
    if (end == std::string::npos) {
      end = xml.find("</node>", pos);
    }
    if (end == std::string::npos)
      break;

    std::string node_str = xml.substr(pos, end - pos);

    OSMNode node;

    // Extract id
    size_t id_pos = node_str.find("id=\"");
    if (id_pos != std::string::npos) {
      id_pos += 4;
      size_t id_end = node_str.find("\"", id_pos);
      node.id = std::stoll(node_str.substr(id_pos, id_end - id_pos));
    }

    // Extract lat
    size_t lat_pos = node_str.find("lat=\"");
    if (lat_pos != std::string::npos) {
      lat_pos += 5;
      size_t lat_end = node_str.find("\"", lat_pos);
      node.lat = std::stod(node_str.substr(lat_pos, lat_end - lat_pos));

      network.min_lat = std::min(network.min_lat, node.lat);
      network.max_lat = std::max(network.max_lat, node.lat);
    }

    // Extract lon
    size_t lon_pos = node_str.find("lon=\"");
    if (lon_pos != std::string::npos) {
      lon_pos += 5;
      size_t lon_end = node_str.find("\"", lon_pos);
      node.lon = std::stod(node_str.substr(lon_pos, lon_end - lon_pos));

      network.min_lon = std::min(network.min_lon, node.lon);
      network.max_lon = std::max(network.max_lon, node.lon);
    }

    network.nodes[node.id] = node;
    pos = end;
  }
}

void OSMParser::parseWays(const std::string &xml, RoadNetwork &network) {
  size_t pos = 0;

  while ((pos = xml.find("<way", pos)) != std::string::npos) {
    size_t end = xml.find("</way>", pos);
    if (end == std::string::npos)
      break;

    std::string way_str = xml.substr(pos, end - pos);

    OSMWay way;

    // Extract id
    size_t id_pos = way_str.find("id=\"");
    if (id_pos != std::string::npos) {
      id_pos += 4;
      size_t id_end = way_str.find("\"", id_pos);
      way.id = std::stoll(way_str.substr(id_pos, id_end - id_pos));
    }

    // Extract node references
    size_t nd_pos = 0;
    while ((nd_pos = way_str.find("<nd ref=\"", nd_pos)) != std::string::npos) {
      nd_pos += 9;
      size_t nd_end = way_str.find("\"", nd_pos);
      long long node_id = std::stoll(way_str.substr(nd_pos, nd_end - nd_pos));
      way.node_refs.push_back(node_id);
      nd_pos = nd_end;
    }

    // Extract tags
    size_t tag_pos = 0;
    while ((tag_pos = way_str.find("<tag k=\"", tag_pos)) !=
           std::string::npos) {
      tag_pos += 8;
      size_t key_end = way_str.find("\"", tag_pos);
      std::string key = way_str.substr(tag_pos, key_end - tag_pos);

      size_t val_pos = way_str.find("v=\"", key_end);
      if (val_pos != std::string::npos) {
        val_pos += 3;
        size_t val_end = way_str.find("\"", val_pos);
        std::string value = way_str.substr(val_pos, val_end - val_pos);
        way.tags[key] = value;
      }

      tag_pos = key_end;
    }

    // Extract way attributes
    extractWayAttributes(way);

    // Only add if it's a road
    if (!way.highway_type.empty()) {
      network.ways.push_back(way);
    }

    pos = end;
  }
}

void OSMParser::extractWayAttributes(OSMWay &way) {
  // Highway type
  auto highway_it = way.tags.find("highway");
  if (highway_it != way.tags.end()) {
    way.highway_type = highway_it->second;
  }

  // Lanes
  auto lanes_it = way.tags.find("lanes");
  if (lanes_it != way.tags.end()) {
    way.lanes = std::stoi(lanes_it->second);
  } else {
    way.lanes = getDefaultLanes(way.highway_type);
  }

  // Max speed
  auto maxspeed_it = way.tags.find("maxspeed");
  if (maxspeed_it != way.tags.end()) {
    way.max_speed = std::stod(maxspeed_it->second);
  } else {
    way.max_speed = getDefaultSpeedLimit(way.highway_type);
  }

  // Oneway
  auto oneway_it = way.tags.find("oneway");
  if (oneway_it != way.tags.end()) {
    way.oneway = (oneway_it->second == "yes");
  }

  // Name
  auto name_it = way.tags.find("name");
  if (name_it != way.tags.end()) {
    way.name = name_it->second;
  }
}

void OSMParser::createRoads(RoadNetwork &network) {
  // Calculate center for coordinate conversion
  double center_lat = (network.min_lat + network.max_lat) / 2.0;
  double center_lon = (network.min_lon + network.max_lon) / 2.0;

  // Create roads from ways
  for (const auto &way : network.ways) {
    if (way.node_refs.size() < 2)
      continue;

    // Get start and end nodes
    auto start_it = network.nodes.find(way.node_refs.front());
    auto end_it = network.nodes.find(way.node_refs.back());

    if (start_it == network.nodes.end() || end_it == network.nodes.end()) {
      continue;
    }

    // Convert to local coordinates
    auto start_pos = latLonToMeters(start_it->second.lat, start_it->second.lon,
                                    center_lat, center_lon);

    auto end_pos = latLonToMeters(end_it->second.lat, end_it->second.lon,
                                  center_lat, center_lon);

    // Create road
    std::string road_id = "osm_way_" + std::to_string(way.id);
    auto road = std::make_shared<kernel::model::Road>(road_id, start_pos,
                                                      end_pos, way.lanes,
                                                      3.5 // Default lane width
    );

    // Set speed limit (convert km/h to m/s)
    for (int i = 0; i < way.lanes; i++) {
      auto lane = road->getLane(i);
      lane->setSpeedLimit(way.max_speed / 3.6);
    }

    network.roads.push_back(road);
  }
}

} // namespace osm
} // namespace realdata
} // namespace jamfree
