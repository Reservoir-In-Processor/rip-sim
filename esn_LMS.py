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
class ESNOnline:

    def __init__(self, reservoir_dim, input_dim, output_dim, lr=0.5, sr = 0.9, input_scaling=0.1, ridge = 1e-7, activation=np.tanh):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation
        self.ridge = ridge

        self.current_state = np.empty(shape = (1, self.reservoir_dim))

        # Initialize weights
        self.Win = np.random.uniform(low = -input_scaling, high = input_scaling, size = [self.input_dim, reservoir_dim])
        self.Wrec = np.random.normal(loc = 0, scale = 1, size = (self.reservoir_dim, self.reservoir_dim))
        self.Wrec = self.Wrec * sr /  max(abs(linalg.eigvals(self.Wrec))) # scale spectral radius

        self.Wout = np.zeros([reservoir_dim, output_dim])


    def train(self, Y_train, epochs = 10, eta = 1e-2):
      for i in range(epochs):
        pred = self.current_state @ self.Wout
        error = (Y_train - pred)
        self.Wout = self.Wout + eta * error * self.current_state.T
        # print("epoch: {}, loss {:2f}".format(i, error[0, 0]))

    def predict(self, input):
      self.current_state = self.get_next_state(input)
      pred = np.matmul(self.current_state, self.Wout)

      return pred

    def get_next_state(self, input):
        next_state = (1 - self.lr) * self.current_state + self.lr * self.activation(np.matmul(input, self.Win) + np.matmul(self.current_state, self.Wrec))
        return next_state


#%%


X_train = data[:, 2].reshape([-1, 1])
Y_train = X_train[1:]
X_train = X_train[:-1]
preds = []


model = ESNOnline(input_dim = 1, reservoir_dim = 100, output_dim=1)

for x_train, y_train in tqdm(zip(X_train, Y_train)):
  pred = model.predict(x_train[np.newaxis, ...])
  preds.append(pred)
  model.train(y_train[np.newaxis, ...])

preds = np.array(preds).reshape([-1])


  # thre = 0.
  # pred_class = np.where(preds > thre, 1., 0.)

  # accuracy = np.mean(pred_class == Y_test)
  # print("Split: {}, Accuracy: {:.3f}".format(split, accuracy))


  # fig, axes = plt.subplots(1, 2, figsize=(12, 4))

  # axes[0].grid()
  # axes[0].plot(pred_class, label="Predicted", color="blue", alpha = 0.5)
  # axes[0].plot(Y_test, label="True", color="red", alpha = 0.5)
  # axes[0].set_xlabel("Step")

  # axes[1].grid()
  # axes[1].plot(preds[:100], label="Predicted", color="blue", alpha = 0.5)
  # axes[1].plot(Y_test[:100], label="True", color="red", alpha = 0.5)
  # axes[1].set_xlabel("Step")

  # acc_his.append(accuracy)


# # %%

# %%
pred_class = None
thre_res = None
accuracy = None
max_accuracy = 0
thresholds = np.linspace(0, np.max(preds)*1.1, 100)

acuracy_list = []

for thre in thresholds:
  # thre = 100

  pred_class = np.where(preds > thre, 1., 0.)

  accuracy = np.mean(pred_class == Y_train)
  print("Thre: {}, Accuracy: {:.3f}".format(thre, accuracy))
  if (0.8 < accuracy):
    thre_res = thre
    break
  if (max_accuracy < accuracy):
    max_accuracy = accuracy
    thre_res = thre

  acuracy_list.append(accuracy)

print("thre_res = ", thre_res)
print("max_accuracy = ", max_accuracy)
#%%
