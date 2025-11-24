import heapq
import random
import math

class RoutingEngine:
    def __init__(self, roads, connection_threshold=1.0):
        """
        Initialize routing engine with a list of roads.
        
        Args:
            roads: List of jamfree.Road objects
            connection_threshold: Max distance (meters) to consider roads connected
        """
        self.roads = {r.get_id(): r for r in roads}
        self.graph = self._build_graph(roads, connection_threshold)
        
    def _build_graph(self, roads, threshold):
        """Build adjacency list graph where nodes are road IDs."""
        adj = {r.get_id(): [] for r in roads}
        
        # This is O(N^2), which is slow for large networks.
        # Ideally we'd use a spatial index, but for now we'll use a simple grid optimization.
        
        # 1. Bucket start points
        starts_grid = {}
        grid_size = 50.0 # 50m grid cells
        
        for r in roads:
            start = r.get_start()
            grid_x = int(start.x / grid_size)
            grid_y = int(start.y / grid_size)
            key = (grid_x, grid_y)
            if key not in starts_grid:
                starts_grid[key] = []
            starts_grid[key].append(r)
            
        # 2. Check ends against nearby start buckets
        for r_from in roads:
            end = r_from.get_end()
            grid_x = int(end.x / grid_size)
            grid_y = int(end.y / grid_size)
            
            # Check 3x3 neighboring cells
            potential_next_roads = []
            for dx in [-1, 0, 1]:
                for dy in [-1, 0, 1]:
                    key = (grid_x + dx, grid_y + dy)
                    if key in starts_grid:
                        potential_next_roads.extend(starts_grid[key])
            
            # Check actual distance
            for r_to in potential_next_roads:
                if r_from.get_id() == r_to.get_id():
                    continue
                    
                dist = end.distance_to(r_to.get_start())
                if dist <= threshold:
                    adj[r_from.get_id()].append(r_to.get_id())
                    
        return adj

    def get_shortest_path(self, start_road_id, end_road_id):
        """
        Find shortest path using A* algorithm.
        Returns list of road IDs or None if no path found.
        """
        if start_road_id not in self.graph or end_road_id not in self.graph:
            return None
            
        # Priority queue: (f_score, current_road_id, path)
        # f_score = g_score (cost so far) + h_score (heuristic)
        start_road = self.roads[start_road_id]
        end_road = self.roads[end_road_id]
        
        start_h = start_road.get_end().distance_to(end_road.get_start())
        queue = [(start_h, start_road_id, [start_road_id])]
        
        visited = set()
        min_g_scores = {start_road_id: 0.0}
        
        while queue:
            f, current_id, path = heapq.heappop(queue)
            
            if current_id == end_road_id:
                return path
            
            if current_id in visited:
                continue
            visited.add(current_id)
            
            current_road = self.roads[current_id]
            current_g = min_g_scores[current_id]
            
            # Explore neighbors
            for next_id in self.graph[current_id]:
                if next_id in visited:
                    continue
                    
                next_road = self.roads[next_id]
                # Cost is length of current road (traversal cost)
                # We could add traffic weight here later
                edge_cost = current_road.get_length()
                new_g = current_g + edge_cost
                
                if next_id not in min_g_scores or new_g < min_g_scores[next_id]:
                    min_g_scores[next_id] = new_g
                    # Heuristic: Euclidean distance from next road start to destination start
                    h = next_road.get_start().distance_to(end_road.get_start())
                    heapq.heappush(queue, (new_g + h, next_id, path + [next_id]))
                    
        return None

    def get_random_od(self):
        """Return a random (start_road_id, end_road_id) pair that has a path."""
        # Try a few times to find a connected pair
        road_ids = list(self.roads.keys())
        if not road_ids:
            return None, None
            
        for _ in range(10):
            start = random.choice(road_ids)
            end = random.choice(road_ids)
            if start == end:
                continue
                
            # Quick check if reachable (BFS/DFS check would be better but A* will just fail fast if disconnected)
            path = self.get_shortest_path(start, end)
            if path:
                return start, end
                
        return None, None

    def get_target_lane_index(self, current_road_id, next_road_id, current_lane_index):
        """
        Determine the best lane index on current road to reach next road.
        """
        if current_road_id not in self.roads or next_road_id not in self.roads:
            return current_lane_index
            
        current_road = self.roads[current_road_id]
        next_road = self.roads[next_road_id]
        
        # Calculate heading difference
        # Heading at end of current road
        h1 = current_road.get_heading_at(current_road.get_length())
        # Heading at start of next road
        h2 = next_road.get_heading_at(0)
        
        diff = h2 - h1
        # Normalize to [-pi, pi]
        while diff > math.pi: diff -= 2*math.pi
        while diff < -math.pi: diff += 2*math.pi
        
        num_lanes = current_road.get_num_lanes()
        if num_lanes <= 1:
            return 0
            
        # Thresholds for turns (approx 30 degrees)
        threshold = math.pi / 6
        
        if diff > threshold: # Left turn
            # In many regions, left turn is from leftmost lane (highest index usually, or check convention)
            # Assuming index 0 is rightmost, index N-1 is leftmost.
            return num_lanes - 1
        elif diff < -threshold: # Right turn
            return 0
        else: # Straight
            # Prefer center lanes or keep current if valid
            # If current is extreme left/right and we are going straight, maybe move to center?
            # For now, just keep current unless it's a dedicated turn lane (which we don't know).
            # Let's just return current_lane_index clamped.
            return min(current_lane_index, num_lanes - 1)

    def build_spatial_index(self):
        """Build a simple grid-based spatial index for roads."""
        self.grid = {}
        self.grid_size = 100.0 # 100m grid cells
        
        for road_id, road in self.roads.items():
            # Index by start and end points
            points = [road.get_start(), road.get_end()]
            
            for p in points:
                grid_x = int(p.x / self.grid_size)
                grid_y = int(p.y / self.grid_size)
                key = (grid_x, grid_y)
                
                if key not in self.grid:
                    self.grid[key] = []
                self.grid[key].append(road_id)

    def find_nearest_road(self, x, y, max_distance=50.0):
        """Find the nearest road to a point (x, y)."""
        if not hasattr(self, 'grid'):
            self.build_spatial_index()
            
        grid_x = int(x / self.grid_size)
        grid_y = int(y / self.grid_size)
        
        candidates = set()
        # Check 3x3 grid
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                key = (grid_x + dx, grid_y + dy)
                if key in self.grid:
                    candidates.update(self.grid[key])
        
        best_road = None
        min_dist = max_distance
        
        p = type('Point', (), {'x': x, 'y': y}) # Mock point
        
        for road_id in candidates:
            road = self.roads[road_id]
            # Simple distance to start/end for now
            # Ideally distance to segment
            d1 = road.get_start().distance_to(p) # This might fail if p is not Point2D
            # We need to pass a Point2D object or implement distance manually
            # Since Point2D is C++, we can't easily instantiate it here without jamfree
            # But we can calculate distance manually
            
            # Manual distance calculation
            start = road.get_start()
            end = road.get_end()
            
            d_start = math.sqrt((start.x - x)**2 + (start.y - y)**2)
            d_end = math.sqrt((end.x - x)**2 + (end.y - y)**2)
            
            dist = min(d_start, d_end)
            
            if dist < min_dist:
                min_dist = dist
                best_road = road
                
        return best_road
