"""
Branching Timelines DSL Example - Random Agent with Multiple Futures

This example demonstrates a single agent moving randomly in the environment,
with branching timelines that allow analysis of multiple futures using the SIMILAR DSL.

The DSL provides a clean, declarative API for setting up simulations, while
branching timelines enable exploring alternative futures from the same starting point.
"""

import sys
import random
import math
sys.path.insert(0, '../../../build/lib')

from similar2logo.dsl import Simulation, SimpleTurtle


class RandomTurtle(SimpleTurtle):
    """
    A turtle that moves randomly, changing direction occasionally.

    This demonstrates basic agent behavior using the DSL.
    """

    def __init__(self, behavior_seed=None, **kwargs):
        super().__init__(**kwargs)
        self.step_count = 0
        self.color = "blue"
        self.behavior_seed = behavior_seed
        if behavior_seed is not None:
            self.rng = random.Random(behavior_seed)
        else:
            self.rng = random

    def decide(self, perception):
        """
        Make random movement decisions.

        Returns a list of influences to apply.
        """
        influences = []

        # Occasionally change direction (20% chance per step)
        if self.rng.random() < 0.2:
            # Turn by a random angle (-45 to +45 degrees)
            turn_angle = self.rng.uniform(-math.pi/4, math.pi/4)
            influences.append(self.influence_turn(turn_angle))

        # Always try to move forward
        influences.append(self.influence_move_forward(1.0))

        self.step_count += 1
        return influences


def run_single_branch(sim_instance, branch_name, max_steps=15):
    """
    Run a single simulation branch and collect position data.

    Args:
        sim_instance: LogoSimulation instance
        branch_name: Name for this branch
        max_steps: Maximum steps to run

    Returns:
        List of (x, y) positions for the agent
    """
    print(f"Running {branch_name}...")

    # Get the single agent
    agent = sim_instance.turtles[0]
    positions = [(agent.position.x, agent.position.y)]

    for step in range(max_steps):
        sim_instance.step()

        # Record position
        pos = agent.position
        positions.append((pos.x, pos.y))

        if step % 5 == 0:
            print(f"  Step {step}: Position ({pos.x:.1f}, {pos.y:.1f})")

    print(f"  Final position: ({positions[-1][0]:.1f}, {positions[-1][1]:.1f})")
    return positions


def create_branch_simulation(original_sim, behavior_seed):
    """
    Create a branch simulation with modified agent behavior.

    Args:
        original_sim: Original LogoSimulation to clone
        behavior_seed: Random seed for behavior variation

    Returns:
        New LogoSimulation instance with modified agent
    """
    # Create a deep copy of the simulation
    branch_sim = original_sim.__class__(
        environment=original_sim.environment,
        parallel_backend=original_sim.parallel_backend,
        num_workers=original_sim.num_workers
    )

    # Copy turtles with modified behavior
    for turtle in original_sim.turtles:
        if hasattr(turtle, 'behavior_seed'):
            # Create new turtle with different behavior seed
            new_turtle = RandomTurtle(
                position=turtle.position,
                heading=turtle.heading,
                behavior_seed=behavior_seed
            )
            new_turtle.speed = turtle.speed
            new_turtle.color = turtle.color
            branch_sim.turtles.append(new_turtle)
        else:
            # For other turtle types, just copy
            branch_sim.turtles.append(turtle)

    # Copy other state
    branch_sim.current_step = original_sim.current_step
    branch_sim.reaction_model = original_sim.reaction_model

    return branch_sim


def main():
    print("=" * 70)
    print("Branching Timelines DSL Example - Random Agent Multiple Futures")
    print("=" * 70)
    print()

    # Create base simulation with DSL
    print("1. Creating base simulation...")
    sim = (Simulation("Random Agent Branching")
           .grid(100, 100, toroidal=True)
           .agents(RandomTurtle, count=1, behavior_seed=42))

    # Build the simulation
    base_sim = sim._build_simulation()
    print("   ✓ DSL simulation created with 1 random agent")
    print()

    # Run initial steps to establish baseline
    print("2. Running initial steps...")
    for _ in range(5):
        base_sim.step()
    initial_agent_pos = base_sim.turtles[0].position
    print(f"   ✓ Ran 5 initial steps, agent at ({initial_agent_pos.x:.1f}, {initial_agent_pos.y:.1f})")
    print()

    # Create branching points - create different behavior variations
    print("3. Creating branching timelines...")

    # Branch 1: Conservative turning (same seed as original)
    branch1_sim = create_branch_simulation(base_sim, behavior_seed=42)
    print("   ✓ Branch 1: Conservative behavior (seed=42)")

    # Branch 2: More aggressive turning
    branch2_sim = create_branch_simulation(base_sim, behavior_seed=123)
    print("   ✓ Branch 2: Aggressive turning (seed=123)")

    # Branch 3: Straight-line behavior
    branch3_sim = create_branch_simulation(base_sim, behavior_seed=999)
    print("   ✓ Branch 3: Straight-line behavior (seed=999)")
    print()

    # Run all branches
    print("4. Running all branches...")

    # Original branch continuation
    original_positions = run_single_branch(base_sim, "Original Branch")
    print()

    # Alternative branches
    branch1_positions = run_single_branch(branch1_sim, "Branch 1")
    print()
    branch2_positions = run_single_branch(branch2_sim, "Branch 2")
    print()
    branch3_positions = run_single_branch(branch3_sim, "Branch 3")
    print()

    print("=" * 70)
    print("BRANCHING ANALYSIS COMPLETE")
    print("=" * 70)
    print()

    # Analyze the different paths
    print("Path Analysis:")

    # Calculate net displacement for each branch
    def calculate_net_displacement(positions):
        start_x, start_y = positions[0]
        end_x, end_y = positions[-1]
        return math.sqrt((end_x - start_x)**2 + (end_y - start_y)**2)

    disp_original = calculate_net_displacement(original_positions)
    disp_branch1 = calculate_net_displacement(branch1_positions)
    disp_branch2 = calculate_net_displacement(branch2_positions)
    disp_branch3 = calculate_net_displacement(branch3_positions)

    print(".1f")
    print(".1f")
    print(".1f")
    print(".1f")

    # Calculate distances traveled
    def calculate_total_distance(positions):
        total_dist = 0.0
        for i in range(1, len(positions)):
            dx = positions[i][0] - positions[i-1][0]
            dy = positions[i][1] - positions[i-1][1]
            total_dist += math.sqrt(dx*dx + dy*dy)
        return total_dist

    dist_original = calculate_total_distance(original_positions)
    dist_branch1 = calculate_total_distance(branch1_positions)
    dist_branch2 = calculate_total_distance(branch2_positions)
    dist_branch3 = calculate_total_distance(branch3_positions)

    print("\nDistance Analysis:")
    print(dist_original)
    print(dist_branch1)
    print(dist_branch2)
    print(dist_branch3)

    # Calculate path efficiency (net displacement / total distance)
    eff_original = disp_original / dist_original if dist_original > 0 else 0
    eff_branch1 = disp_branch1 / dist_branch1 if dist_branch1 > 0 else 0
    eff_branch2 = disp_branch2 / dist_branch2 if dist_branch2 > 0 else 0
    eff_branch3 = disp_branch3 / dist_branch3 if dist_branch3 > 0 else 0

    print("\nPath Efficiency (Net/Total):")
    print(eff_original)
    print(eff_branch1)
    print(eff_branch2)
    print(eff_branch3)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"ERROR: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
