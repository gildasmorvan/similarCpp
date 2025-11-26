# SIMILAR Release Process

This document describes how to build and release SIMILAR components.

## Overview

SIMILAR is a multi-language simulation framework with components in C++, Python, and Java. The release process builds all components and creates distributable artifacts.

## Version Management

- **Version**: Defined in `python/similar2logo/__init__.py` as `__version__`
- **Current Version**: 1.0.0
- **Release Tags**: Use semantic versioning (v1.0.0, v1.1.0, etc.)

## Build Requirements

### System Dependencies
- CMake 3.10+
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Python 3.8+
- Java 11+ (optional, for Java components)
- Maven 3.6+ (optional, for Java components)

### Python Dependencies
```bash
pip install pybind11 numpy fastapi uvicorn websockets pydantic build twine
```

## Release Build Process

### Automated Release Build

Use the provided release script:

```bash
# Make sure script is executable
chmod +x make_release.sh

# Run the release build
./make_release.sh
```

This will:
1. Build C++ microkernel and extendedkernel
2. Build Similar2Logo Python bindings with C++ extensions
3. Build Python wheel and source distribution
4. Attempt to build Java components (optional)
5. Attempt to build JamFree components (optional)
6. Create release archives and artifacts

### Manual Build Steps

If you need to build components individually:

#### 1. Build C++ Core Components
```bash
# Build microkernel
cd cpp/microkernel
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Build extendedkernel
cd ../../extendedkernel
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Build Similar2Logo Python bindings
cd ../../similar2logo/python
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
make install
```

#### 2. Build Python Package
```bash
# Install in development mode first
pip install -e .

# Build distribution packages
python -m build --wheel --sdist
```

#### 3. Test the Build
```bash
# Test import
python -c "import similar2logo; print('Version:', similar2logo.__version__)"

# Test C++ engine availability
python -c "from similar2logo.cpp_engine import use_cpp_engine; print('C++ engine:', use_cpp_engine())"

# Test basic functionality
python -c "
from similar2logo import Environment, Turtle
class TestTurtle(Turtle):
    def decide(self, perception):
        return []
env = Environment(10, 10, toroidal=True)
turtle = TestTurtle(position=(5, 5), heading=0.0)
print('Basic functionality test passed')
"
```

## Release Artifacts

The release build creates the following artifacts in `release-build/artifacts/`:

### Python Artifacts
- `python/similar2logo-1.0.0-py3-none-any.whl` - Python wheel
- `python/similar2logo-1.0.0.tar.gz` - Source distribution

### C++ Artifacts
- `cpp/` - Compiled libraries (*.so, *.dylib, *.dll, *.a)

### Java Artifacts (if available)
- `java/` - JAR files for Java components

### Documentation
- `docs/` - Documentation files
- `VERSION` - Version file
- `BUILD_DATE` - Build timestamp
- `GIT_COMMIT` - Git commit hash

### Distribution Archives
- `similar-1.0.0-{platform}.tar.gz` - Complete release archive
- `similar-1.0.0-{platform}.zip` - Complete release archive (ZIP format)

## Distribution

### PyPI Release
```bash
# Install twine for PyPI uploads
pip install twine

# Upload to PyPI (requires PYPI_API_TOKEN secret)
twine upload release-build/artifacts/python/*.whl
twine upload release-build/artifacts/python/*.tar.gz
```

### GitHub Release
1. Create a git tag:
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

2. The `.github/workflows/release.yml` will automatically:
   - Build all components
   - Create GitHub release
   - Upload artifacts
   - Generate release notes

### Manual GitHub Release
If the automated workflow fails, create a release manually:
1. Go to GitHub → Releases → Create new release
2. Tag: `v1.0.0`
3. Title: `SIMILAR 1.0.0`
4. Upload the distribution archives from `release-build/artifacts/`

## Installation for Users

### From PyPI
```bash
pip install similar2logo
```

### From Source
```bash
# Clone repository
git clone https://github.com/gildasmorvan/similar.git
cd similar

# Install Python package
pip install .

# Build C++ engine for performance (optional)
./build_cpp_engine.sh
pip install -e .
```

### From Release Archive
```bash
# Extract archive
tar -xzf similar-1.0.0-{platform}.tar.gz
cd similar-1.0.0

# Install Python package
pip install python/similar2logo-1.0.0-py3-none-any.whl
```

## Testing Release

### Automated Testing
The CI/CD pipeline (`.github/workflows/ci.yml`) runs comprehensive tests on:
- Ubuntu 22.04, macOS 12, Windows 2022
- Python 3.8, 3.9, 3.10, 3.11
- C++, Python, and Java components

### Manual Testing
```bash
# Test basic import
python -c "import similar2logo"

# Test examples
cd examples/python
python boids_dsl.py
python multiturmite.py

# Test web interface
python -c "
from similar2logo.web import WebSimulation
from similar2logo import Environment, Turtle
# ... test web functionality
"
```

## Known Issues

### C++ Engine Availability
- The Python wheel does not include pre-compiled C++ extensions
- Users must have a compatible C++ compiler to use C++ acceleration
- The package falls back to pure Python implementation if C++ is unavailable

### Java Components
- Java build may fail if Java project modules are missing
- Java components are optional and don't affect Python functionality

### JamFree Components
- JamFree has compilation issues in some configurations
- JamFree is currently optional in the release build

## Troubleshooting

### Build Failures
1. **Missing dependencies**: Install required system packages
2. **CMake errors**: Ensure CMake 3.10+ is installed
3. **Python errors**: Use Python 3.8+ and install required packages
4. **Java errors**: Java components are optional, can be skipped

### Installation Issues
1. **C++ extensions not available**: Build C++ engine with `./build_cpp_engine.sh`
2. **Import errors**: Ensure all dependencies are installed
3. **Platform compatibility**: Wheels are platform-specific

## Future Improvements

- [ ] Cross-platform C++ extension wheels
- [ ] Automated multi-platform releases
- [ ] Docker container releases
- [ ] Conda package distribution
- [ ] Fix JamFree compilation issues
- [ ] Complete Java component integration

---

**Build Date**: $(date)
**Version**: 1.0.0
**Maintainer**: SIMILAR Team
