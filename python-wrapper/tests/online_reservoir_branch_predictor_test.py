from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
from scipy import linalg


class ESNOnline:
    def __init__(
        self,
        reservoir_dim,
        input_dim,
        output_dim,
        lr=0.5,
        sr=0.9,
        input_scaling=0.1,
        ridge=1e-7,
        activation=np.tanh,
    ):
        self.reservoir_dim = reservoir_dim
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.lr = lr
        self.input_scaling = input_scaling
        self.activation = activation
        self.ridge = ridge

        self.log_reservoir_nodes = np.empty(shape=(self.reservoir_dim))

        # Initialize weights
        # FIXME: ここで重みをランダムにしない
        self.Win = np.random.uniform(
            low=-input_scaling, high=input_scaling, size=[self.input_dim, reservoir_dim]
        )
        self.Wrec = np.random.normal(
            loc=0, scale=1, size=(self.reservoir_dim, self.reservoir_dim)
        )
        self.Wrec = (
            self.Wrec * sr / max(abs(linalg.eigvals(self.Wrec)))
        )  # scale spectral radius

        self.Wout = np.zeros([reservoir_dim, output_dim])

    def predict(self, inputs: np.ndarray) -> bool:
        # FIXME:" predict

        return True

    def train(self, result: bool) -> None:
        # FIXME: update w_out
        return

    def get_next_state(self, input, current_state) -> np.ndarray:
        # FIXME:
        return


# states =


def test_online_reservoir_bp():
    rsim = RIPSimulator(
        Path("rip-tests/dhry-baremetal.bin"),
        BranchPredKind.Interactive,
        output_sim_err=False,
    )
    branch_num = 0
    hit = 0
    prev_pred = None
    stage_num = 0
    rbp = ESNOnline(reservoir_dim=100, input_dim=1, output_dim=1)
    while True:
        res = rsim.proceed()
        if res is None:  # end of simulation
            break
        if res["Kind"] == "PipelineStates":
            stage_num += 1
            # FIXME: update reservoir state by pipeline states?
            # print(res)
            pass
        elif res["Kind"] == "Predict":
            # predict here, currently input is only previous prediction result.
            inputs = np.array([res["PrevPred"]])
            predict = rbp.predict(inputs)
            rsim.predict(predict)
            prev_pred = predict
        elif res["Kind"] == "Learn":
            branch_num += 1
            # train here
            rbp.train(res["Cond"])
            print("prev_pred: ", prev_pred)
            print("res[Cond]: ", res["Cond"])
            if prev_pred is not None and (
                (prev_pred and res["Cond"]) or (not prev_pred and not res["Cond"])
            ):
                hit += 1
        else:
            assert False, "unreachable!"
    print("hit: ", hit)
    print("branch_num: ", branch_num)
    print("stage_num: ", stage_num)
    print("accuracy: ", hit / branch_num)
    # assert pss[-1]["EX"]["PC"] == 0x8084, "end PC unmatched!"
