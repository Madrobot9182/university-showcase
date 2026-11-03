import heapq
import math

class State:
    """
    Class to represent a state on grid-based pathfinding problems. The class contains two static variables:
    map_width and map_height containing the width and height of the map. Although these variables are properties
    of the map and not of the state, they are used to compute the hash value of the state, which is used
    in the CLOSED list.

    Each state has the values of x, y, g, h, and cost. The cost is used as the criterion for sorting the nodes
    in the OPEN list for both Dijkstra's algorithm and A*. For Dijkstra the cost should be the g-value, while
    for A* the cost should be the f-value of the node.
    """
    map_width = 0
    map_height = 0

    def __init__(self, x, y):
        """
        Constructor - requires the values of x and y of the state. All the other variables are
        initialized with the value of 0.
        """
        self._x = x
        self._y = y
        self._g = 0
        self._cost = 0
        self._parent = None

    def __repr__(self):
        """
        This method is invoked when we call a print instruction with a state. It will print [x, y],
        where x and y are the coordinates of the state on the map.
        """
        state_str = "[" + str(self._x) + ", " + str(self._y) + "]"
        return state_str

    def __lt__(self, other):
        """
        Less-than operator; used to sort the nodes in the OPEN list
        """
        return self._cost < other._cost

    def state_hash(self):
        """
        Given a state (x, y), this method returns the value of y * map_width + x. This is a perfect
        hash function for the problem (i.e., no two states will have the same hash value). This function
        is used to implement the CLOSED list of the algorithms.
        """
        return self._y * State.map_width + self._x

    def __eq__(self, other):
        """
        Method that is invoked if we use the operator == for states. It returns True if self and other
        represent the same state; it returns False otherwise.
        """
        return self._x == other._x and self._y == other._y

    def get_x(self):
        """
        Returns the x coordinate of the state
        """
        return self._x

    def set_parent(self, parent):
        """
        Sets the parent of a node in the search tree
        """
        self._parent = parent

    def get_parent(self):
        """
        Returns the parent of a node in the search tree
        """
        return self._parent

    def get_y(self):
        """
        Returns the y coordinate of the state
        """
        return self._y

    def get_g(self):
        """
        Returns the g-value of the state
        """
        return self._g

    def set_g(self, g):
        """
        Sets the g-value of the state
        """
        self._g = g

    def get_cost(self):
        """
        Returns the cost of a state; the cost is determined by the search algorithm
        """
        return self._cost

    def set_cost(self, cost):
        """
        Sets the cost of the state; the cost is determined by the search algorithm
        """
        self._cost = cost

class Dijkstra:
    def __init__(self, gridded_map):
        """
        Constructor
        """
        self.gmap = gridded_map
        self.closed_data = None
    
    def search(self, start: State, goal: State):
        _OPEN = [start]
        _CLOSED = {}

        while _OPEN:
            current = heapq.heappop(_OPEN)
            
            # Skip node if already expanded
            if current.state_hash() in _CLOSED:
                continue
            _CLOSED[current.state_hash()] = current

            if current == goal:
                # Goal Node Found
                path = []
                state = current
                cost = current.get_g()
                while state is not None:
                    path.append(state)
                    state = state.get_parent()
                path.reverse()
                self.closed_data = _CLOSED
                return path, cost, len(_CLOSED)

            successors = self.gmap.successors(current)

            for state in successors:
                # Update each successor
                new_g = current.get_g() + self.gmap.cost(current.get_x() - state.get_x(), current.get_y() - state.get_y())
                # First time successor is seen or this is a cheaper route
                if state.get_parent() is None or new_g < state.get_g():
                    # Set/update value of state
                    state.set_g(new_g)
                    state.set_cost(new_g)
                    state.set_parent(current)

                    # Update Open and Closed
                    heapq.heappush(_OPEN, state)

        return None, -1, len(_CLOSED)

    def get_closed_data(self):
        return self.closed_data

class AStar:
    def __init__(self, gridded_map):
        """
        Constructor
        """
        self.gmap = gridded_map
        self.closed_data = None
    
    def octile_heuristic(self, state, goal):
        delta_x = abs(state.get_x() - goal.get_x())
        delta_y = abs(state.get_y() - goal.get_y())
        return 1.5 * min(delta_x, delta_y) + abs(delta_x - delta_y) #* 2

    def search(self, start: State, goal: State):
        _OPEN = [start]
        _CLOSED = {}

        while _OPEN:
            current = heapq.heappop(_OPEN)
            
            # Skip node if already expanded
            if current.state_hash() in _CLOSED:
                continue
            _CLOSED[current.state_hash()] = current

            if current == goal:
                # Goal Node Found
                path = []
                state = current
                cost = current.get_g()
                while state is not None:
                    path.append(state)
                    state = state.get_parent()
                path.reverse()
                self.closed_data = _CLOSED
                return path, cost, len(_CLOSED)

            successors = self.gmap.successors(current)

            for state in successors:
                # Update each successor
                new_g = current.get_g() + self.gmap.cost(current.get_x() - state.get_x(), current.get_y() - state.get_y())
                # First time successor is seen or this is a cheaper route
                if state.get_parent() is None or new_g < state.get_g():
                    # Set/update value of state
                    state.set_g(new_g)
                    state.set_cost(new_g + self.octile_heuristic(state, goal)) # TODO difference
                    state.set_parent(current)

                    # Update Open
                    heapq.heappush(_OPEN, state)

        return None, -1, len(_CLOSED)

    def get_closed_data(self):
        return self.closed_data