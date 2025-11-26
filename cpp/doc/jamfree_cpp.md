## JamFree C++ Engine Overview

JamFree is a **traffic simulation engine** built on SIMILAR concepts and implemented in **C++** under `cpp/jamfree`. It supports microscopic and hybrid micro/macro simulation, Python bindings, a web UI, and optional GPU acceleration.

### Directory Structure (C++)

At a high level:

```text
cpp/jamfree/
├── kernel/          # Core geometry, tools, and low-level traffic model types
├── microscopic/     # Microscopic models (IDM, MOBIL, etc.)
├── macroscopic/     # Macroscopic / hybrid models
├── realdata/        # Real-data integration
├── supervision/     # Higher-level supervision logic
├── python/          # Python bindings and web UI
└── gpu/             # (Optional) GPU/Metal acceleration
```

For implementation details and status, see:

- `cpp/jamfree/IMPLEMENTATION_SUMMARY.md`
- `cpp/jamfree/OPTIMIZATION_COMPLETION_REPORT.md`
- `cpp/jamfree/MICRO_MACRO_INTERFACE_COMPLETE.md`
- `cpp/jamfree/GPU_IMPLEMENTATION_SUMMARY.md`

### Building the C++ Engine

Basic C++ build (CPU engine only):

```bash
cd cpp/jamfree
mkdir -p build
cd build
cmake ..
make
```

Python bindings and web UI (see also `PYTHON_BINDINGS_SUMMARY.md` and `QUICK_START.md`):

```bash
cd cpp/jamfree
./build_python.sh         # Build Python extension
./run_web_ui.sh           # Start the web UI
```

Then open `http://localhost:5001` in a browser.

### Core C++ Components

Key C++ pieces include:

- **Kernel / tools** (e.g. `GeometryTools`, `MathTools`) – header-only utilities for geometry and numerics.
- **Model** types (e.g. `Point2D`, `Road`, `Lane`, `Vehicle`, `Network`) – represent the road network and vehicles.
- **Microscopic models** – car-following and lane-changing (e.g. IDM, MOBIL).
- **Hybrid micro/macro bridge** – adaptive switching between microscopic and macroscopic models (see `MICRO_MACRO_AUTO_SWITCH.md`).

These are designed for:

- High performance (inline utilities, efficient data structures, multithreading).
- Clean, testable APIs.
- Direct exposure to Python via pybind11.

### Python and Web Integration

JamFree’s C++ engine is controlled from Python:

- Python package and bindings: `cpp/jamfree/python/` (see `PYTHON_BINDINGS_SUMMARY.md` and `python/README.md`).
- Web UI and API server: `cpp/jamfree/python/web/` (started via `run_web_ui.sh`).

Typical workflow:

1. Build the C++ + Python bindings.
2. Launch the web UI.
3. Load or download a road network (e.g. from OpenStreetMap).
4. Configure simulation parameters (vehicles, models, optimizations).
5. Run simulations and inspect performance metrics.

`QUICK_START.md` provides a concise “run this now” guide, and `ADAPTIVE_HYBRID_SIMULATION.md` explains the hybrid engine behavior.

### Performance and Optimizations

JamFree includes several major optimizations (see `OPTIMIZATION_COMPLETION_REPORT.md` and `WEB_UI_OPTIMIZATIONS.md`):

- **IDM lookup tables** for fast car-following updates.
- **Spatial indexing** for leader/follower queries.
- **Multithreading** for parallel vehicle updates.
- **Adaptive hybrid micro/macro** switching for large‑scale scenarios.
- Optional **GPU/Metal** acceleration (`gpu/metal`, documented in `GPU_METAL_ACCELERATION.md`).

Performance expectations and test procedures are summarized in:

- `QUICK_START.md`
- `TEST_RESULTS_SUMMARY.md`
- `GPU_WEB_INTEGRATION.md`

The top-level `AUDIT.md` also defines **future JamFree C++ work** (Section 9) that builds on these optimizations:

- Completing microscopic lane-changing and multi-lane logic in `MicroscopicReactionModel.cpp`.
- Enhancing the C++ hybrid controller in `MultiLevelCoordinator.cpp`.
- Adding calibration/scenario APIs, richer C++ probes, and optional GPU/SIMD backends.

Those items are **planned next steps** and should be implemented using the JamFree C++ architecture and tests described in this document and the JamFree markdown files under `cpp/jamfree/`.

### Relationship to SIMILAR Documentation

Conceptually, JamFree follows the SIMILAR architecture:

- Agents (vehicles) evolve within levels (road network, macroscopic cells).
- Influences and reactions determine the next state.
- Probes and supervision components observe the simulation.

For the underlying theory (multi-level simulation, influences, reactions), use the original docs in `src/doc`. For concrete C++/Python usage and traffic‑specific behavior, use the documents in `cpp/jamfree/` together with this overview.


