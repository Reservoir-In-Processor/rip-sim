from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path
import glob


def test_dhrystone_baremetal():
    matching_files = glob.glob("rip-tests/dhry-baremetal*.bin")

    if matching_files:
        first_matching_file = matching_files[0]
    rsim = RIPSimulator(Path(first_matching_file), BranchPredKind.No, False)
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
