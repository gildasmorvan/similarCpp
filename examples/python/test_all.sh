#!/bin/bash
# Test all Python DSL examples

export PYTHONPATH=$PYTHONPATH:$(pwd)/../../python

echo "=== Testing Python DSL Examples ==="
echo ""

examples=(
    "boids.py:200"
    "forest_fire.py:100"
    "randomwalk_1d.py:3000"
    "passive_turtle.py:1000"
    "circle.py:1000"
)

passed=0
failed=0

for entry in "${examples[@]}"; do
    IFS=':' read -r example steps <<< "$entry"
    echo "Testing $example ($steps steps)..."
    
    if timeout 30 python3 "$example" 2>&1 | grep -q "Simulation complete"; then
        echo "  ✅ PASSED"
        ((passed++))
    else
        echo "  ❌ FAILED"
        ((failed++))
    fi
    echo ""
done

echo "=== Results ==="
echo "Passed: $passed"
echo "Failed: $failed"
echo ""

if [ $failed -eq 0 ]; then
    echo "🎉 All tests passed!"
    exit 0
else
    echo "⚠️  Some tests failed"
    exit 1
fi
