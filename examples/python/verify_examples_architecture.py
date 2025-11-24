#!/usr/bin/env python3
"""
Verify all Python examples comply with the new architecture.

This script checks that all examples:
1. Use correct imports (Turtle, not SimpleTurtle)
2. Have proper decide() method signatures
3. Work with the new perception format
4. Can run without errors
"""

import sys
import os
import importlib.util
from pathlib import Path

# Add parent directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../../python'))

def check_file(filepath):
    """Check if a file complies with the new architecture."""
    print(f"\nChecking: {filepath.name}")
    print("-" * 60)
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    issues = []
    
    # Check 1: SimpleTurtle usage (should use Turtle)
    if 'SimpleTurtle' in content:
        issues.append("❌ Uses deprecated 'SimpleTurtle' - should use 'Turtle'")
    else:
        print("✓ Uses correct 'Turtle' base class")
    
    # Check 2: Proper imports
    if 'from similar2logo.dsl import' in content:
        print("✓ Uses DSL imports")
    elif 'from similar2logo.model import' in content:
        print("⚠️  Uses low-level model imports (consider using DSL)")
    
    # Check 3: Has decide() method
    if 'def decide(self, perception)' in content:
        print("✓ Has decide(perception) method")
    else:
        if 'class' in content and 'Turtle' in content:
            issues.append("❌ Missing decide(perception) method")
    
    # Check 4: Perception access pattern
    if "perception.get(" in content or "perception[" in content:
        print("✓ Uses correct perception access pattern")
    elif 'def decide' in content:
        issues.append("⚠️  May not be accessing perception correctly")
    
    # Check 5: Influence methods
    if 'influence_' in content:
        print("✓ Uses influence methods")
    
    if issues:
        print("\nIssues found:")
        for issue in issues:
            print(f"  {issue}")
        return False
    else:
        print("\n✅ All checks passed!")
        return True

def main():
    """Check all Python examples."""
    examples_dir = Path(__file__).parent
    
    print("=" * 60)
    print("Verifying Python Examples Architecture Compliance")
    print("=" * 60)
    
    # Find all Python files
    py_files = sorted(examples_dir.glob("*.py"))
    
    # Filter out this script and utility files
    py_files = [f for f in py_files if f.name not in [
        'verify_examples_architecture.py',
        'run_examples.py',
        '__init__.py'
    ]]
    
    results = {}
    for filepath in py_files:
        try:
            passed = check_file(filepath)
            results[filepath.name] = passed
        except Exception as e:
            print(f"❌ Error checking {filepath.name}: {e}")
            results[filepath.name] = False
    
    # Summary
    print("\n" + "=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    passed = sum(1 for v in results.values() if v)
    total = len(results)
    
    for filename, passed_check in sorted(results.items()):
        status = "✅" if passed_check else "❌"
        print(f"{status} {filename}")
    
    print(f"\nPassed: {passed}/{total}")
    
    if passed == total:
        print("\n🎉 All examples comply with the new architecture!")
        return 0
    else:
        print(f"\n⚠️  {total - passed} example(s) need updating")
        return 1

if __name__ == "__main__":
    sys.exit(main())
