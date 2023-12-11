#%%
import numpy as np
from scipy import linalg
import matplotlib.pyplot as plt
from tqdm import tqdm

acc_his = []

# %%
data = np.loadtxt('data.txt')
datasize = data.shape[0]
train_proportion = 0.8


# online training
class ESN_LMS:
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

    def __init__(self, reservoir_dim, input_dim, output_dim, lr=0.5, sr = 0.9, input_scaling=0.1, ridge = 1e-7, activation=np.tanh):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation
        self.ridge = ridge

        self.current_state = np.empty(shape = (self.reservoir_dim, 1))

        # Initialize weights
        self.Win = np.random.uniform(low = -input_scaling, high = input_scaling, size = [reservoir_dim, self.input_dim])
        self.Wrec = np.random.normal(loc = 0, scale = 1, size = (self.reservoir_dim, self.reservoir_dim))
        self.Wrec = self.Wrec * sr /  max(abs(linalg.eigvals(self.Wrec))) # scale spectral radius

        self.Wout = np.zeros([output_dim, reservoir_dim])

    def train(self, Y_train, epochs = 100, eta = 1e-1):
      for i in range(epochs):
        pred = np.matmul(self.Wout, self.current_state)
        error = (Y_train - pred)
        self.Wout = self.Wout + eta * np.matmul(error, self.current_state.T)
        # print("epoch: {}, loss {:2f}, pred: {:2f}, Y_train: {}".format(i, error[0, 0], pred[0, 0], Y_train[0, 0]))


    def predict(self, input):
      self.current_state = self.get_next_state(input)
      pred = np.matmul(self.Wout, self.current_state)

      return pred

    def get_next_state(self, input):
        next_state = (1 - self.lr) * self.current_state + self.lr * self.activation(np.matmul(self.Win, input) + np.matmul(self.Wrec, self.current_state))
        return next_state


#%%


X_train = data[:, 2].reshape([-1, 1])
Y_train = X_train[1:]
X_train = X_train[:-1]
preds = []


model = ESN_LMS(input_dim = 1, reservoir_dim = 100, output_dim=1)

for x_train, y_train in tqdm(zip(X_train, Y_train)):
  pred = model.predict(x_train[np.newaxis, ...])
  preds.append(pred)
  model.train(y_train[np.newaxis, ...])
  # print(pred, y_train)

preds = np.array(preds).reshape([-1])

# %%
pred_class = None
thre_res = None
accuracy = None
max_accuracy = 0
thresholds = np.linspace(0, np.max(preds)*1.1, 100)

accuracy_list = []

for thre in thresholds:

  pred_class = np.where(preds > thre, 1., 0.)

  accuracy = np.mean(pred_class == Y_train)
  # print("Thre: {}, Accuracy: {:.3f}".format(thre, accuracy))
  if (0.8 < accuracy):
    thre_res = thre
    break
  if (max_accuracy < accuracy):
    max_accuracy = accuracy
    thre_res = thre

  accuracy_list.append(accuracy)

print("thre_res = ", thre_res)
print("max_accuracy = ", max_accuracy)
#%%

plt.plot(accuracy_list)
# %%


thre = 0.5
pred_class = np.where(preds > thre, 1., 0.)

accuracy = np.mean(pred_class == Y_train[:, 0])
print("Accuracy: {:.3f}".format(accuracy))


fig, axes = plt.subplots(1, 2, figsize=(12, 4))

axes[0].set_xlim([650, 700])
axes[0].grid()
axes[0].plot(preds, label="prediction", color="blue", alpha = 0.5)
axes[0].plot(Y_train, label="True", color="red", alpha = 0.5)
axes[0].set_xlabel("Step")
axes[0].legend()

axes[1].grid()
axes[1].plot(preds[:100], label="prediction", color="blue", alpha = 0.5)
axes[1].plot(Y_train[:100], label="True", color="red", alpha = 0.5)
axes[1].set_xlabel("Step")
axes[1].legend()

acc_his.append(accuracy)


# %%

np.min(preds)
# %%