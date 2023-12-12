# %%
import numpy as np
from scipy import linalg
import matplotlib.pyplot as plt
from tqdm import tqdm

acc_his = []

# %%
data = np.loadtxt("data.txt")
datasize = data.shape[0]


# online training
class ESN_RLS:
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
    ):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation

        self.current_state = np.empty(shape=(self.reservoir_dim, 1))

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

    def train(self, Y_train, epochs=1, f_factor=0.5, delta=0.01):
        P = 1 / delta * np.identity(self.reservoir_dim)
        for i in range(epochs):
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

    def predict(self, input):
        self.current_state = self.get_next_state(input)
        pred = np.matmul(self.Wout, self.current_state)

        return pred

    def get_next_state(self, input):
        next_state = (1 - self.lr) * self.current_state + self.lr * self.activation(
            np.matmul(self.Win, input) + np.matmul(self.Wrec, self.current_state)
        )
        return next_state


# %%


X_train = data[:, 2].reshape([-1, 1])
Y_train = X_train[1:]
X_train = X_train[:-1]
preds = []


model = ESN_RLS(input_dim=1, reservoir_dim=100, output_dim=1)

for x_train, y_train in tqdm(zip(X_train, Y_train)):
    pred = model.predict(x_train[np.newaxis, ...])
    preds.append(pred)
    model.train(y_train[np.newaxis, ...])

preds = np.array(preds).reshape([-1])

plt.xlim([650, 700])
plt.plot(preds)
plt.plot(Y_train)


# %%

plt.xlim([650, 700])
plt.plot(preds)
plt.plot(Y_train)


# %%
pred_class = None
thre_res = None
accuracy = None
max_accuracy = 0
thresholds = np.linspace(np.min(preds), np.max(preds), 100)

acuracy_list = []

for thre in thresholds:
    pred_class = np.where(preds > thre, 1.0, 0.0)

    accuracy = np.mean(pred_class == Y_train[:, 0])
    # print("Thre: {}, Accuracy: {:.3f}".format(thre, accuracy))
    if 0.8 < accuracy:
        thre_res = thre
        break
    if max_accuracy < accuracy:
        max_accuracy = accuracy
        thre_res = thre

    acuracy_list.append(accuracy)

print("thre_res = ", thre_res)
print("max_accuracy = ", max_accuracy)
# %%

thre = 0.5
pred_class = np.where(preds > thre, 1.0, 0.0)

accuracy = np.mean(pred_class == Y_train[:, 0])
print("Accuracy: {:.3f}".format(accuracy))


fig, axes = plt.subplots(1, 2, figsize=(12, 4))

axes[0].set_xlim([650, 700])
axes[0].grid()
axes[0].plot(preds, label="Predicted", color="blue", alpha=0.5)
axes[0].plot(Y_train, label="True", color="red", alpha=0.5)
axes[0].set_xlabel("Step")
axes[0].legend()

axes[1].grid()
axes[1].plot(preds[:100], label="Predicted", color="blue", alpha=0.5)
axes[1].plot(Y_train[:100], label="True", color="red", alpha=0.5)
axes[1].set_xlabel("Step")
axes[1].legend()

acc_his.append(accuracy)


# %%
