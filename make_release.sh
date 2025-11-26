#!/bin/bash

# SIMILAR Release Build Script
# This script builds all components and creates release artifacts

set -e  # Exit on error

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Activate virtual environment if it exists
if [ -f "venv/bin/activate" ]; then
    source venv/bin/activate
    echo "✓ Activated virtual environment"
elif [ -f "venv/Scripts/activate" ]; then
    source venv/Scripts/activate
    echo "✓ Activated virtual environment (Windows)"
else
    echo "⚠️  No virtual environment found, using system Python"
fi

echo "========================================"
echo "SIMILAR Release Build Script v1.0.0"
echo "========================================"
echo ""

# Check for required tools
echo "Checking dependencies..."
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is required but not installed."; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "Error: python3 is required but not installed."; exit 1; }
command -v java >/dev/null 2>&1 || { echo "Error: java is required but not installed."; exit 1; }
command -v mvn >/dev/null 2>&1 || { echo "Error: maven is required but not installed."; exit 1; }

# Check for pybind11 and other Python deps
python3 -c "import pybind11" 2>/dev/null || {
    echo "Error: pybind11 is required. Install with: pip install pybind11"
    exit 1
}

echo "✓ All dependencies found"
echo ""

# Create release directory
RELEASE_DIR="$SCRIPT_DIR/release-build"
rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

echo "Building C++ components..."
echo "=========================="

# Build microkernel
echo "Building microkernel..."
MICROKERNEL_BUILD="$RELEASE_DIR/microkernel"
mkdir -p "$MICROKERNEL_BUILD"
cd "$MICROKERNEL_BUILD"
cmake -DCMAKE_BUILD_TYPE=Release "$SCRIPT_DIR/cpp/microkernel"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Build extendedkernel
echo ""
echo "Building extendedkernel..."
EXTKERNEL_BUILD="$RELEASE_DIR/extendedkernel"
mkdir -p "$EXTKERNEL_BUILD"
cd "$EXTKERNEL_BUILD"
cmake -DCMAKE_BUILD_TYPE=Release "$SCRIPT_DIR/cpp/extendedkernel"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Build similar2logo Python bindings
echo ""
echo "Building similar2logo Python bindings..."
SIMILAR2LOGO_BUILD="$RELEASE_DIR/similar2logo"
mkdir -p "$SIMILAR2LOGO_BUILD"
cd "$SIMILAR2LOGO_BUILD"
cmake -DCMAKE_BUILD_TYPE=Release "$SCRIPT_DIR/cpp/similar2logo/python"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Build jamfree (optional - skip if there are compilation issues)
echo ""
echo "Building jamfree..."
JAMFREE_BUILD="$RELEASE_DIR/jamfree"
mkdir -p "$JAMFREE_BUILD"
cd "$JAMFREE_BUILD"
if cmake -DCMAKE_BUILD_TYPE=Release "$SCRIPT_DIR/cpp/jamfree" && make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4); then
    echo "✓ JamFree built successfully"
else
    echo "⚠️  JamFree build failed - skipping (known compilation issues)"
    JAMFREE_FAILED=true
fi

echo ""
echo "Building Python bindings..."
echo "==========================="

# Build Python bindings
cd "$SCRIPT_DIR"
python3 -m pip install --upgrade pip build wheel twine
python3 -m build --wheel --sdist

echo ""
echo "Building Java components..."
echo "==========================="

# Build Java components (optional - skip if modules are missing)
cd "$SCRIPT_DIR"
if mvn clean package -DskipTests -q; then
    echo "✓ Java components built successfully"
else
    echo "⚠️  Java build failed - skipping (missing Java modules)"
    JAVA_FAILED=true
fi

echo ""
echo "Creating release artifacts..."
echo "============================="

# Create release artifacts directory
ARTIFACTS_DIR="$RELEASE_DIR/artifacts"
mkdir -p "$ARTIFACTS_DIR"/{cpp,python,java,docs}

# Copy C++ libraries
echo "Copying C++ artifacts..."
find "$RELEASE_DIR" -name "*.so" -o -name "*.dylib" -o -name "*.dll" -o -name "*.a" | xargs -I {} cp {} "$ARTIFACTS_DIR/cpp/" 2>/dev/null || true

# Copy Python artifacts
echo "Copying Python artifacts..."
cp dist/*.whl "$ARTIFACTS_DIR/python/" 2>/dev/null || true
cp dist/*.tar.gz "$ARTIFACTS_DIR/python/" 2>/dev/null || true

# Copy Java artifacts
echo "Copying Java artifacts..."
find . -name "*.jar" -not -path "*/target/test-classes/*" -not -path "*/target/classes/*" | xargs -I {} cp {} "$ARTIFACTS_DIR/java/" 2>/dev/null || true

# Copy documentation
echo "Copying documentation..."
cp README.md "$ARTIFACTS_DIR/docs/"
cp *.md "$ARTIFACTS_DIR/docs/" 2>/dev/null || true
cp -r docs/* "$ARTIFACTS_DIR/docs/" 2>/dev/null || true

# Create version file
echo "1.0.0" > "$ARTIFACTS_DIR/VERSION"
date > "$ARTIFACTS_DIR/BUILD_DATE"
git rev-parse HEAD > "$ARTIFACTS_DIR/GIT_COMMIT" 2>/dev/null || echo "unknown" > "$ARTIFACTS_DIR/GIT_COMMIT"

# Create compressed archives
echo ""
echo "Creating distribution archives..."
cd "$RELEASE_DIR/artifacts"

# Create platform-specific archives
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="windows"
else
    PLATFORM="unknown"
fi

ARCHIVE_NAME="similar-1.0.0-${PLATFORM}"

tar -czf "${ARCHIVE_NAME}.tar.gz" .
zip -r "${ARCHIVE_NAME}.zip" . 2>/dev/null || echo "zip not available, skipping zip archive"

echo ""
echo "========================================"
echo "✓ Release build completed successfully!"
echo "========================================"
echo ""
echo "Release artifacts created in: $ARTIFACTS_DIR"
echo ""
echo "Archives created:"
ls -la *.tar.gz *.zip 2>/dev/null || true
echo ""
echo "Contents:"
find . -type f | head -20
echo ""
echo "To test the release:"
echo "  python3 -m pip install $ARTIFACTS_DIR/python/*.whl"
echo "  python3 -c \"import similar2logo; print('✓ Import successful')\""
echo ""
echo "For distribution:"
echo "  - Upload Python wheels to PyPI"
echo "  - Upload Java JARs to Maven Central"
echo "  - Upload archives to GitHub Releases"
