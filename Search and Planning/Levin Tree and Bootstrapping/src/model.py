import numpy as np

class UniformModel:
    def get_probabilities(self, _):
        return np.ones(4) / 4

class Model:
    def __init__(self):
        self._table = {}
        self._increment = 10

    def get_probabilities(self, pattern):
        if pattern not in self._table:
            self._table[pattern] = np.ones(4)

        return self._table[pattern] / np.sum(self._table[pattern])
    
    def _increment_entry(self, context, action):
        if context not in self._table:
            self._table[context] = np.ones(4)
        self._table[context][action] += self._increment

    def update(self, path, use_reversed=True):
        states = path.get_states()
        actions = path.get_actions()

        for state, action in zip(states, actions):
            regular, reversed_ = state.get_reversed_pattern()

            self._increment_entry(regular, action)

            # Skip the second update when both contexts are identical
            if use_reversed and reversed_ != regular:
                self._increment_entry(reversed_, action)