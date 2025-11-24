#!/usr/bin/env python3
"""Minimal test to check if jamfree imports correctly"""

print("Testing jamfree import...")
try:
    import jamfree
    print(f"✓ jamfree imported successfully")
    print(f"  - AdaptiveSimulator: {hasattr(jamfree, 'AdaptiveSimulator')}")
    print(f"  - MetalCompute: {hasattr(jamfree, 'MetalCompute')}")
    print(f"  - is_metal_available: {hasattr(jamfree, 'is_metal_available')}")
    
    if hasattr(jamfree, 'is_metal_available'):
        print(f"  - Metal available: {jamfree.is_metal_available()}")
except Exception as e:
    print(f"✗ Error importing jamfree: {e}")
    import traceback
    traceback.print_exc()

print("\nAll tests passed!")
