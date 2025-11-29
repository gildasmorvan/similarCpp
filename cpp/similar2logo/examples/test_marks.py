
import sys
import os

# Add the build directory to the path so we can import the module
# Found at ../../build/similar2logo/python/_core.cpython-314-darwin.so
build_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../build/similar2logo/python'))
print(f"Build path: {build_path}")
if os.path.exists(build_path):
    print(f"Contents of build path: {os.listdir(build_path)}")
else:
    print(f"Build path does not exist!")

# Remove conflicting paths
sys.path = [p for p in sys.path if 'similar/python' not in p]

sys.path.insert(0, build_path)
print(f"sys.path: {sys.path}")

try:
    import _core
    print(f"Imported _core from: {_core.__file__}")
    print(f"Available in _core: {dir(_core)}")

    from _core import (
        Point2D,
        SimpleMark,
        LevelIdentifier,
        SimulationTimeStamp,
        environment,
        influences
    )
    print("Successfully imported _core modules")
    import _core
    print(f"Imported _core from: {_core.__file__}")
    print(f"Available in _core: {dir(_core)}")
    if 'environment' in dir(_core):
        print(f"Available in _core.environment: {dir(_core.environment)}")
    
    # Try importing from environment just in case
    try:
        from _core.environment import SimpleMark
        print("Imported SimpleMark from _core.environment")
    except ImportError:
        try:
            from _core import SimpleMark
            print("Imported SimpleMark from _core")
        except ImportError:
            print("Could not import SimpleMark from anywhere")
            raise
    print("Successfully imported _core modules")
except ImportError as e:
    print(f"Failed to import _core: {e}")
    sys.exit(1)

def test_marks():
    print("Testing Marks and DropMark...")

    # 1. Create Environment
    width = 100
    height = 100
    env = environment.Environment(width, height, False)
    print("Environment created.")

    # 2. Create a SimpleMark
    loc = Point2D(10.5, 20.5)
    mark = SimpleMark(loc, "test_category")
    print(f"Mark created at ({mark.get_location().x}, {mark.get_location().y}) with category '{mark.get_category()}'")

    # 3. Verify Mark properties
    assert mark.get_location().x == 10.5
    assert mark.get_location().y == 20.5
    assert mark.get_category() == "test_category"

    # 4. Create DropMark influence
    t1 = SimulationTimeStamp(0)
    t2 = SimulationTimeStamp(1)
    drop_mark = influences.DropMark(t1, t2, mark)
    print("DropMark influence created.")

    # 5. Add mark to environment
    env.add_mark(10, 20, mark)
    print("Mark added to environment.")

    # 6. Verify get_marks
    marks_grid = env.get_marks()
    # marks_grid is a vector<vector<unordered_set<SimpleMark>>>
    # Accessing [10][20] should contain our mark
    marks_at_loc = marks_grid[10][20]
    print(f"Found {len(marks_at_loc)} marks at (10, 20)")
    
    found = False
    for m in marks_at_loc:
        if m.get_category() == "test_category":
            found = True
            break
    
    assert found
    print("Mark successfully retrieved from environment!")
    
    print("Verification successful.")

if __name__ == "__main__":
    test_marks()
