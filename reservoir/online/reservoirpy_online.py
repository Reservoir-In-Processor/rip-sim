# %%
import numpy as np
from reservoirpy.nodes import Reservoir, FORCE, LMS, RLS
import matplotlib.pyplot as plt
import matplotlib as mpl


data = np.loadtxt("data.txt")
datasize = data.shape[0]
X = data[:, 2][:-1][..., np.newaxis]
Y = data[:, 2][1:][..., np.newaxis]
print("Datasize: {}, Num. of 1: {} ({:.2f} %)".format(datasize, int(Y.sum()), Y.mean()))


# %% Define  Hyperparams.
lr = 0.8
sr = 0.8
reservoir_nodes = 100
online_alg = RLS

# %% Define model
reservoir, readout = Reservoir(reservoir_nodes, lr=lr, sr=sr), online_alg()
esn = reservoir >> readout

# %% Train and prediction
preds = esn.train(X, Y)

# %%
pred_class = None
best_thre = None
accuracy = None
best_accuracy = 0

thresholds = np.linspace(
    np.mean(preds) - 3 * np.std(preds), np.mean(preds) + 3 * np.std(preds), 100
)

accuracy_list = []

for thre in thresholds:
    pred_class = np.where(preds > thre, 1.0, 0.0)
    accuracy = np.mean(pred_class == Y)

    if best_accuracy < accuracy:
        best_accuracy = accuracy
        best_thre = thre

    accuracy_list.append(accuracy)

print("best_thre = ", best_thre)
print("best_accuracy = ", best_accuracy)
plt.xlabel("Threshould")
plt.ylabel("Accuracy")
plt.plot(thresholds, accuracy_list)
plt.show()

# %%

# Visualize the prediction
fig, axes = plt.subplots(1, 2, figsize=(12, 4))

pred_class = np.where(preds > best_thre, 1.0, 0.0)
accuracy = np.mean(pred_class == Y)

axes[0].grid()
axes[0].plot(preds, label="prediction", color="blue", alpha=0.5)
axes[0].plot(Y, label="Ground-Truth", color="red", alpha=0.5)
axes[0].set_xlabel("Branch instructions")
axes[0].legend(loc="lower left")

axes[1].grid()
axes[1].set_xlim([650, 700])
axes[1].plot(preds, label="prediction", color="blue", alpha=0.5)
axes[1].plot(Y, label="Ground-Truth", color="red", alpha=0.5)
axes[1].set_xlabel("Branch instructions")

axes[1].legend(loc="lower left")

# %%
