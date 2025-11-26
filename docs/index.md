# SIMILAR: Multi-Agent Simulation Framework

SIMILAR is a comprehensive framework for developing and running multi-agent-based simulations. It provides both Java and C++/Python implementations of the SIMILAR API, enabling high-performance agent-based modeling with easy-to-use interfaces.

## Key Features

- **Multi-paradigm**: Support for microscopic, macroscopic, and hybrid simulation approaches
- **High Performance**: C++ backend with multithreaded execution engine
- **Easy to Use**: Python DSL with web-based visualization
- **Extensible**: Plugin architecture for custom models and behaviors
- **Cross-platform**: Runs on Linux, macOS, and Windows

## Quick Start

### Python DSL

```python
from similar2logo import LogoSimulation, Turtle

class MyAgent(Turtle):
    def decide(self, perception):
        # Your agent logic here
        return self.influence_move_forward(1.0)

# Create and run simulation
sim = LogoSimulation.create(MyAgent, count=100)
sim.run_web()  # Opens browser with live visualization
```

### C++ Engine

```cpp
// High-performance C++ simulation
auto engine = std::make_unique<MultiThreadedSimulationEngine>();
engine->run(model, parameters);
```

## Components

- **Similar2Logo**: Logo-inspired agent-based modeling
- **JamFree**: Traffic simulation platform
- **Microkernel**: Core simulation engine
- **ExtendedKernel**: Advanced agent modeling features

## Documentation

- [Getting Started](GETTING_STARTED.md)
- [User Guide](README_USER.txt)
- [Developer Guide](README_DEVELOPPER.txt)
- [API Reference](QUICK_REFERENCE.md)

## Community

- [GitHub Repository](https://github.com/gildasmorvan/similar)
- [Issues](https://github.com/gildasmorvan/similar/issues)
- [Discussions](https://github.com/gildasmorvan/similar/discussions)

---

*Built with ❤️ by the SIMILAR team*
