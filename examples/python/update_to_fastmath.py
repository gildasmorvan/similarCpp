#!/usr/bin/env python3
"""
Update all examples to use FastMath instead of standard math module.

This script automatically replaces:
- import math -> from similar2logo.fastmath import sin, cos, atan2, sqrt, PI, TWO_PI
- math.sin -> sin
- math.cos -> cos
- math.atan2 -> atan2
- math.sqrt -> sqrt
- math.pi -> PI
- 2 * math.pi -> TWO_PI
"""

import os
import re
from pathlib import Path

def update_file(filepath):
    """Update a single file to use FastMath."""
    print(f"\nProcessing: {filepath.name}")
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    original_content = content
    changes = []
    
    # Check if file uses math module
    if 'import math' not in content and 'math.' not in content:
        print("  ⏭️  No math usage found")
        return False
    
    # Step 1: Add fastmath import if not present
    if 'from similar2logo.fastmath import' not in content:
        # Find where to add the import
        if 'from similar2logo.dsl import' in content:
            # Add after DSL import
            content = content.replace(
                'from similar2logo.dsl import',
                'from similar2logo.dsl import'
            )
            # Add fastmath import after similar2logo imports
            lines = content.split('\n')
            new_lines = []
            added = False
            for i, line in enumerate(lines):
                new_lines.append(line)
                if 'from similar2logo' in line and not added:
                    # Look ahead to find last similar2logo import
                    j = i + 1
                    while j < len(lines) and 'from similar2logo' in lines[j]:
                        new_lines.append(lines[j])
                        j += 1
                        i = j - 1
                    new_lines.append('from similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI')
                    added = True
                    changes.append("Added fastmath import")
            content = '\n'.join(new_lines)
        elif 'import math' in content:
            content = content.replace(
                'import math',
                'import math  # TODO: Replace with fastmath\nfrom similar2logo.fastmath import sin, cos, atan2, sqrt, normalize_angle, PI, TWO_PI'
            )
            changes.append("Added fastmath import")
    
    # Step 2: Replace math function calls
    replacements = [
        (r'math\.sin\(', 'sin('),
        (r'math\.cos\(', 'cos('),
        (r'math\.atan2\(', 'atan2('),
        (r'math\.sqrt\(', 'sqrt('),
        (r'2\s*\*\s*math\.pi\b', 'TWO_PI'),
        (r'math\.pi\b', 'PI'),
    ]
    
    for pattern, replacement in replacements:
        matches = re.findall(pattern, content)
        if matches:
            content = re.sub(pattern, replacement, content)
            changes.append(f"Replaced {len(matches)} occurrence(s) of {pattern}")
    
    # Step 3: Replace angle normalization patterns
    normalization_pattern = r'while\s+(\w+)\s*>\s*math\.pi:.*?while\s+\1\s*<\s*-math\.pi:.*?'
    if re.search(normalization_pattern, content, re.DOTALL):
        # This is complex, skip for now
        changes.append("⚠️  Manual review needed for angle normalization")
    
    # Write back if changed
    if content != original_content:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"  ✅ Updated:")
        for change in changes:
            print(f"     - {change}")
        return True
    else:
        print("  ℹ️  No changes needed")
        return False

def main():
    """Update all Python examples."""
    examples_dir = Path(__file__).parent
    
    print("=" * 60)
    print("Updating Examples to Use FastMath")
    print("=" * 60)
    
    # Find all Python files
    py_files = sorted(examples_dir.glob("*.py"))
    
    # Filter out this script and utility files
    py_files = [f for f in py_files if f.name not in [
        'update_to_fastmath.py',
        'verify_examples_architecture.py',
        'run_examples.py',
        '__init__.py'
    ]]
    
    updated = 0
    for filepath in py_files:
        try:
            if update_file(filepath):
                updated += 1
        except Exception as e:
            print(f"  ❌ Error: {e}")
    
    print("\n" + "=" * 60)
    print(f"Updated {updated}/{len(py_files)} files")
    print("=" * 60)
    print("\n✅ All examples now use FastMath where applicable!")
    print("   C++ optimizations will be used when available.")

if __name__ == "__main__":
    main()
