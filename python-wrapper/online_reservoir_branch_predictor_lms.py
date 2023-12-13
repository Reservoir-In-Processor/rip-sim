# %%
from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
from tqdm import tqdm
from reservoir_branch_predictor.esn_lms import ESN_LMS

np.random.seed(0)

# %%
# launch rip simulator
rsim = RIPSimulator(
    Path("../rip-tests/dhry.bin"),
    BranchPredKind.Interactive,
    output_sim_err=True,
)
# %%


# initialize reservoir state
input_dim = 3
prev_branch = False
branch_dist = 0
from collections import defaultdict

inst_frequency_from_previous_branch = defaultdict(int)
inputs = np.zeros(shape=(input_dim, 1))


def update(
    res: dict,
    prev_branch: bool,
    branch_dist: int,
    inst_frequency_from_previous_branch: dict,
) -> np.ndarray:
    """
    update inputs by res.
    preprocessing should be hardware friendly.
    """
    print(res)
    if res["Kind"] == "PipelineStates" and "IF" in res:
        inst_frequency_from_previous_branch[res["IF"]["mnemo"]] += 1
    inputs = np.zeros(shape=(input_dim, 1))
    inputs[0] = prev_branch
    inputs[1] = branch_dist
    inputs[2] = inst_frequency_from_previous_branch["addi"]

    return inputs


# initialize reservoir branch predictor
rbp = ESN_LMS(reservoir_dim=100, input_dim=input_dim, output_dim=1)

# %%
while True:
    res = rsim.proceed()
    if res is None:  # end of simulation
        break
    if res["Kind"] == "PipelineStates":
        # FIXME: update reservoir state by pipeline states?
        branch_dist += 1
        inputs = update(
            res, prev_branch, branch_dist, inst_frequency_from_previous_branch
        )
        pass
    elif res["Kind"] == "Predict":
        # predict here, currently input is only previous prediction result.
        inputs = update(
            res, prev_branch, branch_dist, inst_frequency_from_previous_branch
        )
        predict = rbp.predict(inputs)
        branch_dist = 0
        inst_frequency_from_previous_branch = defaultdict(int)
        rsim.predict(predict)
    elif res["Kind"] == "Learn":
        # train here
        rbp.train(res["BranchResult"])
        prev_branch = res["BranchResult"]
    else:
        assert False, "unreachable!"

# %%

# %%
