from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
from tqdm import tqdm
from reservoir_branch_predictor.esn_rls import ESN_RLS


rsim = RIPSimulator(
    Path("../rip-tests/dhry.bin"),
    BranchPredKind.Interactive,
    output_sim_err=True,
)

prev_branch = False
rbp = ESN_RLS(reservoir_dim=100, input_dim=1, output_dim=1)

while True:
    res = rsim.proceed()
    if res is None:  # end of simulation
        break
    if res["Kind"] == "PipelineStates":
        # FIXME: update reservoir state by pipeline states?
        pass
    elif res["Kind"] == "Predict":
        # predict here, currently input is only previous prediction result.
        inputs = np.array([prev_branch]).reshape([1, 1])
        predict = rbp.predict(inputs)
        rsim.predict(predict)
    elif res["Kind"] == "Learn":
        # train here
        rbp.train(res["BranchResult"])
        prev_branch = res["BranchResult"]
    else:
        assert False, "unreachable!"
