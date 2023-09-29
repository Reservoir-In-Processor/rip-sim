#include "RIPSimulator/RIPSimulator.h"

void RIPSimulator::writeback(GPRegisters &, PipelineStates &) {
  const auto &Inst = PS[STAGES::WB];
  if (auto II = std::dynamic_pointer_cast<IInstruction>(Inst)) {

  } else if (auto RI = std::dynamic_pointer_cast<RInstruction>(Inst)) {

  } else {
  }
  // dynamic_castをする
}
void RIPSimulator::memoryaccess(Memory &, PipelineStates &) {
  const auto &Inst = PS[STAGES::MA];
}
void RIPSimulator::exec(PipelineStates &) {
  const auto &Inst = PS[STAGES::EX];
  // stall if
}
void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  const auto &Inst = PS[STAGES::DE];
  // stall if
}
void RIPSimulator::fetch(Memory &, PipelineStates &) {
  const auto &Inst = PS[STAGES::IF];
  // stall if
}