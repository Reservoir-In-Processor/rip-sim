from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path


def test_online_reservoir_bp():
    rsim = RIPSimulator(
        Path("rip-tests/dhry-baremetal.bin"),
        BranchPredKind.Interactive,
        output_sim_err=True,
    )
    pss = []
    learns = []
    cycle = 0
    print("hi I'm here")
    max_cycle = 100000
    while cycle < max_cycle:
        res = rsim.proceed()
        if res is None:
            break
        if res["Kind"] == "PipelineStates":
            cycle += 1
            # print(res)
        elif res["Kind"] == "Predict":
            # predict true if cycle % 2 == 0, false otherwise.
            # rsim.send_data('{ "PredRes": ' + str(cycle % 2 == 0) + " }")
            print(res)
            return
        elif res["Kind"] == "Learn":
            print(res)
        else:
            assert False, "unreachable!"
    # assert pss[-1]["EX"]["PC"] == 0x8084, "end PC unmatched!"
