"""
setup_metal.py for JamFree Metal GPU Python bindings

This builds the Metal GPU acceleration extension module using pybind11.
Requires macOS with Metal support.
"""

import os
import sys
import platform
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""
    def __str__(self):
        import pybind11
        return pybind11.get_include()

# Only build on macOS
if platform.system() != 'Darwin':
    print("Metal GPU acceleration is only available on macOS")
    print("Skipping Metal module build")
    sys.exit(0)

# C++ source files for Metal module
cpp_sources = [
    'python/src/metal_bindings.cpp',
    'gpu/metal/MetalCompute.mm',  # Objective-C++ implementation
]

# Include directories
include_dirs = [
    str(get_pybind_include()),
    '.',  # Root directory for includes
]

# Compile and link flags for Metal
extra_compile_args = [
    '-std=c++17',
    '-ObjC++',  # Enable Objective-C++
]

extra_link_args = [
    '-framework', 'Metal',
    '-framework', 'Foundation',
]

# Extension module
ext_modules = [
    Extension(
        'jamfree._metal',
        sources=cpp_sources,
        include_dirs=include_dirs,
        language='c++',
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

setup(
    name='jamfree-metal',
    version='0.1.0',
    author='Gildas Morvan',
    author_email='gildas.morvan@univ-artois.fr',
    description='Metal GPU acceleration for JamFree traffic simulation',
    long_description='Provides Apple Metal GPU acceleration for large-scale traffic simulations (10,000+ vehicles)',
    ext_modules=ext_modules,
    packages=['jamfree'],
    package_dir={'jamfree': 'python/jamfree'},
    install_requires=[
        'pybind11>=2.6.0',
    ],
    python_requires='>=3.7',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Physics',
        'License :: OSI Approved :: CeCILL-B Free Software License Agreement (CECILL-B)',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Operating System :: MacOS :: MacOS X',
    ],
    zip_safe=False,
)
