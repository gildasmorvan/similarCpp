"""
Parallelism Benchmark - Compare Sequential, Threading, and Multiprocessing

This benchmark demonstrates the performance differences between:
1. Sequential execution (no parallelism)
2. Threading (limited by GIL)
3. Multiprocessing (true parallelism)

Run this to see which backend is best for your use case.
"""

import time
from similar2logo.dsl import Simulation, Turtle
from similar2logo.fastmath import sin, cos, atan2, normalize_angle, PI
from dataclasses import dataclass
import random


@dataclass
class BenchmarkParams:
    repulsion_distance: float = 1.0
    orientation_distance: float = 2.0
    attraction_distance: float = 4.0
    repulsion_weight: float = 10.0
    orientation_weight: float = 20.0
    attraction_weight: float = 0.1
    max_turn_angle: float = PI / 4
    max_initial_speed: float = 2.0
    min_initial_speed: float = 1.0
    grid_size: int = 100


class BenchmarkBoid(Turtle):
    """Boid with CPU-intensive decision logic for benchmarking."""
    
    def __init__(self, params=None, position=None, heading=None, **kwargs):
        if params is None:
            params = kwargs.get('params', BenchmarkParams())
        self.params = params
        
        from similar2logo.tools import Point2D
        from similar2logo.fastmath import TWO_PI
        
        if position is None:
            position = Point2D(random.random() * params.grid_size,
                             random.random() * params.grid_size)
        if heading is None:
            heading = random.random() * TWO_PI
            
        super().__init__(position=position, heading=heading, color='blue', **kwargs)
        self.speed = (self.params.min_initial_speed + 
                     random.random() * (self.params.max_initial_speed - self.params.min_initial_speed))
    
    def decide(self, perception):
        """Boids algorithm with weighted mean angle calculation."""
        neighbors = perception.get('nearby_turtles', [])
        influences = []
        
        if neighbors:
            total_weight = 0.0
            weighted_sin = 0.0
            weighted_cos = 0.0
            orientation_speed = 0.0
            nb_turtles_in_orientation = 0
            
            for neighbor_info in neighbors:
                distance = neighbor_info['distance']
                neighbor = neighbor_info['turtle']
                direction_to = neighbor_info.get('direction', 
                    atan2(neighbor.position.x - self.position.x,
                          -(neighbor.position.y - self.position.y)))
                
                if distance <= self.params.repulsion_distance:
                    angle_diff = normalize_angle(self.heading - direction_to)
                    weight = self.params.repulsion_weight
                    weighted_sin += sin(angle_diff) * weight
                    weighted_cos += cos(angle_diff) * weight
                    total_weight += weight
                elif distance <= self.params.orientation_distance:
                    angle_diff = normalize_angle(neighbor.heading - self.heading)
                    weight = self.params.orientation_weight
                    weighted_sin += sin(angle_diff) * weight
                    weighted_cos += cos(angle_diff) * weight
                    total_weight += weight
                    orientation_speed += neighbor.speed - self.speed
                    nb_turtles_in_orientation += 1
                elif distance <= self.params.attraction_distance:
                    angle_diff = normalize_angle(direction_to - self.heading)
                    weight = self.params.attraction_weight
                    weighted_sin += sin(angle_diff) * weight
                    weighted_cos += cos(angle_diff) * weight
                    total_weight += weight
            
            if total_weight > 0:
                mean_angle = atan2(weighted_sin, weighted_cos)
                if abs(mean_angle) > 1e-10:
                    if mean_angle > self.params.max_turn_angle:
                        mean_angle = self.params.max_turn_angle
                    elif mean_angle < -self.params.max_turn_angle:
                        mean_angle = -self.params.max_turn_angle
                    influences.append(self.influence_turn(mean_angle))
            
            if nb_turtles_in_orientation > 0:
                avg_speed_diff = orientation_speed / nb_turtles_in_orientation
                influences.append(self.influence_change_speed(avg_speed_diff))
        
        influences.append(self.influence_move_forward(self.speed))
        return influences


def run_benchmark(backend, num_agents, num_steps, num_workers=None):
    """Run a benchmark with specified configuration."""
    params = BenchmarkParams()
    
    start_time = time.time()
    
    sim = (Simulation(f'Benchmark-{backend or "sequential"}')
           .grid(params.grid_size, params.grid_size, toroidal=True)
           .parallel(backend=backend, num_workers=num_workers)
           .agents(BenchmarkBoid, count=num_agents, params=params))
    
    sim.run(max_steps=num_steps)
    
    elapsed = time.time() - start_time
    return elapsed


def main():
    """Run comprehensive parallelism benchmarks."""
    print('=' * 70)
    print('SIMILAR2Logo Parallelism Benchmark')
    print('=' * 70)
    print()
    
    # Configuration
    num_agents = 500
    num_steps = 20
    num_workers = 4
    
    print(f'Configuration:')
    print(f'  Agents: {num_agents}')
    print(f'  Steps: {num_steps}')
    print(f'  Workers (for parallel): {num_workers}')
    print()
    
    results = {}
    
    # Benchmark 1: Sequential
    print('Running Sequential (no parallelism)...')
    results['sequential'] = run_benchmark(None, num_agents, num_steps)
    print(f'  Time: {results["sequential"]:.2f}s')
    print()
    
    # Benchmark 2: Threading
    print('Running Threading (limited by GIL)...')
    results['thread'] = run_benchmark('thread', num_agents, num_steps, num_workers)
    print(f'  Time: {results["thread"]:.2f}s')
    print()
    
    # Benchmark 3: Multiprocessing
    print('Running Multiprocessing (true parallelism)...')
    results['process'] = run_benchmark('process', num_agents, num_steps, num_workers)
    print(f'  Time: {results["process"]:.2f}s')
    print()
    
    # Results
    print('=' * 70)
    print('Results Summary')
    print('=' * 70)
    print()
    
    baseline = results['sequential']
    
    for backend, elapsed in results.items():
        speedup = baseline / elapsed
        print(f'{backend:15s}: {elapsed:6.2f}s  (speedup: {speedup:.2f}x)')
    
    print()
    print('=' * 70)
    print('Analysis')
    print('=' * 70)
    print()
    
    if results['thread'] < baseline * 0.95:
        print('✅ Threading provides some benefit (likely I/O or coordination)')
    else:
        print('⚠️  Threading shows no speedup (GIL-limited CPU-bound work)')
    
    if results['process'] < baseline * 0.7:
        print('✅ Multiprocessing provides significant speedup (true parallelism)')
    else:
        print('⚠️  Multiprocessing overhead may be too high for this workload')
    
    print()
    print('Recommendations:')
    print()
    
    if results['process'] < results['thread']:
        print('  • Use backend="process" for this simulation')
        print(f'  • Expected speedup: {baseline/results["process"]:.1f}x')
    elif results['thread'] < baseline:
        print('  • Use backend="thread" for this simulation')
        print(f'  • Expected speedup: {baseline/results["thread"]:.1f}x')
    else:
        print('  • Use backend=None (sequential) for this simulation')
        print('  • Parallel overhead exceeds benefits')
    
    print()


if __name__ == '__main__':
    main()
