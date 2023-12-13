# %%
from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
from tqdm import tqdm
from reservoir_branch_predictor.esn_lms import ESN_LMS


# %%
# launch rip simulator
rsim = RIPSimulator(
    Path("../rip-tests/dhry.bin"),
    BranchPredKind.Interactive,
    output_sim_err=False,
)
# %%


pipeline_states = {"PC", "EXInstVal", ""}
branch_num = 0
hit = 0
prev_pred = None
stage_num = 0
prev_branch = False
# initialize reservoir branch predictor
rbp = ESN_LMS(reservoir_dim=100, input_dim=1, output_dim=1)

# %%
for _ in tqdm(range(124791 * 2)):
    # proceed 1 cycle or trapped by branch predictor
    res = rsim.proceed()
    if res is None:  # end of simulation
        break
    if res["Kind"] == "PipelineStates":
        stage_num += 1
        # FIXME: update reservoir state by pipeline states?
        pass
    elif res["Kind"] == "Predict":
        # predict here, currently input is only previous prediction result.
        inputs = np.array([prev_branch]).reshape([1, 1])
        predict = rbp.predict(inputs)
        rsim.predict(predict)
        prev_pred = predict
    elif res["Kind"] == "Learn":
        branch_num += 1
        # train here
        rbp.train(res["Cond"])
        if prev_pred is not None and (
            (prev_pred and res["Cond"]) or (not prev_pred and not res["Cond"])
        ):
            hit += 1
        prev_branch = res["Cond"]
    else:
        assert False, "unreachable!"

# %%
print("hit: ", hit)
print("branch_num: ", branch_num)
print("stage_num: ", stage_num)
print("accuracy: ", hit / branch_num)

# %%
