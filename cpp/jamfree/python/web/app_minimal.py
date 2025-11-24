#!/usr/bin/env python3
"""
Minimal JamFree Web UI for testing
"""

from flask import Flask, render_template, jsonify, request
from flask_cors import CORS
import json

app = Flask(__name__)
CORS(app)

# Try to import jamfree
try:
    import jamfree
    JAMFREE_AVAILABLE = True
    print("✓ JamFree available")
except ImportError:
    JAMFREE_AVAILABLE = False
    print("✗ JamFree not available")

# Global simulation state
simulation_state = {
    'network': None,
    'vehicles': [],
    'running': False,
    'step': 0,
    'config': {
        'num_vehicles': 200,
        'desired_speed': 120,
        'time_headway': 1.5,
        'politeness': 0.3,
        'use_idm_lookup': True,
        'use_spatial_index': True,
        'use_multithreading': True,
        'use_adaptive_hybrid': True,
        'use_gpu': False,
        'adaptive_threshold': 50,
    }
}

@app.route('/')
def index():
    """Main page."""
    return render_template('index.html', jamfree_available=JAMFREE_AVAILABLE)

@app.route('/api/status')
def get_status():
    """Get simulation status."""
    return jsonify({
        'jamfree_available': JAMFREE_AVAILABLE,
        'running': simulation_state['running'],
        'step': simulation_state['step'],
        'num_vehicles': len(simulation_state['vehicles']),
        'network_loaded': simulation_state['network'] is not None
    })

@app.route('/api/config', methods=['GET', 'POST'])
def config():
    """Get or update simulation configuration."""
    if request.method == 'POST':
        data = request.json
        simulation_state['config'].update(data)
        return jsonify({'status': 'ok', 'config': simulation_state['config']})
    else:
        return jsonify(simulation_state['config'])

@app.route('/api/data/sources')
def list_data_sources():
    """List available real-time traffic data sources."""
    sources = [
        {
            'id': 'tomtom',
            'name': 'TomTom Traffic API',
            'type': 'commercial',
        },
        {
            'id': 'here',
            'name': 'HERE Traffic API',
            'type': 'commercial',
        },
    ]
    return jsonify({'sources': sources})

if __name__ == '__main__':
    print("=" * 60)
    print("JamFree Web UI (Minimal)")
    print("=" * 60)
    print()
    print(f"JamFree available: {JAMFREE_AVAILABLE}")
    print()
    print("Starting server on http://localhost:5001")
    print()
    
    app.run(debug=False, host='0.0.0.0', port=5001, threaded=True)
