#%%
import numpy as np
from scipy import linalg
import matplotlib.pyplot as plt
# from reservoirpy.nodes import Reservoir, Ridge, Input
from sklearn.model_selection import TimeSeriesSplit

acc_his = []

# %%
data = np.loadtxt('data.txt')
datasize = data.shape[0]
train_proportion = 0.8


class ReservoirNetwork:

    def __init__(self, reservoir_dim, input_dim, output_dim, lr=0.5, sr = 0.9, input_scaling=0.1, ridge = 1e-7, activation=np.tanh):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation
        self.ridge = ridge

        self.log_reservoir_nodes = np.empty(shape = (self.reservoir_dim))

        # Initialize weights
        self.Win = np.random.uniform(low = -input_scaling, high = input_scaling, size = [self.input_dim, reservoir_dim])
        self.Wrec = np.random.normal(loc = 0, scale = 1, size = (self.reservoir_dim, self.reservoir_dim))
        self.Wrec = self.Wrec * sr /  max(abs(linalg.eigvals(self.Wrec))) # scale spectral radius

        self.Wout = np.zeros([reservoir_dim, output_dim])

    def train(self, X_train, Y_train, lambda0=0.1):
      current_state = np.zeros(self.reservoir_dim)

      for input in X_train:
        current_state = self.get_next_state(input, current_state)
        self.log_reservoir_nodes = np.concatenate([self.log_reservoir_nodes, current_state])

      self.log_reservoir_nodes = self.log_reservoir_nodes.reshape([-1, self.reservoir_dim])[1:]

      I_ridge = np.identity(self.reservoir_dim) * self.ridge
      inv_x = np.linalg.inv(self.log_reservoir_nodes.T @ self.log_reservoir_nodes + I_ridge)
      self.Wout = (inv_x @ self.log_reservoir_nodes.T) @ Y_train

    def predict(self, inputs):
      preds = []
      current_state = self.log_reservoir_nodes[-1]
      
      for input in inputs:
          current_state = self.get_next_state(input, current_state)
          preds.append(current_state @ self.Wout)
      return np.array(preds)

    def get_next_state(self, input, current_state):
        next_state = (1 - self.lr) * current_state
        next_state += self.lr * (np.matmul(input, self.Win) + current_state @ self.Wrec)
        return self.activation(next_state)


#%%
tscv = TimeSeriesSplit(n_splits=5)

Y = data[:, 2].reshape([-1, 1])


for split, (train_index, test_index) in enumerate(tscv.split(data)):
  print(len(train_index))


  X_train = Y[train_index[:-1]]
  Y_train = Y[train_index[1:]]

  X_test = Y[test_index[:-1]]
  Y_test = Y[test_index[1:]]



  model = ReservoirNetwork(input_dim = 1, reservoir_dim = 100, output_dim=1)
  model.train(X_train, Y_train)

  Y_pred = model.predict(X_test)

  thre = 0.5
  pred_class = np.where(Y_pred > thre, 1., 0.)

  accuracy = np.mean(pred_class == Y_test)
  print("Split: {}, Accuracy: {:.3f}".format(split, accuracy))


  fig, axes = plt.subplots(1, 2, figsize=(12, 4))

  axes[0].grid()
  axes[0].plot(Y_pred, label="Predicted", color="blue", alpha = 0.5)
  axes[0].plot(Y_test, label="True", color="red", alpha = 0.5)
  axes[0].set_xlabel("Step")

  axes[1].grid()
  axes[1].plot(Y_pred[:100], label="Predicted", color="blue", alpha = 0.5)
  axes[1].plot(Y_test[:100], label="True", color="red", alpha = 0.5)
  axes[1].set_xlabel("Step")

  acc_his.append(accuracy)

  break


# %%
