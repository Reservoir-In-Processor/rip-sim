from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path


def test_dhrystone_baremetal():
    rsim = RIPSimulator(Path("rip-tests/dhry-baremetal.bin"), BranchPredKind.No, False)
    jsons = []
    while True:
        res = rsim.proceed()
        if res is None:
            break
        assert (
            res["Kind"] == "PipelineStates"
        ), "not interactive branch predictor mode is only pipeline states"
        jsons.append(res)
    assert jsons[-1]["EX"]["PC"] == 0x0084, "end PC unmatched!"
