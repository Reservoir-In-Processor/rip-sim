from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path
import glob


def test_dhrystone_baremetal_interactive_gshare():
    matching_files = glob.glob("rip-tests/dhry-baremetal*.bin")

    if matching_files:
        first_matching_file = matching_files[0]

    rsim = RIPSimulator(
        Path(first_matching_file),
        BranchPredKind.Interactive,
        output_sim_err=False,
    )
    pss = []
    learns = []
    cycle = 0
    while True:
        res = rsim.proceed()
        if res is None:
            break
        if res["Kind"] == "PipelineStates":
            cycle += 1
            pss.append(res)
        elif res["Kind"] == "Predict":
            # predict true if cycle % 2 == 0, false otherwise.
            rsim.predict(str(cycle % 2 == 0).lower())
        elif res["Kind"] == "Learn":
            learns.append(res)
        else:
            assert False, "unreachable!"
    assert pss[-1]["EX"]["PC"] == 0x0084, "end PC unmatched!"
