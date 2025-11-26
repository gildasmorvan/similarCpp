#!/usr/bin/env python3
"""
Test runner for JamFree unit tests
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

def run_cmake_test(test_name):
    """Run a CMake-built test executable"""
    try:
        # Try to run from jamfree build directory
        jamfree_build = os.path.join(os.path.dirname(__file__), '..', 'cpp', 'jamfree', 'build')
        test_path = os.path.join(jamfree_build, test_name)

        if os.path.exists(test_path):
            result = subprocess.run([test_path], capture_output=True, text=True, timeout=60)
            print(result.stdout)
            if result.stderr:
                print("STDERR:", result.stderr)
            return result.returncode == 0
        else:
            print(f"Test executable not found: {test_path}")
            return False

    except subprocess.TimeoutExpired:
        print(f"Test {test_name} timed out")
        return False
    except Exception as e:
        print(f"Failed to run {test_name}: {e}")
        return False

def run_all_jamfree_tests():
    """Run all JamFree tests"""
    test_results = []

    print("=" * 60)
    print("JAMFREE UNIT TEST SUITE")
    print("=" * 60)

    # Python binding tests
    python_tests = [
        'tests/python/test_jamfree_bindings.py'
    ]

    for test_file in python_tests:
        if os.path.exists(test_file):
            print(f"\n{'='*40}")
            print(f"Running Python: {test_file}")
            print(f"{'='*40}")
            success = run_unittest(test_file)
            test_results.append((f"Python: {test_file}", success))
        else:
            print(f"Python test file not found: {test_file}")
            test_results.append((f"Python: {test_file}", False))

    # C++ unit tests
    cpp_tests = [
        'jamfree_comprehensive_test'
    ]

    for test_name in cpp_tests:
        print(f"\n{'='*40}")
        print(f"Running C++: {test_name}")
        print(f"{'='*40}")
        success = run_cmake_test(test_name)
        test_results.append((f"C++: {test_name}", success))

    # Summary
    print(f"\n{'='*60}")
    print("JAMFREE TEST SUMMARY")
    print(f"{'='*60}")

    all_passed = True
    for test_name, success in test_results:
        status = "PASSED" if success else "FAILED"
        print(f"{test_name}: {status}")
        if not success:
            all_passed = False

    print(f"\nOverall: {'ALL TESTS PASSED' if all_passed else 'SOME TESTS FAILED'}")
    return all_passed

if __name__ == '__main__':
    success = run_all_jamfree_tests()
    sys.exit(0 if success else 1)
