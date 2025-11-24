# Running Boids DSL Web Interface

## Quick Start

To run the boids_dsl.py web interface, you need to install the web dependencies first.

### Option 1: Using a Virtual Environment (Recommended)

```bash
# Create virtual environment
cd /Users/morvan/Antigravity/similar
python3 -m venv venv

# Activate it
source venv/bin/activate

# Install dependencies
pip install fastapi uvicorn websockets

# Run the simulation
PYTHONPATH=python python3 examples/python/boids_dsl.py --web
```

Then open http://localhost:8080 in your browser!

### Option 2: Install with --break-system-packages (Not Recommended)

```bash
python3 -m pip install --break-system-packages fastapi uvicorn websockets
PYTHONPATH=python python3 examples/python/boids_dsl.py --web
```

### Option 3: Run Console Version (No Dependencies Needed)

```bash
cd /Users/morvan/Antigravity/similar
PYTHONPATH=python python3 examples/python/boids_dsl.py
```

This runs the simulation in console mode without needing web dependencies.

## What You Need

The web interface requires:
- `fastapi` - Web framework
- `uvicorn` - ASGI server  
- `websockets` - WebSocket support

These are only needed for the web interface (`--web` flag).

## Already Have Dependencies?

If you already have the dependencies installed (in a venv or globally), just run:

```bash
cd /Users/morvan/Antigravity/similar
PYTHONPATH=python python3 examples/python/boids_dsl.py --web
```

## Troubleshooting

**"ModuleNotFoundError: No module named 'fastapi'"**
→ Install dependencies using Option 1 above

**"externally-managed-environment"**
→ Use a virtual environment (Option 1)

**Want to avoid dependencies?**
→ Use console mode (Option 3)
