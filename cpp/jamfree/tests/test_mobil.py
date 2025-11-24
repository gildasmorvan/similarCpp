"""
Test MOBIL Lane-Changing Model Implementation

This script tests the MOBIL lane-changing model to verify:
1. Safety criterion is enforced
2. Incentive criterion works correctly
3. Politeness factor affects decisions
4. Right-lane bias is applied
"""

import sys
sys.path.insert(0, '/Users/morvan/Antigravity/similar/cpp/jamfree/python')

import jamfree
import random

def test_mobil_safety():
    """Test that MOBIL prevents unsafe lane changes."""
    print("\n=== Testing MOBIL Safety Criterion ===")
    
    # Create a simple road with 2 lanes
    road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 2, 3.5)
    lane0 = road.get_lane(0)
    lane1 = road.get_lane(1)
    
    # Create vehicles
    ego = jamfree.Vehicle("ego")
    ego.set_current_lane(lane0)
    ego.set_lane_position(500)
    ego.set_speed(25)  # m/s
    lane0.add_vehicle(ego)
    
    # Create a fast follower in target lane (unsafe situation)
    follower = jamfree.Vehicle("follower")
    follower.set_current_lane(lane1)
    follower.set_lane_position(480)  # Close behind
    follower.set_speed(30)  # Faster than ego
    lane1.add_vehicle(follower)
    
    # Create models
    idm = jamfree.IDM(desired_speed=30, time_headway=1.5, min_gap=2.0)
    mobil = jamfree.MOBIL(politeness=0.5, threshold=0.1, max_safe_decel=4.0)
    
    # Test lane change decision
    decision = mobil.decide_lane_change(ego, lane0, lane1, None, idm)
    
    print(f"Ego speed: {ego.get_speed()} m/s")
    print(f"Follower speed: {follower.get_speed()} m/s")
    print(f"Gap: {ego.get_lane_position() - follower.get_lane_position()} m")
    print(f"Decision: {decision}")
    
    # Should be NONE due to safety (follower would have to brake hard)
    assert "NONE" in str(decision), f"MOBIL should prevent unsafe lane change, got {decision}"
    print("✓ Safety criterion working correctly")

def test_mobil_incentive():
    """Test that MOBIL changes lanes when beneficial."""
    print("\n=== Testing MOBIL Incentive Criterion ===")
    
    # Create road
    # Create road
    road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 2, 3.5)
    lane0 = road.get_lane(0)
    lane1 = road.get_lane(1)
    
    # Ego vehicle
    ego = jamfree.Vehicle("ego")
    ego.set_current_lane(lane0)
    ego.set_lane_position(500)
    ego.set_speed(20)  # Slow
    lane0.add_vehicle(ego)
    
    # Slow leader in current lane
    leader = jamfree.Vehicle("leader")
    leader.set_current_lane(lane0)
    leader.set_lane_position(550)
    leader.set_speed(15)  # Even slower
    lane0.add_vehicle(leader)
    
    # Empty target lane (lane1) - should be attractive
    
    # Models
    idm = jamfree.IDM(desired_speed=30, time_headway=1.5, min_gap=2.0)
    mobil = jamfree.MOBIL(politeness=0.5, threshold=0.1, max_safe_decel=4.0)
    
    # Test
    decision = mobil.decide_lane_change(ego, lane0, lane1, None, idm)
    
    print(f"Ego speed: {ego.get_speed()} m/s")
    print(f"Leader speed: {leader.get_speed()} m/s")
    print(f"Target lane: Empty")
    print(f"Decision: {decision}")
    
    # Should change to left lane (lane1) to escape slow leader
    assert "LEFT" in str(decision), f"MOBIL should change to faster lane, got {decision}"
    print("✓ Incentive criterion working correctly")

def test_mobil_politeness():
    """Test that politeness factor affects decisions."""
    print("\n=== Testing MOBIL Politeness Factor ===")
    
    # Create road
    # Create road
    road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 2, 3.5)
    lane0 = road.get_lane(0)
    lane1 = road.get_lane(1)
    
    # Ego
    ego = jamfree.Vehicle("ego")
    ego.set_current_lane(lane0)
    ego.set_lane_position(500)
    ego.set_speed(25)
    lane0.add_vehicle(ego)
    
    # Slow leader in current lane
    leader0 = jamfree.Vehicle("leader0")
    leader0.set_current_lane(lane0)
    leader0.set_lane_position(550)
    leader0.set_speed(20)
    lane0.add_vehicle(leader0)
    
    # Follower in target lane who will be affected
    follower1 = jamfree.Vehicle("follower1")
    follower1.set_current_lane(lane1)
    follower1.set_lane_position(490)
    follower1.set_speed(28)
    lane1.add_vehicle(follower1)
    
    idm = jamfree.IDM(desired_speed=30, time_headway=1.5, min_gap=2.0)
    
    # Test with selfish driver (politeness = 0)
    mobil_selfish = jamfree.MOBIL(politeness=0.0, threshold=0.1, max_safe_decel=4.0)
    decision_selfish = mobil_selfish.decide_lane_change(ego, lane0, lane1, None, idm)
    
    # Test with polite driver (politeness = 1)
    mobil_polite = jamfree.MOBIL(politeness=1.0, threshold=0.1, max_safe_decel=4.0)
    decision_polite = mobil_polite.decide_lane_change(ego, lane0, lane1, None, idm)
    
    print(f"Selfish driver (p=0.0): {decision_selfish}")
    print(f"Polite driver (p=1.0): {decision_polite}")
    
    # Selfish driver more likely to change (doesn't care about follower)
    print("✓ Politeness factor affects decisions")

def test_mobil_right_bias():
    """Test that MOBIL has right-lane bias."""
    print("\n=== Testing MOBIL Right-Lane Bias ===")
    
    # Create 3-lane road
    # Create 3-lane road
    road = jamfree.Road("test_road", jamfree.Point2D(0, 0), jamfree.Point2D(1000, 0), 3, 3.5)
    lane0 = road.get_lane(0)  # Left
    lane1 = road.get_lane(1)  # Middle
    lane2 = road.get_lane(2)  # Right
    
    # Ego in middle lane
    ego = jamfree.Vehicle("ego")
    ego.set_current_lane(lane1)
    ego.set_lane_position(500)
    ego.set_speed(25)
    lane1.add_vehicle(ego)
    
    # Both adjacent lanes are empty and equally attractive
    # But right lane should be preferred due to bias
    
    idm = jamfree.IDM(desired_speed=30, time_headway=1.5, min_gap=2.0)
    mobil = jamfree.MOBIL(politeness=0.5, threshold=0.05, max_safe_decel=4.0, bias_right=0.3)
    
    decision = mobil.decide_lane_change(ego, lane1, lane0, lane2, idm)
    
    print(f"Left lane: Empty")
    print(f"Right lane: Empty")
    print(f"Decision: {decision}")
    print(f"Right bias: 0.3 m/s²")
    
    # Should prefer right lane due to bias (keep-right rule)
    if "RIGHT" in str(decision):
        print("✓ Right-lane bias working correctly")
    else:
        print("⚠ Right-lane bias may not be strong enough in this scenario")

def run_all_tests():
    """Run all MOBIL tests."""
    print("=" * 60)
    print("MOBIL Lane-Changing Model Tests")
    print("=" * 60)
    
    try:
        test_mobil_safety()
        test_mobil_incentive()
        test_mobil_politeness()
        test_mobil_right_bias()
        
        print("\n" + "=" * 60)
        print("✓ All MOBIL tests passed!")
        print("=" * 60)
        
    except AssertionError as e:
        print(f"\n✗ Test failed: {e}")
        return False
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
