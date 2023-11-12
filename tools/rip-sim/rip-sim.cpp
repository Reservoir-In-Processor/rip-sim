#include <RIPSimulator/Options.h>
#include <RIPSimulator/RIPSimulator.h>
#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  Options Ops;
  if (!Ops.parse(argc, argv)) {
    return 1;
  }
  std::string FileName = Ops.getFileName();
  std::string BaseNoExt = FileName.substr(0, FileName.find_last_of('.'));
  auto Files = std::ifstream(FileName);
  std::unique_ptr<BranchPredictor> BP = nullptr;
  if (Ops.getBPKind() == BranchPredKind::No) {
    BP = nullptr;
  } else if (Ops.getBPKind() == BranchPredKind::One) {
    BP = std::make_unique<OneBitBranchPredictor>();
  } else if (Ops.getBPKind() == BranchPredKind::Two) {
    BP = std::make_unique<TwoBitBranchPredictor>();
  } else if (Ops.getBPKind() == BranchPredKind::Gshare) {
    BP = std::make_unique<GshareBranchPredictor>();
  } else {
    assert(false && "unreachable!");
  }
  RIPSimulator RipSim(Files, std::move(BP));
  RipSim.run();

  return 0;
}
