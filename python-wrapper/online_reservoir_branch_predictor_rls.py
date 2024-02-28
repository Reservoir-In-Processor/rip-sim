from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path

import numpy as np
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
    This function updates and returns the inputs for the branch prediction in a
    RISC-V emulator wrapper (RIPSimulator) using a reservoir network. The process
    involves taking the result of the previous branch prediction and preparing it
    as input for the reservoir network.

    The reservoir network utilizes the Recursive Least Squares (RLS) method for
    error propagation. This approach aims to improve the accuracy of branch
    prediction in the emulator by adapting to the patterns observed in the
    branching behavior of the simulated program.

    Parameters:
    rsim (RIPSimulator): An instance of RIPSimulator, which is a wrapper for the
    RISC-V emulator. This simulator runs a specified binary and provides
    functionalities for branch prediction and error output.

    Returns:
    np.ndarray: A numpy array containing the processed input for the reservoir
    network. This input is derived from the previous branch result in the RISC-V
    emulator.

    Note:
    The function clears the pipeline states of the RIPSimulator instance as part
    of its processing. This clearing is necessary to maintain the accuracy and
    consistency of the simulation state between branch predictions.

    The process and methodology are particularly suited for hardware-friendly
    implementation, ensuring efficient computation in emulation environments.
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
