import numpy as np

from action_value_table import ActionValueTable

# HINT: You will need to sample from probability distributions to complete this
# question.  Please use `numpy.random.rand` to generate a floating point number
# uniformly at random and `numpy.random.choice` to uniformly randomly select an
# element from a list.
#
# Documentation:
# - https://numpy.org/doc/stable/reference/random/generated/numpy.random.rand.html
# - https://numpy.org/doc/stable/reference/random/generated/numpy.random.choice.html


UP, DOWN, LEFT, RIGHT = 0, 1, 2, 3  # agents actions
GAMMA = 0.95
STEP_SIZE = 0.25
EPSILON = 0.1


class QLearningAgent():
    """
    Implement your code for a Q-learning agent here. We have provided code
    implementing the action-value table in `action_value_table.py`. Here, you
    will implement the `get_action`, `get_greedy_action` and `update` methods.
    """
    def __init__(self, dimension):
        self.actions = [UP, DOWN, LEFT, RIGHT]
        num_actions = len(self.actions)
        self.values = ActionValueTable(dimension, num_actions)
        self.gamma = GAMMA
        self.step_size = STEP_SIZE
        self.epsilon = EPSILON


    def update(self, state, action, reward, next_state, done):
        """
        Update the values stored in `self.values` using Q-learning.

        HINT: Use `self.values.get_value` and `self.values.set_value`
        HINT: Remember to add a special case to handle the terminal state

        Args:
            state : (list) a list of type [bool, int, int] where the first
            entry is whether the agent posseses the key, and the next two
            entries are the row and column position of the agent in the maze
            
            action : (int) the action taken at state

            reward : float

        Returns:
            None
        """
        current_Q = self.values.get_value(state, action)
        next_action = self.get_greedy_action(next_state) # Get the action which maximizes Q of next state
        next_Q = self.values.get_value(next_state, next_action)

        # Continue on with the update
        update_value = STEP_SIZE * (reward + GAMMA*next_Q - current_Q)
        new_q_value = current_Q + update_value
        self.values.set_value(state, action, new_q_value)


    def get_action(self, state):
        """
        This function returns an action from self.actions given a state.

        Implement this function using an epsilon-greedy policy.

        HINT: use np.random.rand() to generate random numbers
        HINT: If more than one action has maximum value, treat them all as the
        greedy action. In other words, if there are b greedy actions, each
        should have probability
        (1 - epsilon)/b + epsilon/|A|,
        where |A| is the number of actions in this state.

        Args:
            state : (list)
            a list of type [bool, int, int] where the first entry is whether
            the agent posseses the key, and the next two entries are the row
            and column position of the agent in the maze

        Returns:
            action : (int) a epsilon-greedy action for `state`
        """
        rand_epi = np.random.rand() # Returns a random number [0,1)

        if rand_epi < EPSILON: # Perform a random action
            rand_action = np.random.randint(0, 4) # Generate a random number 0-3
            return self.actions[rand_action] # return random action
        else:
            # Iterate through all actions, check which gives best value
            # If multiple greedy solutions exist pick one at random
            return self.get_greedy_action(state)


    def get_greedy_action(self, state):
        """
        This function returns an action from self.actions given a state. 

        Implement this function using a greedy policy, i.e. return the action
        with the highest value
        
        HINT: If more than more than one action has maximum value, uniformly
        randomize amongst them

        Args:
            state : (list)
            a list of type [bool, int, int] where the first entry is whether
            the agent posseses the key, and the next two entries are the row
            and column position of the agent in the maze

        Returns:
            action : (int) a greedy action for `state`
        """

        greedy_values = [] # list of action values actions
        greedy_actions = [] # List of actions to narrow down into one

        for action in self.actions: # Get the value from performing each action
            value_from_action = self.values.get_value(state, action) # Get Q_Value for each action on a state
            greedy_values.append(value_from_action)

        max_value = max(greedy_values) # Find the largest value, this is the greedy option
        for i in range(0, len(self.actions)): # Cull out the non greedy actions
            if greedy_values[i] == max_value:
                greedy_actions.append(self.actions[i])

        # Pick a random action from greedy actions
        num_greedy_action = len(greedy_actions)
        random_greedy_action = np.random.randint(0, num_greedy_action) # 0 to num greedy - 1
        return greedy_actions[random_greedy_action]
