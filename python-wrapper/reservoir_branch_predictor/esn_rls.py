import numpy as np
from scipy import linalg
from .esn_online import ESNOnline

# np.random.seed(0)


class ESN_RLS(ESNOnline):
    # "リザバーコンピューティング", 田中剛平著, P. 64との対応
    # d    <=> Y_train
    # λ    <=> f_factor
    # Nx   <=> self.reservoir_dim
    # x(n) <=> self.current_state
    # current_state.shape = (Nx, 1)
    # Win.shape = (Nx, Nu)
    # Wout.shape = (1, Nx)
    # P.shape = (Nx, Nx)
    # g.shape = (Nx, 1)
    # v.shape = (1, 1)
    def __init__(
        self,
        reservoir_dim,
        input_dim,
        output_dim,
        lr=0.8,
        sr=0.8,
        input_scaling=0.1,
        activation=np.tanh,
        epochs=1,
        threshold=0.5,
    ):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation
        self.current_state = np.empty(shape=(self.reservoir_dim, 1))
        self.epochs = epochs
        self.threshold = threshold

        # Initialize weights
        self.Win = np.random.uniform(
            low=-input_scaling, high=input_scaling, size=[reservoir_dim, self.input_dim]
        )
        self.Wrec = np.random.normal(
            loc=0, scale=1, size=(self.reservoir_dim, self.reservoir_dim)
        )
        self.Wrec = (
            self.Wrec * sr / max(abs(linalg.eigvals(self.Wrec)))
        )  # scale spectral radius

        self.Wout = np.zeros([output_dim, reservoir_dim])

    def train(self, Y_train, f_factor=0.5, delta=0.01):
        P = 1 / delta * np.identity(self.reservoir_dim)
        for i in range(self.epochs):
            v = Y_train - np.matmul(self.Wout, self.current_state)
            # np.matmul(self.current_state, P) ... (1, Nx)
            g = (1 / f_factor * np.matmul(P, self.current_state)) / (
                1
                + 1
                / f_factor
                * np.matmul(self.current_state.T, np.matmul(P, self.current_state))
            )
            # np.matmul(self.current_state, P) ... (1, Nx)
            P = 1 / f_factor * (P - np.matmul(g, np.matmul(self.current_state.T, P)))

            self.Wout = self.Wout + np.matmul(v, g.T)

    def predict(self, inputs: np.ndarray) -> bool:
        self.current_state = self.get_next_state(inputs)
        pred: np.ndarray = np.matmul(self.Wout, self.current_state)
        assert pred.shape == (1, 1), f"pred.shape: {pred.shape} should be (1, 1)"
        pred_class: bool = self.threshold < pred[0, 0]
        return pred_class

    def get_next_state(self, input: bool):
        next_state = (1 - self.lr) * self.current_state + self.lr * self.activation(
            np.matmul(self.Win, input) + np.matmul(self.Wrec, self.current_state)
        )

        return next_state
