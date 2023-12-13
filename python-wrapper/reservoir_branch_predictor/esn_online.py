import numpy as np


class ESNOnline:
    def __init__(self):
        raise NotImplementedError

    def predict(self, inputs: np.ndarray) -> bool:
        raise NotImplementedError

    def train(self, result: bool) -> None:
        raise NotImplementedError

    def get_next_state(self, input, current_state) -> np.ndarray:
        raise NotImplementedError
