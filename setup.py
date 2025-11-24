"""
Setup script for SIMILAR2Logo Python bindings.
"""

from setuptools import setup, find_packages
from pathlib import Path

# Read README
readme_file = Path(__file__).parent / "README.md"
long_description = readme_file.read_text() if readme_file.exists() else ""

setup(
    name="similar2logo",
    version="0.1.0",
    author="SIMILAR Team",
    author_email="similar@example.com",
    description="Python bindings for SIMILAR2Logo multi-agent simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/gildasmorvan/similar",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20.0",
        "fastapi>=0.100.0",
        "uvicorn>=0.23.0",
        "websockets>=11.0",
        "pydantic>=2.0.0",
    ],
    extras_require={
        "viz": ["matplotlib>=3.5.0"],
        "dev": [
            "pytest>=7.0.0",
            "pytest-asyncio>=0.21.0",
            "black>=23.0.0",
            "mypy>=1.0.0",
        ],
    },
    # Note: The C++ extension (_core) is built separately via CMake
    # and should be copied to the package directory before installation
)
