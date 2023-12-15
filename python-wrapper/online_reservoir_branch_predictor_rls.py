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
input_dim = 1
inputs = np.zeros(shape=(input_dim, 1))


def create_inputs(rsim: RIPSimulator) -> np.ndarray:
    """
    update inputs by res.
    preprocessing should be hardware friendly.
    """
    inputs[0] = rsim.previous_branch_result
    rsim.pipeline_states.clear()

    return inputs


rbp = ESN_RLS(reservoir_dim=100, input_dim=input_dim, output_dim=1)
while True:
    trap = rsim.proceed()
    if trap == RIPSimulator.Trap.EBREAK:  # end of Dhrystone
        break
    elif trap == RIPSimulator.Trap.BRANCH_PRED:
        rbp.train(rsim.previous_branch_result)
        inputs = create_inputs(rsim)
        rsim.predict(rbp.predict(inputs))
    else:
        assert False, "unreachable!"
