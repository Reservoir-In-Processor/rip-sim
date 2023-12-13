# %%
import numpy as np
import matplotlib.pyplot as plt
from reservoirpy.nodes import Reservoir, Ridge, Input
from sklearn.model_selection import TimeSeriesSplit

acc_his = []

# %%
data = np.loadtxt("data.txt")
datasize = data.shape[0]
train_proportion = 0.8

tscv = TimeSeriesSplit(n_splits=5)

Y = data[:, 2].reshape([-1, 1])


for split, (train_index, test_index) in enumerate(tscv.split(data)):
    X_train = Y[train_index[:-1]]
    Y_train = Y[train_index[1:]]

    X_test = Y[test_index[:-1]]
    Y_test = Y[test_index[1:]]

    data = Input()
    reservoir = Reservoir(1000, lr=0.5, sr=0.9)
    readout = Ridge(ridge=1e-7)

    # esn_model = data >> reservoir >> readout & data >> readout
    esn_model = data >> reservoir >> readout
    esn_model = esn_model.fit(X_train, Y_train, warmup=10)

    Y_pred = esn_model.run(X_test)
    thre = 0.5
    pred_class = np.where(Y_pred > thre, 1.0, 0.0)

    accuracy = np.mean(pred_class == Y_test)
    print("Split: {}, Accuracy: {:.3f}".format(split, accuracy))

    fig, axes = plt.subplots(1, 2, figsize=(12, 4))

    axes[0].grid()
    axes[0].plot(Y_pred, label="Predicted", color="blue", alpha=0.5)
    axes[0].plot(Y_test, label="True", color="red", alpha=0.5)
    axes[0].set_xlabel("Step")

    axes[1].grid()
    axes[1].plot(Y_pred[:100], label="Predicted", color="blue", alpha=0.5)
    axes[1].plot(Y_test[:100], label="True", color="red", alpha=0.5)
    axes[1].set_xlabel("Step")

    acc_his.append(accuracy)

    plt.show()

# %%
print("accuracy: {:.3f} ± {:.3f}".format(np.mean(acc_his), np.std(acc_his)))
# %%
