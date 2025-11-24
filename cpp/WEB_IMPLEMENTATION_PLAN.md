# SIMILAR C++ Web Interface - Implementation Plan

## Overview

Implementing the complete web interface from `fr.univ_artois.lgi2a.similar.extendedkernel.libs.web` package.

## Architecture

### Technology Stack

**C++ Backend:**
- **cpp-httplib** (header-only HTTP server)
- **nlohmann/json** (JSON serialization)
- **Threading** (std::thread for async simulation)

**Frontend** (from Java resources):
- Bootstrap 5 (CSS framework)
- jQuery 3.6.0 (JavaScript)
- Custom similar-gui.js/css

## Components to Implement

### 1. Core Interfaces

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `IHtmlInitializationData` | Interface | ⏳ TODO |
| `IHtmlControls` | Interface | ⏳ TODO |
| `IHtmlRequests` | Interface | ⏳ TODO |

### 2. Configuration

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `SimilarWebConfig` | Class | ⏳ TODO |

### 3. Controller (MVC)

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `SimilarWebController` | Class + IProbe | ⏳ TODO |
| `EngineState` | Enum class | ⏳ TODO |

### 4. View (HTTP Server)

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `SimilarHttpServer` | cpp-httplib wrapper | ⏳ TODO |
| `SimilarHtmlGenerator` | HTML template generator | ⏳ TODO |

### 5. Runner

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `SimilarWebRunner` | Main entry point | ⏳ TODO |

### 6. Utilities

| Java Class | C++ Translation | Status |
|------------|-----------------|--------|
| `SimulationExecutionThread` | std::thread wrapper | ⏳ TODO |
| `ParameterNotFoundException` | Exception | ⏳ TODO |
| `ResourceNotFoundException` | Exception | ⏳ TODO |

## API Endpoints

### Control Endpoints
- `GET /` - Main dashboard (HTML)
- `GET /start` - Start simulation
- `GET /stop` - Stop/abort simulation
- `GET /pause` - Pause/resume simulation
- `GET /shutdown` - Shutdown server

### Data Endpoints
- `GET /status` - Get engine state (JSON)
- `GET /getParameter?name=X` - Get parameter value
- `GET /setParameter?name=X&value=Y` - Set parameter value

### Static Resources
- `GET /css/*` - CSS files
- `GET /js/*` - JavaScript files
- `GET /img/*` - Images

## File Structure

```
cpp/extendedkernel/
├── include/libs/web/
│   ├── IHtmlInitializationData.h
│   ├── IHtmlControls.h
│   ├── IHtmlRequests.h
│   ├── SimilarWebConfig.h
│   ├── SimilarWebRunner.h
│   ├── control/
│   │   ├── SimilarWebController.h
│   │   └── EngineState.h
│   ├── view/
│   │   ├── SimilarHttpServer.h
│   │   └── SimilarHtmlGenerator.h
│   └── SimulationExecutionThread.h
│
├── src/libs/web/
│   ├── SimilarWebConfig.cpp
│   ├── SimilarWebRunner.cpp
│   ├── control/
│   │   └── SimilarWebController.cpp
│   ├── view/
│   │   ├── SimilarHttpServer.cpp
│   │   └── SimilarHtmlGenerator.cpp
│   └── SimulationExecutionThread.cpp
│
└── resources/web/
    ├── css/
    │   ├── bootstrap.min.css
    │   └── similar-gui.css
    ├── js/
    │   ├── jquery-3.6.0.min.js
    │   ├── bootstrap.min.js
    │   └── similar-gui.js
    └── img/
        └── (logo files)
```

## Dependencies

### Required Libraries

1. **cpp-httplib** (Header-only)
   ```bash
   # Download single header
   wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
   ```

2. **nlohmann/json** (Header-only)
   ```bash
   # Download single header
   wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
   ```

Both are header-only, so no linking required!

## Implementation Priority

### Phase 1: Core Infrastructure (HIGH PRIORITY)
1. ✅ Download cpp-httplib and nlohmann/json headers
2. ⏳ Implement `EngineState` enum
3. ⏳ Implement `SimilarWebConfig`
4. ⏳ Implement `IHtmlControls`, `IHtmlRequests`, `IHtmlInitializationData`

### Phase 2: Controller (HIGH PRIORITY)
1. ⏳ Implement `SimulationExecutionThread`
2. ⏳ Implement `SimilarWebController` (MVC controller + IProbe)

### Phase 3: View (MEDIUM PRIORITY)
1. ⏳ Implement `SimilarHtmlGenerator` (HTML templates)
2. ⏳ Implement `SimilarHttpServer` (HTTP endpoints)
3. ⏳ Copy static resources (CSS/JS/images)

### Phase 4: Runner (MEDIUM PRIORITY)
1. ⏳ Implement `SimilarWebRunner` (main entry point)

### Phase 5: Example (LOW PRIORITY)
1. ⏳ Create web-enabled predator-prey example
2. ⏳ Test all endpoints
3. ⏳ Add real-time updates

## Example Usage (Target API)

```cpp
#include "libs/web/SimilarWebRunner.h"

int main() {
    // Create simulation model
    auto model = std::make_shared<PredatorPreyModel>();
    
    // Create and configure web runner
    SimilarWebRunner runner;
    runner.getConfig()->setPort(8080);
    runner.getConfig()->setSimulationName("Predator-Prey Ecosystem");
    runner.getConfig()->setAutoOpenBrowser(true);
    
    // Initialize with model
    runner.initializeRunner(model);
    
    // Start web interface (blocking)
    runner.showView();
    
    // Web interface now running at http://localhost:8080
    // - View simulation status
    // - Start/stop/pause via buttons
    // - Adjust parameters in real-time
    
    return 0;
}
```

## Features

### MVP (Minimum Viable Product)
- ✅ Basic HTTP server
- ✅ Start/stop/pause controls
- ✅ Engine state display
- ✅ Static resource serving

### Nice-to-Have
- 📊 Real-time charts (Chart.js)
- 🔄 WebSocket for live updates
- 📥 Data export (JSON/CSV)
- 🎨 Custom visualization canvas

## Testing Plan

1. **Unit Tests**
   - Test each controller method
   - Test state transitions
   - Test parameter get/set

2. **Integration Tests**
   - Test HTTP endpoints
   - Test simulation lifecycle
   - Test concurrent requests

3. **End-to-End Tests**
   - Run full simulation via web
   - Test all buttons
   - Test parameter changes

## Timeline Estimate

- **Phase 1**: 2-3 hours
- **Phase 2**: 3-4 hours
- **Phase 3**: 2-3 hours
- **Phase 4**: 1-2 hours
- **Phase 5**: 2-3 hours

**Total**: ~10-15 hours for complete implementation

## Next Steps

1. Download dependencies (cpp-httplib, nlohmann/json)
2. Implement core interfaces and enums
3. Implement controller logic
4. Implement HTTP server
5. Create working example

Ready to start implementation!
