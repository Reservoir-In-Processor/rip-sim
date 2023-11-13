from rip_simulator.rip_simulator import RIPSimulator, BranchPredKind
from pathlib import Path


def test_dhrystone_baremetal():
    rsim = RIPSimulator(Path("rip-tests/dhry-baremetal.bin"), BranchPredKind.No)
    jsons = []
    while True:
        res = rsim.proceed_1cycle()
        if res is None:
            break
        jsons.append(res)
    assert jsons[-1]["EX"]["PC"] == 0x8084, "end PC unmatched!"
