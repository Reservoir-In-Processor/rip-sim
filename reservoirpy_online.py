#%%
import numpy as np
from reservoirpy.nodes import Reservoir, FORCE, LMS, RLS
import matplotlib.pyplot as plt


data = np.loadtxt('data.txt')
datasize = data.shape[0]


X = data[:, 2][:-1][..., np.newaxis]
Y = data[:, 2][1:][..., np.newaxis]

# %%

lr = 0.9
sr = 0.9

reservoir, readout = Reservoir(100, lr=lr, sr=sr), LMS()

esn = reservoir >> readout
preds = esn.train(X, Y)
thre = 0.5
pred_class = np.where(preds > thre, 1., 0.)


plt.xlim([650, 800])
plt.plot(pred_class)
plt.plot(Y)

print(lr, sr, (pred_class == Y).mean())

# %%

fig, axes = plt.subplots(1, 2, figsize = (12, 4))

axes[0].grid()
axes[0].plot(preds, label = 'prediction', color = 'blue', alpha=0.5)
axes[0].plot(Y, label = 'Ground-Truth', color = 'red', alpha=0.5)

axes[1].grid()
axes[1].set_xlim([650, 700])
axes[1].plot(preds, label = 'prediction', color = 'blue', alpha=0.5)
axes[1].plot(Y, label = 'Ground-Truth', color = 'red', alpha=0.5)

plt.legend()
print(lr, sr, (pred_class == Y).mean())

# %%
pred_class = None
thre_res = None
accuracy = None
max_accuracy = 0
thresholds = np.linspace(0, np.max(preds)*1.1, 100)

accuracy_list = []

for thre in thresholds:

  pred_class = np.where(preds > thre, 1., 0.)

  accuracy = np.mean(pred_class == Y)
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

# %%
fig, axes = plt.subplots(1, 2, figsize = (12, 4))

axes[0].grid()
axes[0].plot(preds, label = 'prediction', color = 'blue', alpha=0.5)
axes[0].plot(Y, label = 'Ground-Truth', color = 'red', alpha=0.5)

axes[1].grid()
axes[1].set_xlim([650, 700])
axes[1].plot(preds, label = 'prediction', color = 'blue', alpha=0.5)
axes[1].plot(Y, label = 'Ground-Truth', color = 'red', alpha=0.5)

plt.legend()
print(lr, sr, (pred_class == Y).mean())
