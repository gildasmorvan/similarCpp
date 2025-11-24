#ifndef JAMFREE_REALDATA_TRAFFIC_DATA_SOURCE_H
#define JAMFREE_REALDATA_TRAFFIC_DATA_SOURCE_H

#include "../kernel/include/model/Point2D.h"
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace jamfree {
namespace realdata {

/**
 * @brief Time of day representation.
 */
struct DateTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int day_of_week; // 0=Sunday, 6=Saturday

  /**
   * @brief Get seconds since midnight.
   */
  double getSecondsFromMidnight() const {
    return hour * 3600.0 + minute * 60.0 + second;
  }

  /**
   * @brief Check if rush hour.
   */
  bool isRushHour() const {
    return (hour >= 7 && hour <= 9) || (hour >= 17 && hour <= 19);
  }

  /**
   * @brief Check if weekend.
   */
  bool isWeekend() const { return day_of_week == 0 || day_of_week == 6; }

  /**
   * @brief Create from current system time.
   */
  static DateTime now();

  /**
   * @brief Create from timestamp.
   */
  static DateTime fromTimestamp(time_t timestamp);
};

/**
 * @brief Real-time traffic speed data.
 */
struct TrafficSpeedData {
  std::string road_id;
  double speed_kmh;
  double free_flow_speed_kmh;
  double congestion_level; // 0.0 = free flow, 1.0 = jammed
  DateTime timestamp;
  std::string source; // "google", "tomtom", "here", "estimated"
};

/**
 * @brief Traffic incident data.
 */
struct TrafficIncident {
  std::string id;
  kernel::model::Point2D location;
  std::string type;     // "accident", "construction", "closure"
  std::string severity; // "minor", "moderate", "severe"
  DateTime start_time;
  DateTime end_time;
  std::vector<std::string> affected_roads;
};

/**
 * @brief Weather data.
 */
struct WeatherData {
  DateTime timestamp;
  std::string condition; // "clear", "rain", "snow", "fog"
  double temperature_celsius;
  double precipitation_mm;
  double visibility_km;
  double wind_speed_kmh;
};

/**
 * @brief Base class for traffic data sources.
 */
class TrafficDataSource {
public:
  virtual ~TrafficDataSource() = default;

  /**
   * @brief Get current traffic speeds.
   *
   * @param bounds Bounding box for query
   * @return Map of road_id -> speed data
   */
  virtual std::unordered_map<std::string, TrafficSpeedData>
  getTrafficSpeeds(const kernel::model::Point2D &min_point,
                   const kernel::model::Point2D &max_point) = 0;

  /**
   * @brief Get traffic incidents.
   */
  virtual std::vector<TrafficIncident>
  getIncidents(const kernel::model::Point2D &min_point,
               const kernel::model::Point2D &max_point) = 0;

  /**
   * @brief Get weather data.
   */
  virtual WeatherData getWeather(const kernel::model::Point2D &location) = 0;

  /**
   * @brief Check if data source is available.
   */
  virtual bool isAvailable() const = 0;
};

/**
 * @brief Google Maps traffic data source.
 */
class GoogleMapsDataSource : public TrafficDataSource {
public:
  GoogleMapsDataSource(const std::string &api_key) : m_api_key(api_key) {}

  std::unordered_map<std::string, TrafficSpeedData>
  getTrafficSpeeds(const kernel::model::Point2D &min_point,
                   const kernel::model::Point2D &max_point) override;

  std::vector<TrafficIncident>
  getIncidents(const kernel::model::Point2D &min_point,
               const kernel::model::Point2D &max_point) override;

  WeatherData getWeather(const kernel::model::Point2D &location) override;

  bool isAvailable() const override { return !m_api_key.empty(); }

private:
  std::string m_api_key;
};

/**
 * @brief TomTom traffic data source.
 */
class TomTomDataSource : public TrafficDataSource {
public:
  TomTomDataSource(const std::string &api_key) : m_api_key(api_key) {}

  std::unordered_map<std::string, TrafficSpeedData>
  getTrafficSpeeds(const kernel::model::Point2D &min_point,
                   const kernel::model::Point2D &max_point) override;

  std::vector<TrafficIncident>
  getIncidents(const kernel::model::Point2D &min_point,
               const kernel::model::Point2D &max_point) override;

  WeatherData getWeather(const kernel::model::Point2D &location) override;

  bool isAvailable() const override { return !m_api_key.empty(); }

private:
  std::string m_api_key;
};

/**
 * @brief Estimated traffic data based on time patterns.
 *
 * Uses historical patterns when real data is unavailable.
 */
class EstimatedDataSource : public TrafficDataSource {
public:
  EstimatedDataSource() = default;

  std::unordered_map<std::string, TrafficSpeedData>
  getTrafficSpeeds(const kernel::model::Point2D &min_point,
                   const kernel::model::Point2D &max_point) override;

  std::vector<TrafficIncident>
  getIncidents(const kernel::model::Point2D &min_point,
               const kernel::model::Point2D &max_point) override;

  WeatherData getWeather(const kernel::model::Point2D &location) override;

  bool isAvailable() const override { return true; }

  /**
   * @brief Estimate speed based on time of day and road type.
   *
   * @param free_flow_speed Free-flow speed (km/h)
   * @param road_type Road type ("motorway", "primary", "residential")
   * @param time Current time
   * @return Estimated speed (km/h)
   */
  double estimateSpeed(double free_flow_speed, const std::string &road_type,
                       const DateTime &time) const;

  /**
   * @brief Estimate congestion level.
   *
   * @param road_type Road type
   * @param time Current time
   * @return Congestion level (0.0-1.0)
   */
  double estimateCongestion(const std::string &road_type,
                            const DateTime &time) const;

private:
  /**
   * @brief Get rush hour factor.
   */
  double getRushHourFactor(const DateTime &time) const;

  /**
   * @brief Get road type capacity factor.
   */
  double getRoadTypeCapacity(const std::string &road_type) const;
};

/**
 * @brief Traffic data manager with fallback sources.
 */
class TrafficDataManager {
public:
  /**
   * @brief Add data source with priority.
   *
   * Lower priority number = higher priority.
   */
  void addSource(std::shared_ptr<TrafficDataSource> source, int priority = 0);

  /**
   * @brief Get traffic speeds from best available source.
   */
  std::unordered_map<std::string, TrafficSpeedData>
  getTrafficSpeeds(const kernel::model::Point2D &min_point,
                   const kernel::model::Point2D &max_point);

  /**
   * @brief Get incidents from best available source.
   */
  std::vector<TrafficIncident>
  getIncidents(const kernel::model::Point2D &min_point,
               const kernel::model::Point2D &max_point);

  /**
   * @brief Get weather from best available source.
   */
  WeatherData getWeather(const kernel::model::Point2D &location);

  /**
   * @brief Set current simulation time.
   */
  void setCurrentTime(const DateTime &time) { m_current_time = time; }

  /**
   * @brief Get current simulation time.
   */
  const DateTime &getCurrentTime() const { return m_current_time; }

private:
  struct SourceEntry {
    std::shared_ptr<TrafficDataSource> source;
    int priority;

    bool operator<(const SourceEntry &other) const {
      return priority < other.priority;
    }
  };

  std::vector<SourceEntry> m_sources;
  DateTime m_current_time;
};

} // namespace realdata
} // namespace jamfree

#endif // JAMFREE_REALDATA_TRAFFIC_DATA_SOURCE_H
