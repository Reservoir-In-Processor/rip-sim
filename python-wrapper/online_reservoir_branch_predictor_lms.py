# %%
from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
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
from collections import defaultdict

inputs = np.zeros(shape=(input_dim, 1))


def create_inputs(rsim: RIPSimulator) -> np.ndarray:
    """
    update inputs by res.
    preprocessing should be hardware friendly.
    """
    inst_frequency_from_previous_branch: dict[str, int] = defaultdict(int)
    for pipeline_state in rsim.pipeline_states:
        assert pipeline_state["Kind"] == "PipelineStates"
        if "IF" in pipeline_state:
            inst_frequency_from_previous_branch[pipeline_state["IF"]["mnemo"]] += 1

    branch_dist = len(rsim.pipeline_states)
    inputs = np.zeros(shape=(input_dim, 1))
    inputs[0] = rsim.previous_branch_result
    inputs[1] = branch_dist
    inputs[2] = inst_frequency_from_previous_branch["addi"]
    rsim.pipeline_states.clear()

    return inputs


# initialize reservoir branch predictor
rbp = ESN_LMS(reservoir_dim=100, input_dim=input_dim, output_dim=1)

# %%
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

# %%
