"""
Solution stub for the Zombie Problem.

Fill in the implementation of the `Zombie_problem` class to match the
representation and heuristic that you specified in questions (2a) and (2c).

We will test your solution by calling ``python3 zombie.py`` from the shell
prompt.  DO NOT EDIT the main() function.  You may add additional tests to the
`unit_tests` function if you desire.
"""
from heapq import heappush, heappop

class Zombie_problem(object):
    def __init__(self):
        pass
    
    def start_node(self):
        """returns start node"""
        return ([1,2,5,10],[], 'L')
    
    def is_goal(self, node):
        """is True if `node` is a goal"""
        return node == ([], [1,2,5,10], 'R')

    def neighbors(self, node):
        """returns a list of the arcs for the neighbors of `node`"""
        neighbours_array = []

        # Set new direction of flashlight
        if node[2] == 'L': 
            direction = 'R'
            left_index = 0
        else: # node[2] == 'R'
            direction = 'L'
            left_index = 1

        i = 0
        for person1 in node[left_index]:
            # Neighbours where one person crosses
            left_array = node[left_index].copy()
            right_array = node[(left_index + 1) % 2].copy() # Get the other side of the node

            # Move a person from the left to the right
            left_array.pop(i)
            right_array.append(person1)
            right_array.sort()
            
            # Flip the left/right array depend on which state we're in
            if direction == 'R': tmp_node1 = (left_array,right_array,direction)
            else: tmp_node1 = (right_array,left_array, direction)
            neighbours_array.append(tmp_node1)

            j = 0
            for person2 in left_array:
                # Neighbours where two people cross
                second_left_array = left_array.copy()
                second_right_array = right_array.copy()

                # Move a secon person to the right
                second_left_array.pop(j)
                second_right_array.append(person2)
                second_right_array.sort()

                if direction == 'R': tmp_node2 = (second_left_array,second_right_array,direction)
                else: tmp_node2 = (second_right_array,second_left_array, direction)
                
                # Prevent duplications when appending to neighbourhood
                if tmp_node2 not in neighbours_array: 
                    neighbours_array.append(tmp_node2)
                j += 1
            i += 1
        return neighbours_array


    def arc_cost(self, node1, node2):
        """Returns the cost of `arc`"""
        # Find the biggest moved element between two nodes
        diff_arr = []
        for i in range(0,2):
            for elem in node1[i]:
                if elem not in node2[i]:
                    diff_arr.append(elem)
        return max(diff_arr)


    def cost(self, path):
        """Returns the cost of `path`"""
        sum_cost = 0
        for i in range(len(path) - 1):
            sum_cost += self.arc_cost(path[i], path[i+1])
        return sum_cost

    def heuristic(self, node):
        """Returns the heuristic value of `node`"""
        if len(node[0]) == 0: return 0
        return max(node[0])

    def search(self):
        """Return a solution path"""
        frontier = Frontier()
        frontier.add([self.start_node()], 0)

        while(not frontier.is_empty()):
            # Get cheapest path from priority queue
            path = frontier.remove()

            # Check if path is solution
            if self.is_goal(path[-1]):
                return path

            # Else append neighbours to new path
            for new_node in self.neighbors(path[-1]):
                new_path = path.copy()

                # Ignore cycles
                if new_node not in new_path:
                    new_path.append(new_node)
                    frontier.add(new_path, self.cost(new_path) + self.heuristic(new_node))

        return # No solution

class Frontier(object):
    """
    Convenience wrapper for a priority queue usable as a frontier
    implementation.
    """
    def __init__(self):
        self.heap = []

    def add(self, path, priority):
        """Add `path` to the frontier with `priority`"""
        # Push a ``(priority, item)`` tuple onto the heap so that `heappush`
        # and `heappop` will order them properly
        heappush(self.heap, (priority, path))

    def remove(self):
        """Remove and return the smallest-priority path from the frontier"""
        priority,path = heappop(self.heap)
        return path

    def is_empty(self):
        return len(self.heap) == 0


def unit_tests():
    """
    Some trivial tests to check that the implementation even runs.
    Feel free to add additional tests.
    """
    print("testing...")
    p = Zombie_problem()
    assert p.start_node() is not None
    assert not p.is_goal(p.start_node())
    assert p.heuristic(p.start_node()) >= 0

    ns = p.neighbors(p.start_node())
    assert len(ns) > 0

    soln = p.search()
    assert p.cost(soln) > 0
    print("tests ok")

def main():
    unit_tests()
    p = Zombie_problem()
    soln = p.search()
    if soln:
        print("Solution found (cost=%s)\n%s" % (p.cost(soln), soln))
    else:
        raise RuntimeError("Empty solution")

if __name__ == '__main__':
    main()
