import sys
import json
import subprocess
from pathlib import Path
from enum import Enum
from typing import Any
from copy import deepcopy


class BranchPredKind(Enum):
    No = "no"
    One = "onebit"
    Two = "twobit"
    Gshare = "gshare"
    Interactive = "interactive"


class RIPSimulator:
    def __init__(
        self,
        input_file_path: Path,
        branch_predictor_kind: BranchPredKind,
        output_sim_err: bool,
    ):
        args = [
            str(Path(__file__).parent / "rip-sim"),
            "-i",
            f"{str(input_file_path)}",
            f"-b={branch_predictor_kind.value}",
            "--dram-size=268435456",
            "--stats",
            "-i",
        ]
        # FIXME: work around for jupyter cell output
        try:
            stderr = sys.stdout.buffer
        except Exception:
            stderr = None
        if not output_sim_err:
            stderr = None
        self.process = subprocess.Popen(
            args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=stderr,
            text=True,
        )
        self.branch_predictor_kind = branch_predictor_kind
        self.pipeline_states = []
        self.previous_branch_result = False  # FIXME: should this be neutral?

    class Trap(Enum):
        EBREAK = 0
        BRANCH_PRED = 1

    def proceed(self) -> Trap:
        while True:
            res_line = self.process.stdout.readline()
            if len(res_line.strip()) == 0:
                return self.Trap.EBREAK
            try:
                res_json = json.loads(res_line)
                if res_json["Kind"] == "PipelineStates":
                    self.pipeline_states.append(res_json)
                    continue
                elif res_json["Kind"] == "Predict":
                    return self.Trap.BRANCH_PRED
                elif res_json["Kind"] == "Learn":
                    self.previous_branch_result = res_json["Cond"]
                    continue
            except json.JSONDecodeError:
                # FIXME: error output ?
                print("broken json")
                return None

    def predict(self, pred: bool):
        assert self.branch_predictor_kind == BranchPredKind.Interactive
        self.send_data('{"PredRes":' + str(pred).lower() + "}\n")
        return None

    def send_data(self, data):
        self.process.stdin.write(data)
        self.process.stdin.flush()

    def close(self):
        self.process.stdin.close()
        self.process.terminate()
        self.process.wait()

    def __del__(self):
        self.close()
