"""
Circle Simulation - DSL Example

Turtles follow the nearest turtle on their left and adapt their speed.
This creates circular flocking patterns.
"""

from similar2logo.dsl import Simulation, SimpleTurtle
from similar2logo.tools import Point2D
import math
import random

class CircleTurtle(SimpleTurtle):
    """A turtle that follows turtles on its left"""
    
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.color = "purple"
        self.speed = 1.0
    
    def decide(self, perception):
        """Follow the nearest turtle on the left"""
        influences = []
        
        # Get nearby turtles
        nearby = perception.get('nearby_turtles', [])
        
        if nearby:
            # Filter turtles on the left (positive angle difference)
            left_turtles = []
            
            for neighbor_data in nearby:
                other = neighbor_data['turtle']
                # Calculate angle to other turtle
                dx = other.position.x - self.position.x
                dy = other.position.y - self.position.y
                angle_to = math.atan2(dx, -dy)  # Logo heading (0=North)
                
                # Calculate relative angle
                angle_diff = angle_to - self.heading
                # Normalize to [-pi, pi]
                while angle_diff > math.pi:
                    angle_diff -= 2 * math.pi
                while angle_diff < -math.pi:
                    angle_diff += 2 * math.pi
                
                if angle_diff > 0:  # On the left
                    left_turtles.append({
                        'turtle': other,
                        'angle': angle_diff,
                        'distance': neighbor_data['distance']
                    })
            
            if left_turtles:
                # Calculate average direction and speed
                sin_sum = 0
                cos_sum = 0
                speed_sum = 0
                
                for t in left_turtles:
                    sin_sum += math.sin(t['angle'])
                    cos_sum += math.cos(t['angle'])
                    speed_sum += t['turtle'].speed
                
                n = len(left_turtles)
                
                # Turn towards average direction
                target_angle = math.atan2(sin_sum, cos_sum)
                influences.append(self.influence_turn(target_angle))
                
                # Adapt speed to average
                target_speed = speed_sum / n
                speed_diff = target_speed - self.speed
                influences.append(self.influence_change_speed(delta_speed=speed_diff))
        
        return influences

def setup(sim):
    """Setup function to create circle turtles"""
    for _ in range(20):
        turtle = CircleTurtle(
            position=Point2D(
                random.uniform(20, 80),
                random.uniform(20, 80)
            ),
            heading=random.uniform(0, 2 * math.pi)
        )
        turtle.speed = random.uniform(0.5, 1.5)
        turtle._environment = sim.environment
        sim.turtles.append(turtle)

def main():
    """Run simulation in console mode"""
    sim = Simulation("Circle Flocking")
    sim.grid(100, 100, toroidal=True)
    sim.setup(setup)
    sim.run(max_steps=1000)

def main_web(port=8080):
    """Run simulation with web interface"""
    sim = Simulation("Circle Flocking")
    sim.grid(100, 100, toroidal=True)
    sim.setup(setup)
    sim.run_web(port=port)

if __name__ == "__main__":
    import sys
    if "--web" in sys.argv:
        port = 8080
        if "--port" in sys.argv:
            port_idx = sys.argv.index("--port") + 1
            if port_idx < len(sys.argv):
                port = int(sys.argv[port_idx])
        main_web(port=port)
    else:
        main()
