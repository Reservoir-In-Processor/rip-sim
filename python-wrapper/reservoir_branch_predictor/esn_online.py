import numpy as np
from scipy import linalg

# np.random.seed(0)


class ESNOnline:
    def __init__(
        self,
        reservoir_dim,
        input_dim,
        output_dim,
        lr=0.5,
        sr=0.9,
        input_scaling=0.1,
        activation=np.tanh,
        threshold=0.5,
        epochs=1,
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

        # # Initialize weights
        # self.Win = np.random.uniform(
        #     low=-input_scaling, high=input_scaling, size=[self.reservoir_dim, input_dim]
        # )
        # self.Wrec = np.random.normal(
        #     loc=0, scale=1, size=(self.reservoir_dim, self.reservoir_dim)
        # )
        # self.Wrec = (
        #     self.Wrec * sr / max(abs(linalg.eigvals(self.Wrec)))
        # )  # scale spectral radius

        # self.Wout = np.zeros([output_dim, reservoir_dim])

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

    def predict(self, inputs: np.ndarray) -> bool:
        raise NotImplementedError

    def train(self, result: bool) -> None:
        raise NotImplementedError

    def get_next_state(self, input, current_state) -> np.ndarray:
        raise NotImplementedError


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
    ):
        super().__init__(
            reservoir_dim,
            input_dim,
            output_dim,
            lr,
            sr,
            input_scaling,
            activation,
            epochs,
        )

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


class ESN_LMS(ESNOnline):
    # "リザバーコンピューティング", 田中剛平著, P. 63との対応
    # d    <=> Y_train
    # y    <=> pred
    # λ    <=> f_factor
    # Nu <=> input_dim
    # Nx   <=> self.reservoir_dim
    # x(n) <=> self.current_state

    # current_state.shape = (Nx, 1)
    # Win.shape = (Nx, Nu)
    # Wout.shape = (1, Nx)

    def __init__(
        self,
        reservoir_dim,
        input_dim,
        output_dim,
        lr=0.8,
        sr=0.8,
        input_scaling=0.1,
        ridge=1e-7,
        eta=1e-1,
        activation=np.tanh,
        epochs=1,
    ):
        super().__init__(
            reservoir_dim,
            input_dim,
            output_dim,
            lr,
            sr,
            input_scaling,
            activation,
            epochs,
        )
        self.ridge = ridge
        self.eta = eta

    def train(self, Y_train):
        for _ in range(self.epochs):
            pred = np.matmul(self.Wout, self.current_state)
            error = Y_train - pred
            self.Wout = self.Wout + self.eta * np.matmul(error, self.current_state.T)

    def predict(self, inputs: np.ndarray) -> bool:
        self.current_state = self.get_next_state(inputs)
        pred: np.ndarray = np.matmul(self.Wout, self.current_state)
        assert pred.shape == (1, 1), f"pred.shape: {pred.shape} should be (1, 1)"
        pred_class: bool = self.threshold < pred[0, 0]
        return pred_class

    def get_next_state(self, input):
        next_state = (1 - self.lr) * self.current_state + self.lr * self.activation(
            np.matmul(self.Win, input) + np.matmul(self.Wrec, self.current_state)
        )
        return next_state
