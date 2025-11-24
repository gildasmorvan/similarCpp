"""
Real-time traffic data source integrations for JamFree.

This module provides interfaces to various traffic data APIs:
- TomTom Traffic API
- HERE Traffic API
- Google Maps Traffic
- OpenTraffic
- DATEX II (Europe)
- PeMS (California)
"""

import requests
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from datetime import datetime
import json

@dataclass
class TrafficData:
    """Real-time traffic data point."""
    speed: float  # km/h
    flow: Optional[float] = None  # vehicles/hour
    density: Optional[float] = None  # vehicles/km
    occupancy: Optional[float] = None  # percentage
    timestamp: Optional[datetime] = None
    location: Optional[Tuple[float, float]] = None  # (lat, lon)
    road_id: Optional[str] = None
    source: str = "unknown"

class TrafficDataSource:
    """Base class for traffic data sources."""
    
    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key
    
    def get_traffic(self, lat: float, lon: float, radius: float = 1000) -> List[TrafficData]:
        """Get traffic data for a location."""
        raise NotImplementedError

class TomTomTraffic(TrafficDataSource):
    """TomTom Traffic API integration.
    
    API: https://developer.tomtom.com/traffic-api
    Free tier: 2,500 requests/day
    """
    
    BASE_URL = "https://api.tomtom.com/traffic/services/4"
    
    def get_traffic(self, lat: float, lon: float, radius: float = 1000) -> List[TrafficData]:
        """Get traffic flow data."""
        if not self.api_key:
            raise ValueError("TomTom API key required")
        
        url = f"{self.BASE_URL}/flowSegmentData/absolute/10/json"
        params = {
            'key': self.api_key,
            'point': f"{lat},{lon}",
            'unit': 'KMPH'
        }
        
        try:
            response = requests.get(url, params=params, timeout=10)
            response.raise_for_status()
            data = response.json()
            
            flow_data = data.get('flowSegmentData', {})
            
            return [TrafficData(
                speed=flow_data.get('currentSpeed', 0),
                flow=flow_data.get('currentTravelTime'),
                timestamp=datetime.now(),
                location=(lat, lon),
                source='tomtom'
            )]
        except Exception as e:
            print(f"TomTom API error: {e}")
            return []

class HERETraffic(TrafficDataSource):
    """HERE Traffic API integration.
    
    API: https://developer.here.com/products/traffic
    Free tier: 250,000 requests/month
    """
    
    BASE_URL = "https://data.traffic.hereapi.com/v7"
    
    def get_traffic(self, lat: float, lon: float, radius: float = 1000) -> List[TrafficData]:
        """Get traffic flow data."""
        if not self.api_key:
            raise ValueError("HERE API key required")
        
        url = f"{self.BASE_URL}/flow"
        params = {
            'apiKey': self.api_key,
            'locationReferencing': 'shape',
            'in': f'circle:{lat},{lon};r={radius}'
        }
        
        try:
            response = requests.get(url, params=params, timeout=10)
            response.raise_for_status()
            data = response.json()
            
            results = []
            for result in data.get('results', []):
                current_flow = result.get('currentFlow', {})
                results.append(TrafficData(
                    speed=current_flow.get('speed', 0),
                    flow=current_flow.get('speedUncapped'),
                    timestamp=datetime.now(),
                    source='here'
                ))
            
            return results
        except Exception as e:
            print(f"HERE API error: {e}")
            return []

class OpenTraffic(TrafficDataSource):
    """OpenTraffic integration.
    
    API: http://opentraffic.io/
    Open source, free to use
    """
    
    BASE_URL = "https://api.opentraffic.io/v2"
    
    def get_traffic(self, lat: float, lon: float, radius: float = 1000) -> List[TrafficData]:
        """Get traffic data from OpenTraffic."""
        # Note: OpenTraffic API may have limited availability
        url = f"{self.BASE_URL}/segments"
        params = {
            'lat': lat,
            'lon': lon,
            'radius': radius
        }
        
        try:
            response = requests.get(url, params=params, timeout=10)
            response.raise_for_status()
            data = response.json()
            
            results = []
            for segment in data.get('segments', []):
                results.append(TrafficData(
                    speed=segment.get('speed', 0),
                    timestamp=datetime.now(),
                    road_id=segment.get('segment_id'),
                    source='opentraffic'
                ))
            
            return results
        except Exception as e:
            print(f"OpenTraffic API error: {e}")
            return []

class PeMSData(TrafficDataSource):
    """PeMS (Performance Measurement System) - California traffic data.
    
    API: http://pems.dot.ca.gov/
    Free, requires registration
    """
    
    BASE_URL = "http://pems.dot.ca.gov/api"
    
    def __init__(self, username: str, password: str):
        super().__init__()
        self.username = username
        self.password = password
    
    def get_traffic(self, station_id: int) -> List[TrafficData]:
        """Get traffic data from PeMS station."""
        # PeMS uses station IDs rather than lat/lon
        url = f"{self.BASE_URL}/station/{station_id}/data"
        
        try:
            response = requests.get(
                url,
                auth=(self.username, self.password),
                timeout=10
            )
            response.raise_for_status()
            data = response.json()
            
            results = []
            for record in data.get('data', []):
                results.append(TrafficData(
                    speed=record.get('speed', 0),
                    flow=record.get('flow', 0),
                    occupancy=record.get('occupancy', 0),
                    timestamp=datetime.fromisoformat(record.get('timestamp')),
                    source='pems'
                ))
            
            return results
        except Exception as e:
            print(f"PeMS API error: {e}")
            return []

class DATEX2(TrafficDataSource):
    """DATEX II - European traffic data standard.
    
    Various European countries provide DATEX II feeds.
    Example: https://www.datex2.eu/
    """
    
    def __init__(self, feed_url: str):
        super().__init__()
        self.feed_url = feed_url
    
    def get_traffic(self) -> List[TrafficData]:
        """Get traffic data from DATEX II feed."""
        try:
            response = requests.get(self.feed_url, timeout=10)
            response.raise_for_status()
            
            # DATEX II is XML format
            # Would need XML parsing here
            # This is a simplified example
            
            return []
        except Exception as e:
            print(f"DATEX II error: {e}")
            return []

class TrafficDataAggregator:
    """Aggregate data from multiple sources."""
    
    def __init__(self):
        self.sources: Dict[str, TrafficDataSource] = {}
    
    def add_source(self, name: str, source: TrafficDataSource):
        """Add a data source."""
        self.sources[name] = source
    
    def get_all_traffic(self, lat: float, lon: float, radius: float = 1000) -> Dict[str, List[TrafficData]]:
        """Get traffic data from all sources."""
        results = {}
        
        for name, source in self.sources.items():
            try:
                data = source.get_traffic(lat, lon, radius)
                results[name] = data
            except Exception as e:
                print(f"Error from {name}: {e}")
                results[name] = []
        
        return results
    
    def get_average_speed(self, lat: float, lon: float, radius: float = 1000) -> Optional[float]:
        """Get average speed from all sources."""
        all_data = self.get_all_traffic(lat, lon, radius)
        
        speeds = []
        for source_data in all_data.values():
            speeds.extend([d.speed for d in source_data if d.speed > 0])
        
        if speeds:
            return sum(speeds) / len(speeds)
        return None

# Example usage and configuration
def create_aggregator(config: Dict[str, str]) -> TrafficDataAggregator:
    """Create traffic data aggregator with configured sources."""
    aggregator = TrafficDataAggregator()
    
    # Add TomTom if API key provided
    if 'tomtom_key' in config:
        aggregator.add_source('tomtom', TomTomTraffic(config['tomtom_key']))
    
    # Add HERE if API key provided
    if 'here_key' in config:
        aggregator.add_source('here', HERETraffic(config['here_key']))
    
    # Add OpenTraffic (no key needed)
    aggregator.add_source('opentraffic', OpenTraffic())
    
    return aggregator

# Data source information for documentation
DATA_SOURCES_INFO = {
    'tomtom': {
        'name': 'TomTom Traffic API',
        'url': 'https://developer.tomtom.com/traffic-api',
        'free_tier': '2,500 requests/day',
        'coverage': 'Global',
        'features': ['speed', 'flow', 'incidents'],
        'signup': 'https://developer.tomtom.com/user/register'
    },
    'here': {
        'name': 'HERE Traffic API',
        'url': 'https://developer.here.com/products/traffic',
        'free_tier': '250,000 requests/month',
        'coverage': 'Global',
        'features': ['speed', 'flow', 'incidents'],
        'signup': 'https://developer.here.com/sign-up'
    },
    'google': {
        'name': 'Google Maps Roads API',
        'url': 'https://developers.google.com/maps/documentation/roads',
        'free_tier': '$200 credit/month',
        'coverage': 'Global',
        'features': ['speed', 'congestion'],
        'signup': 'https://console.cloud.google.com/'
    },
    'opentraffic': {
        'name': 'OpenTraffic',
        'url': 'http://opentraffic.io/',
        'free_tier': 'Free, open source',
        'coverage': 'Limited',
        'features': ['speed', 'flow'],
        'signup': 'No signup required'
    },
    'pems': {
        'name': 'PeMS (California)',
        'url': 'http://pems.dot.ca.gov/',
        'free_tier': 'Free with registration',
        'coverage': 'California, USA',
        'features': ['speed', 'flow', 'occupancy'],
        'signup': 'http://pems.dot.ca.gov/user/register'
    },
    'datex2': {
        'name': 'DATEX II (Europe)',
        'url': 'https://www.datex2.eu/',
        'free_tier': 'Varies by country',
        'coverage': 'Europe',
        'features': ['incidents', 'roadworks', 'conditions'],
        'signup': 'Varies by national provider'
    }
}
