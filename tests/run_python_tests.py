#!/usr/bin/env python3
"""
Test runner for Python unit tests
"""

import sys
import os
import unittest
import subprocess

def run_unittest(module_path):
    """Run a specific unittest module"""
    try:
        # Add to path
        test_dir = os.path.dirname(module_path)
        if test_dir not in sys.path:
            sys.path.insert(0, test_dir)

        # Import and run
        module_name = os.path.basename(module_path).replace('.py', '')
        module = __import__(module_name)

        # Discover and run tests
        loader = unittest.TestLoader()
        suite = loader.loadTestsFromModule(module)
        runner = unittest.TextTestRunner(verbosity=2)
        result = runner.run(suite)

        return result.wasSuccessful()

    except Exception as e:
        print(f"Failed to run {module_path}: {e}")
        return False

def run_all_python_tests():
    """Run all Python tests"""
    test_files = [
        'tests/python/test_python_bindings.py',
        'tests/python/test_dsl_examples.py'
    ]

    results = []
    for test_file in test_files:
        if os.path.exists(test_file):
            print(f"\n{'='*60}")
            print(f"Running {test_file}")
            print(f"{'='*60}")
            success = run_unittest(test_file)
            results.append((test_file, success))
        else:
            print(f"Test file not found: {test_file}")
            results.append((test_file, False))

    # Summary
    print(f"\n{'='*60}")
    print("PYTHON TEST SUMMARY")
    print(f"{'='*60}")

    all_passed = True
    for test_file, success in results:
        status = "PASSED" if success else "FAILED"
        print(f"{test_file}: {status}")
        if not success:
            all_passed = False

    print(f"\nOverall: {'ALL TESTS PASSED' if all_passed else 'SOME TESTS FAILED'}")
    return all_passed

if __name__ == '__main__':
    success = run_all_python_tests()
    sys.exit(0 if success else 1)
