from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path


def test_dhrystone_baremetal_interactive_gshare():
    rsim = RIPSimulator(
        Path("rip-tests/dhry-baremetal.bin"),
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
            learns.push(res)
        else:
            assert False, "unreachable!"
    assert pss[-1]["EX"]["PC"] == 0x8084, "end PC unmatched!"
