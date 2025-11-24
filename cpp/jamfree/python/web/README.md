# JamFree Web UI

Beautiful web interface for JamFree traffic simulation with real-time data integration.

## Features

- 🗺️ **Interactive Map** - Leaflet-based map visualization
- 📍 **OSM Integration** - Load road networks from OpenStreetMap
- 🚗 **Traffic Simulation** - Real-time vehicle simulation
- 📊 **Data Sources** - Integration with multiple traffic data APIs
- 🎨 **Modern UI** - Beautiful, responsive design

## Installation

### Prerequisites

1. **Build JamFree Python bindings**:
   ```bash
   cd ../../
   ./build_python.sh
   ```

2. **Install web dependencies**:
   ```bash
   cd python/web
   pip install -r requirements.txt
   ```

## Running the Web UI

```bash
python app.py
```

Then open your browser to: **http://localhost:5000**

## Usage

### 1. Load Road Network

**Option A: Upload OSM File**
- Download OSM data from https://www.openstreetmap.org/export
- Click "Upload" and select your .osm file

**Option B: Download from Map**
- Navigate to your area of interest on the map
- Click "Download Area"
- Confirm the bounding box

### 2. Configure Simulation

- Set number of vehicles
- Adjust desired speed
- Configure IDM/MOBIL parameters

### 3. Run Simulation

- Click "▶️ Start" to begin
- Watch vehicles move on the map
- Click "⏸️ Stop" to pause

## Real Traffic Data Sources

The web UI integrates with multiple real-time traffic data sources:

### Commercial APIs (Require API Keys)

#### TomTom Traffic API
- **Coverage**: Global
- **Free Tier**: 2,500 requests/day
- **Features**: Speed, flow, incidents
- **Signup**: https://developer.tomtom.com/user/register

#### HERE Traffic API
- **Coverage**: Global
- **Free Tier**: 250,000 requests/month
- **Features**: Speed, flow, incidents
- **Signup**: https://developer.here.com/sign-up

#### Google Maps Roads API
- **Coverage**: Global
- **Free Tier**: $200 credit/month
- **Features**: Speed, congestion
- **Signup**: https://console.cloud.google.com/

### Open/Government Sources

#### OpenTraffic
- **Coverage**: Limited
- **Free Tier**: Free, open source
- **Features**: Speed, flow
- **URL**: http://opentraffic.io/

#### PeMS (California)
- **Coverage**: California, USA
- **Free Tier**: Free with registration
- **Features**: Speed, flow, occupancy
- **URL**: http://pems.dot.ca.gov/

#### DATEX II (Europe)
- **Coverage**: Europe
- **Free Tier**: Varies by country
- **Features**: Incidents, roadworks, conditions
- **URL**: https://www.datex2.eu/

## Configuring Data Sources

Create a `config.json` file:

```json
{
  "tomtom_key": "YOUR_TOMTOM_API_KEY",
  "here_key": "YOUR_HERE_API_KEY",
  "google_key": "YOUR_GOOGLE_API_KEY"
}
```

Then update `app.py` to load the configuration:

```python
import json

# Load config
with open('config.json') as f:
    config = json.load(f)

# Initialize data sources
from traffic_data import create_aggregator
aggregator = create_aggregator(config)
```

## API Endpoints

### Network Management

- `GET /api/status` - Get simulation status
- `POST /api/osm/upload` - Upload OSM file
- `POST /api/osm/download` - Download OSM data from Overpass API

### Simulation Control

- `POST /api/simulation/start` - Start simulation
- `POST /api/simulation/stop` - Stop simulation
- `POST /api/simulation/step` - Execute one step

### Data Sources

- `GET /api/data/sources` - List available data sources
- `GET /api/data/traffic?source=tomtom&location=paris` - Get real-time traffic data

### Configuration

- `GET /api/config` - Get simulation configuration
- `POST /api/config` - Update configuration

## Architecture

```
python/web/
├── app.py                  # Flask application
├── traffic_data.py         # Data source integrations
├── templates/
│   └── index.html          # Web UI
├── uploads/                # Uploaded OSM files
└── requirements.txt        # Python dependencies
```

## Development

### Adding a New Data Source

1. Create a class in `traffic_data.py`:

```python
class MyDataSource(TrafficDataSource):
    def get_traffic(self, lat, lon, radius=1000):
        # Implement API call
        return [TrafficData(...)]
```

2. Add to aggregator in `create_aggregator()`:

```python
if 'myapi_key' in config:
    aggregator.add_source('myapi', MyDataSource(config['myapi_key']))
```

3. Update `DATA_SOURCES_INFO` with documentation

### Customizing the UI

Edit `templates/index.html`:
- Modify CSS in `<style>` section
- Update JavaScript functions
- Add new panels/features

## Troubleshooting

### "JamFree not available"
- Build Python bindings first: `cd ../.. && ./build_python.sh`
- Ensure jamfree is in Python path

### "Cannot download OSM data"
- Check internet connection
- Overpass API may be rate-limited
- Try smaller bounding box

### "API key invalid"
- Verify API keys in config.json
- Check API key permissions
- Ensure free tier limits not exceeded

## Performance

- **Simulation**: ~1000 vehicles at 60 FPS
- **Map**: Handles 10,000+ road segments
- **Data**: Caches API responses for 5 minutes

## Future Enhancements

- [ ] WebSocket for real-time updates
- [ ] 3D visualization
- [ ] Traffic light simulation
- [ ] Route planning
- [ ] Historical data playback
- [ ] Multi-user collaboration
- [ ] Export simulation results

## License

CeCILL-B Free Software License Agreement

## Credits

- **Leaflet** - Map visualization
- **OpenStreetMap** - Map data
- **Flask** - Web framework
- **JamFree** - Traffic simulation engine
