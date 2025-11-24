"""
Registry of available SIMILAR2Logo example models.
"""

import os
import glob

def get_available_models():
    """
    Get list of available example models.
    
    Returns:
        List of dicts with model information
    """
    examples_dir = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'examples', 'python')
    
    models = []
    
    # Define known models with descriptions
    known_models = {
        'simple_random_walk.py': {
            'name': 'Simple Random Walk',
            'description': 'Single turtle performing random walk',
            'category': 'Basic'
        },
        'randomwalk_1d.py': {
            'name': 'Random Walk 1D',
            'description': 'Turtle constrained to 1D movement',
            'category': 'Basic'
        },
        'passive_turtle.py': {
            'name': 'Passive Turtle',
            'description': 'Physics-based movement without decisions',
            'category': 'Basic'
        },
        'boids_dsl.py': {
            'name': 'Boids (Flocking)',
            'description': 'Classic flocking behavior',
            'category': 'Collective Behavior'
        },
        'boids_obstacles.py': {
            'name': 'Boids with Obstacles',
            'description': 'Flocking with obstacle avoidance',
            'category': 'Collective Behavior'
        },
        'circle.py': {
            'name': 'Circle Formation',
            'description': 'Agents forming circular patterns',
            'category': 'Collective Behavior'
        },
        'ant_foraging_dsl.py': {
            'name': 'Ant Foraging',
            'description': 'Ants finding food using pheromones',
            'category': 'Swarm Intelligence'
        },
        'pheromone_following.py': {
            'name': 'Pheromone Following',
            'description': 'Agents following chemical trails',
            'category': 'Swarm Intelligence'
        },
        'slime_mold.py': {
            'name': 'Slime Mold',
            'description': 'Network formation simulation',
            'category': 'Swarm Intelligence'
        },
        'predator_prey.py': {
            'name': 'Predator-Prey',
            'description': 'Lotka-Volterra population dynamics',
            'category': 'Ecology'
        },
        'virus_spread.py': {
            'name': 'Virus Spread',
            'description': 'SIR epidemiology model',
            'category': 'Ecology'
        },
        'forest_fire.py': {
            'name': 'Forest Fire',
            'description': 'Fire spreading through forest',
            'category': 'Ecology'
        },
        'heatbugs.py': {
            'name': 'Heatbugs',
            'description': 'Temperature-seeking agents',
            'category': 'Self-Organization'
        },
        'segregation_model.py': {
            'name': 'Segregation',
            'description': "Schelling's segregation model",
            'category': 'Social Dynamics'
        },
        'turmite.py': {
            'name': 'Turmite',
            'description': "Langton's Ant",
            'category': 'Cellular Automata'
        },
        'multiturmite.py': {
            'name': 'Multi-Turmite',
            'description': 'Multiple Langton\'s Ants',
            'category': 'Cellular Automata'
        },
        'firework.py': {
            'name': 'Firework',
            'description': 'Particle system with physics',
            'category': 'Visual Effects'
        },
        'transport.py': {
            'name': 'Transport',
            'description': 'Traffic simulation',
            'category': 'Urban Systems'
        },
    }
    
    # Scan for Python files
    if os.path.exists(examples_dir):
        for filename in os.listdir(examples_dir):
            if filename.endswith('.py') and not filename.startswith('_') and not filename.startswith('run_'):
                filepath = os.path.join(examples_dir, filename)
                
                # Get model info
                if filename in known_models:
                    info = known_models[filename].copy()
                else:
                    # Unknown model - use filename
                    info = {
                        'name': filename.replace('_', ' ').replace('.py', '').title(),
                        'description': 'Custom simulation',
                        'category': 'Other'
                    }
                
                info['filename'] = filename
                info['filepath'] = filepath
                models.append(info)
    
    # Sort by category then name
    models.sort(key=lambda x: (x['category'], x['name']))
    
    return models


def get_model_code(filename):
    """
    Get the source code for a model.
    
    Args:
        filename: Name of the model file
        
    Returns:
        Source code as string, or None if not found
    """
    examples_dir = os.path.join(os.path.dirname(__file__), '..', '..', '..', 'examples', 'python')
    filepath = os.path.join(examples_dir, filename)
    
    if os.path.exists(filepath):
        with open(filepath, 'r') as f:
            return f.read()
    
    return None
