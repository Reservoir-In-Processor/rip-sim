#include "RIPSimulator/RIPSimulator.h"
#include <set>

void RIPSimulator::writeback(GPRegisters &, PipelineStates &) {
  // save previous result
  GPRegs.write(PS[STAGES::WB]->getRd(), PS.getEXRdVal());
}
void RIPSimulator::memoryaccess(Memory &, PipelineStates &) {
  // const auto &Inst = PS[STAGES::MA];
  const RegVal MARdVal = PS.getEXRdVal();

  PS.setMARdVal(MARdVal);
}

const std::set<std::string> STALL_EX = {"lb",  "lh",   "lw",    "lbu",   "lhu",
                                        "beq", "blt",  "bge",   "bltu",  "bgeu",
                                        "jal", "jalr", "ecall", "ebreak"};
void RIPSimulator::exec(PipelineStates &) {
  const auto &Inst = PS[STAGES::EX];
  // TODO: calc
  RegVal Res = 0;
  if (auto II = std::dynamic_pointer_cast<IInstruction>(Inst)) {
    Res = 13;
  } else if (auto RI = std::dynamic_pointer_cast<RInstruction>(Inst)) {

  } else {
  }

  // TODO: invalid if
  if (STALL_EX.find(Inst->getMnemo()) != STALL_EX.end()) {
    // FIXME: invalide itself, is this right?
    PS.setInvalid(EX);
    PS.setInvalid(DE);
    PS.setInvalid(IF);
  }
  // TODO: Calculate next PC
  PS.setNextPC(PC + 4);
  PS.setEXRdVal(Res);
}

void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  // FIXME: PS indexing seems not consistent(it's not only instructions)
  const auto &Inst = PS[STAGES::DE];

  // TODO: forward if
  // FIXME: this is incorrect, because U-type and J-type instr's Rs1 or Rs2 is a
  // part of immediate.
  if (Inst->getRs1() == PS[STAGES::MA]->getRd()) {
    PS.setDERs1Val(PS.getMARdVal());
  }
  if (Inst->getRs2() == PS[STAGES::MA]->getRd()) {
    PS.setDERs2Val(PS.getMARdVal());
  }
  if (Inst->getRs1() == PS[STAGES::EX]->getRd()) {
    PS.setDERs1Val(PS.getEXRdVal());
  }
  if (Inst->getRs2() == PS[STAGES::EX]->getRd()) {
    PS.setDERs2Val(PS.getEXRdVal());
  }
}
void RIPSimulator::fetch(Memory &, PipelineStates &) {
  // const auto &Inst = PS[STAGES::IF];
  // TODO: stall if
  PS.setFetchedInst(M.readWord(PC));

  // FIXME: how and when can I change PC?
  // we can tantatively change here
  PC = PS.getNextPC();
}