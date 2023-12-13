import sys
import json
import subprocess
from pathlib import Path
from enum import Enum


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

    def proceed(self):
        self.send_data("whatever\n")
        res_line = self.process.stdout.readline()

        if len(res_line.strip()) == 0:
            return None
        try:
            res_json = json.loads(res_line)
            return res_json
        except json.JSONDecodeError:
            print("fin?")
        return None

    def predict(self, pred: bool):
        assert self.branch_predictor_kind == BranchPredKind.Interactive
        self.send_data('{"PredRes":' + str(pred).lower() + "}")
        self.send_data(" FIXME!! ")
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
