import sys
import os

# Add the build directory to the python path
sys.path.append(os.path.abspath("../build"))

import jamfree

def test_bindings():
    print("Testing JamFree Perception/Decision Bindings...")

    # 1. Test Perception Model
    print("\n1. Testing VehiclePerceptionModelMicro...")
    perception = jamfree.VehiclePerceptionModelMicro(200.0)
    print(f"  Created perception model with range 200.0")

    # 2. Test IDM and MOBIL (dependencies)
    print("\n2. Testing IDM and MOBIL...")
    idm = jamfree.IDM()
    mobil = jamfree.MOBIL()
    print("  Created IDM and MOBIL models")

    # 3. Test DMS Classes
    print("\n3. Testing DMS Classes...")
    
    # ForwardAccelerationDMS
    accel_dms = jamfree.ForwardAccelerationDMS(idm)
    print("  Created ForwardAccelerationDMS")
    
    # LaneChangeDMS
    lane_change_dms = jamfree.LaneChangeDMS(mobil, idm)
    print("  Created LaneChangeDMS")
    
    # ConjunctionDMS
    conjunction = jamfree.ConjunctionDMS()
    conjunction.add_submodel(accel_dms)
    conjunction.add_submodel(lane_change_dms)
    print("  Created ConjunctionDMS and added submodels")
    
    # SubsumptionDMS
    subsumption = jamfree.SubsumptionDMS()
    subsumption.add_submodel(lane_change_dms) # Higher priority
    subsumption.add_submodel(accel_dms)       # Lower priority
    print("  Created SubsumptionDMS and added submodels")

    # 4. Test Decision Model
    print("\n4. Testing VehicleDecisionModelMicro...")
    decision = jamfree.VehicleDecisionModelMicro(conjunction)
    print("  Created VehicleDecisionModelMicro with ConjunctionDMS")
    
    decision2 = jamfree.VehicleDecisionModelMicro(subsumption)
    print("  Created VehicleDecisionModelMicro with SubsumptionDMS")

    # 5. Test VehicleAgent Integration
    print("\n5. Testing VehicleAgent Integration...")
    agent = jamfree.VehicleAgent("test_agent")
    print("  Created VehicleAgent")
    
    agent.set_models("Microscopic", perception, decision)
    print("  Set perception and decision models for Microscopic level")
    
    # Create and set states
    public_state = jamfree.VehiclePublicLocalStateMicro("test_agent")
    private_state = jamfree.VehiclePrivateLocalStateMicro("test_agent")
    
    agent.set_states("Microscopic", public_state, private_state)
    print("  Set public and private local states for Microscopic level")

    print("\nAll binding tests passed!")

if __name__ == "__main__":
    try:
        test_bindings()
    except Exception as e:
        print(f"\nFAILED: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
