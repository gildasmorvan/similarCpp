"""
setup.py for JamFree Python bindings

This builds the C++ extension module using pybind11.
"""

import os
import sys
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""
    def __str__(self):
        import pybind11
        return pybind11.get_include()

# C++ source files
cpp_sources = [
    'python/src/bindings.cpp',
    'kernel/src/model/Lane.cpp',
    'realdata/src/OSMParser.cpp',
    'hybrid/src/AdaptiveSimulator.cpp',
]

# Include directories
include_dirs = [
    str(get_pybind_include()),
    '.',  # Root directory for includes
]

# Extension module
ext_modules = [
    Extension(
        'jamfree._jamfree',
        sources=cpp_sources,
        include_dirs=include_dirs,
        language='c++',
        extra_compile_args=['-std=c++17'],
    ),
]

setup(
    name='jamfree',
    version='0.1.0',
    author='Gildas Morvan',
    author_email='gildas.morvan@univ-artois.fr',
    description='Traffic simulation library with microscopic models',
    long_description=open('README.md').read() if os.path.exists('README.md') else '',
    long_description_content_type='text/markdown',
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
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: C++',
    ],
    zip_safe=False,
)
