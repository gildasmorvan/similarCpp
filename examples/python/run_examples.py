#!/usr/bin/env python3
"""
SIMILAR2Logo Examples Launcher

Interactive menu to run any of the available examples.
"""

import sys
import os


EXAMPLES = {
    '1': {
        'name': 'Simple Random Walk',
        'file': 'simple_random_walk.py',
        'description': 'Basic turtle random walk demonstration',
        'difficulty': '⭐ Beginner',
        'web': False
    },
    '2': {
        'name': 'Pheromone Following',
        'file': 'pheromone_following.py',
        'description': 'Gradient-based navigation with sensors',
        'difficulty': '⭐⭐ Intermediate',
        'web': False
    },
    '3': {
        'name': 'Boids Flocking',
        'file': 'boids_flocking.py',
        'description': 'Emergent flocking behavior (Reynolds 1987)',
        'difficulty': '⭐⭐⭐ Advanced',
        'web': True
    },
    '4': {
        'name': 'Segregation Model',
        'file': 'segregation_model.py',
        'description': 'Schelling\'s segregation model (1971)',
        'difficulty': '⭐⭐ Intermediate',
        'web': True
    },
    '5': {
        'name': 'Ant Colony Foraging',
        'file': 'ant_foraging.py',
        'description': 'Stigmergy and pheromone trails',
        'difficulty': '⭐⭐⭐ Advanced',
        'web': True
    },
    '6': {
        'name': 'Predator-Prey',
        'file': 'predator_prey.py',
        'description': 'Lotka-Volterra population dynamics',
        'difficulty': '⭐⭐⭐ Advanced',
        'web': True
    },
    '7': {
        'name': 'Slime Mold Aggregation',
        'file': 'slime_mold.py',
        'description': 'cAMP signaling and chemotaxis',
        'difficulty': '⭐⭐⭐⭐ Expert',
        'web': True
    },
    '8': {
        'name': 'Web Demo',
        'file': 'web_demo.py',
        'description': 'Interactive web interface demonstration',
        'difficulty': '⭐ Beginner',
        'web': True
    },
}


def print_banner():
    """Print welcome banner."""
    print("=" * 70)
    print(" " * 15 + "🐢 SIMILAR2Logo Examples Launcher 🐢")
    print("=" * 70)
    print()


def print_menu():
    """Print example selection menu."""
    print("Available Examples:")
    print()
    
    for key, example in sorted(EXAMPLES.items()):
        web_indicator = " [WEB]" if example['web'] else ""
        print(f"  {key}. {example['name']}{web_indicator}")
        print(f"     {example['description']}")
        print(f"     Difficulty: {example['difficulty']}")
        print()
    
    print("  q. Quit")
    print()


def run_example(example_key, web_mode=False):
    """Run the selected example."""
    if example_key not in EXAMPLES:
        print(f"❌ Invalid selection: {example_key}")
        return
    
    example = EXAMPLES[example_key]
    
    print("=" * 70)
    print(f"Running: {example['name']}")
    print(f"File: {example['file']}")
    if web_mode and example['web']:
        print("Mode: Web Interface")
    else:
        print("Mode: Offline (matplotlib)")
    print("=" * 70)
    print()
    
    # Build command
    script_dir = os.path.dirname(os.path.abspath(__file__))
    script_path = os.path.join(script_dir, example['file'])
    
    if web_mode and example['web']:
        cmd = f"python {script_path} --web"
    else:
        cmd = f"python {script_path}"
    
    # Run
    print(f"Executing: {cmd}")
    print()
    os.system(cmd)


def interactive_mode():
    """Run in interactive menu mode."""
    while True:
        print_banner()
        print_menu()
        
        choice = input("Select an example (1-8) or 'q' to quit: ").strip().lower()
        
        if choice == 'q':
            print("\n👋 Goodbye!")
            break
        
        if choice not in EXAMPLES:
            print(f"\n❌ Invalid choice: {choice}")
            input("\nPress Enter to continue...")
            continue
        
        example = EXAMPLES[choice]
        
        # Ask about web mode if supported
        web_mode = False
        if example['web']:
            web_choice = input("\nRun with web interface? (y/n) [n]: ").strip().lower()
            web_mode = web_choice == 'y'
        
        print()
        run_example(choice, web_mode)
        
        print()
        input("\nPress Enter to return to menu...")


def main():
    """Main entry point."""
    # Check for command line arguments
    if len(sys.argv) > 1:
        # Direct execution mode
        example_key = sys.argv[1]
        web_mode = '--web' in sys.argv
        
        if example_key in EXAMPLES:
            run_example(example_key, web_mode)
        else:
            print(f"❌ Unknown example: {example_key}")
            print("\nAvailable examples:")
            for key, example in sorted(EXAMPLES.items()):
                print(f"  {key}: {example['name']}")
            sys.exit(1)
    else:
        # Interactive mode
        try:
            interactive_mode()
        except KeyboardInterrupt:
            print("\n\n👋 Interrupted. Goodbye!")
            sys.exit(0)


if __name__ == "__main__":
    main()
