# Contributing to SIMILAR & JamFree

Thank you for your interest in contributing! This guide will help you get started.

## 🎯 Ways to Contribute

### 1. Report Issues
- Bug reports
- Feature requests
- Documentation improvements
- Performance issues

### 2. Submit Code
- Bug fixes
- New features
- Performance improvements
- Example simulations

### 3. Improve Documentation
- Fix typos
- Add examples
- Clarify explanations
- Translate docs

### 4. Share Examples
- Create new simulations
- Improve existing examples
- Add use cases
- Write tutorials

## 🚀 Getting Started

### Setup Development Environment

```bash
# Clone repository
git clone https://github.com/gildasmorvan/similar
cd similar

# Install dependencies
pip install -r requirements.txt

# Build C++ engine
./build_cpp_engine.sh

# Build JamFree
cd cpp/jamfree
./build.sh
cd ../..

# Run tests
python3 -m pytest tests/
```

### Verify Installation

```bash
# Test SIMILAR2Logo
SIMILAR_VERBOSE=1 python3 examples/python/simple_random_walk.py

# Test JamFree
cd cpp/jamfree
python3 examples/basic_simulation.py
```

## 📝 Code Style

### Python

Follow PEP 8 with these specifics:

```python
# Use 4 spaces for indentation
# Max line length: 100 characters
# Use docstrings for all public functions

class MyTurtle(Turtle):
    """Brief description.
    
    Detailed description if needed.
    """
    
    def decide(self, perception):
        """Make decision based on perception.
        
        Args:
            perception: Dictionary with perception data
            
        Returns:
            List of influences
        """
        return []
```

### C++

Follow Google C++ Style Guide:

```cpp
// Use camelCase for methods
// Use PascalCase for classes
// Use snake_case for variables
// Document all public APIs

class MyModel {
public:
    /**
     * @brief Calculate acceleration
     * @param vehicle Current vehicle
     * @param leader Leader vehicle (can be nullptr)
     * @return Acceleration in m/s²
     */
    double calculateAcceleration(
        const Vehicle& vehicle,
        const Vehicle* leader
    ) const;
    
private:
    double m_desired_speed;  // m/s
};
```

## 🔧 Development Workflow

### 1. Create Branch

```bash
git checkout -b feature/my-new-feature
# or
git checkout -b fix/bug-description
```

### 2. Make Changes

- Write code
- Add tests
- Update documentation
- Test thoroughly

### 3. Test

```bash
# Run existing tests
python3 -m pytest tests/

# Test your changes
python3 examples/python/your_example.py

# Test with C++ engine
SIMILAR_VERBOSE=1 python3 examples/python/your_example.py
```

### 4. Commit

```bash
git add .
git commit -m "feat: Add new feature

Detailed description of changes.

Fixes #123"
```

**Commit Message Format:**
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation
- `perf:` Performance improvement
- `test:` Add tests
- `refactor:` Code refactoring

### 5. Push and Create PR

```bash
git push origin feature/my-new-feature
```

Then create a Pull Request on GitHub.

## 🎨 Adding New Examples

### SIMILAR2Logo Example

1. **Create file** in `examples/python/`:

```python
"""
My New Example - Brief description

Demonstrates: concept1, concept2
Complexity: Beginner/Intermediate/Advanced
"""

from similar2logo.dsl import *
import random

class MyTurtle(Turtle):
    """Description of turtle behavior."""
    
    def decide(self, perception):
        # Your logic here
        return influences

# Configuration
NUM_AGENTS = 100
GRID_SIZE = 100

# Setup
env = Environment(GRID_SIZE, GRID_SIZE, toroidal=True)
sim = Simulation(environment=env, title="My Example")

# Add agents
for _ in range(NUM_AGENTS):
    sim.add_agent(MyTurtle(
        position=env.random_position(),
        heading=env.random_heading()
    ))

# Run
if __name__ == "__main__":
    sim.run_web(port=8080)
```

2. **Add to README** in `examples/python/README.md`

3. **Test thoroughly**

### JamFree Example

1. **Create file** in `cpp/jamfree/examples/`:

```python
"""
My Traffic Example - Brief description
"""

import jamfree
import sys

def main():
    # Load network
    network = jamfree.OSMParser.parse_file("map.osm")
    
    # Create simulation
    # Your code here
    
    print("Simulation complete!")

if __name__ == "__main__":
    main()
```

2. **Document** in `cpp/jamfree/examples/README.md`

3. **Test** with various configurations

## 📚 Documentation Guidelines

### Adding Documentation

1. **Choose appropriate location**:
   - Main guides → Root directory
   - SIMILAR2Logo → Root or `examples/python/`
   - JamFree → `cpp/jamfree/`

2. **Follow template**:

```markdown
# Title

Brief description.

## Overview

What this document covers.

## Content

Main content with examples.

## See Also

- [Related Doc 1](link)
- [Related Doc 2](link)
```

3. **Update index**:
   - Add to [DOCUMENTATION_INDEX.md](DOCUMENTATION_INDEX.md)
   - Link from related docs

### Documentation Standards

- **Clear titles** - Descriptive and specific
- **Code examples** - Working, copy-paste ready
- **Screenshots** - For UI features
- **Cross-references** - Link related docs
- **Keep updated** - Update when code changes

## 🧪 Testing

### Unit Tests

```python
# tests/test_my_feature.py
import pytest
from similar2logo.dsl import *

def test_my_feature():
    """Test description."""
    # Setup
    env = Environment(100, 100)
    
    # Test
    result = env.some_method()
    
    # Assert
    assert result == expected
```

### Integration Tests

```python
def test_simulation_runs():
    """Test that simulation runs without errors."""
    env = Environment(100, 100)
    sim = Simulation(environment=env)
    sim.add_agent(MyTurtle())
    
    # Should not raise
    sim.run(steps=100)
```

### Performance Tests

```python
import time

def test_performance():
    """Test that simulation meets performance requirements."""
    sim = create_large_simulation()
    
    start = time.time()
    sim.run(steps=1000)
    duration = time.time() - start
    
    # Should complete in reasonable time
    assert duration < 10.0  # seconds
```

## 🐛 Debugging

### Enable Verbose Mode

```bash
SIMILAR_VERBOSE=1 python3 your_script.py
```

### Check C++ Engine

```python
import similar2logo
print(f"C++ engine available: {similar2logo.cpp_available}")
```

### Profile Performance

```python
import cProfile
import pstats

profiler = cProfile.Profile()
profiler.enable()

# Your code here
sim.run(steps=1000)

profiler.disable()
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(20)
```

## 📋 Pull Request Checklist

Before submitting:

- [ ] Code follows style guidelines
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] Examples work
- [ ] No breaking changes (or documented)
- [ ] Commit messages clear
- [ ] PR description complete

## 🎯 Areas Needing Contribution

### High Priority
- Additional traffic data sources
- More visualization options
- Performance profiling tools
- Additional examples

### Medium Priority
- Docker containers
- CI/CD integration
- Cloud deployment guides
- Additional tests

### Low Priority
- Translations
- Additional documentation
- Code cleanup
- Optimization

## 💡 Tips for Contributors

1. **Start small** - Fix a typo, improve a doc
2. **Ask questions** - Use GitHub Issues
3. **Read existing code** - Learn the patterns
4. **Test thoroughly** - Prevent regressions
5. **Document well** - Help future users

## 🤝 Code Review Process

1. **Submit PR** - With clear description
2. **Automated checks** - Must pass
3. **Review** - Maintainer reviews code
4. **Feedback** - Address comments
5. **Merge** - Once approved

## 📞 Getting Help

- **Questions**: GitHub Issues
- **Discussion**: GitHub Discussions
- **Documentation**: [DOCUMENTATION_INDEX.md](DOCUMENTATION_INDEX.md)
- **Examples**: `examples/python/`

## 🏆 Recognition

Contributors are:
- Listed in [AUTHORS.txt](AUTHORS.txt)
- Mentioned in release notes
- Credited in documentation

## 📄 License

By contributing, you agree that your contributions will be licensed under the CeCILL-B license.

## 🎉 Thank You!

Every contribution helps make SIMILAR & JamFree better for everyone!

---

**Questions?** Open an issue or check [DOCUMENTATION_INDEX.md](DOCUMENTATION_INDEX.md)
