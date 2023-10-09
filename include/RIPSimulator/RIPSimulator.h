
#ifndef RIPSIMULATOR_H
#define RIPSIMULATOR_H
#include "Decoder.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

const unsigned STAGENUM = 5;
enum STAGES {
  IF, // Instructioh Fetch
  DE, // DEcode
  EX, // EXection
  MA, // MemoryAccess
  WB, // (Register) Write BAck
};

static std::map<STAGES, std::string> StageNames = {
    {STAGES::IF, "IF"}, {STAGES::DE, "DE"}, {STAGES::EX, "EX"},
    {STAGES::MA, "MA"}, {STAGES::WB, "WB"},
};

class PipelineStates {
private:
  // Members are the values we want to dump for every stages
  unsigned FetchedInst;
  // FIXME: it's enough to be indeces.

  // For operands
  RegVal EXRdVal;
  RegVal MARdVal;
  RegVal DERs1Val;
  RegVal DERs2Val;
  RegVal DEImmVal;

  std::optional<Address> BranchPC;

  // pipeline bubbles are nullptr
  std::unique_ptr<Instruction> Insts[STAGENUM];
  Address PCs[STAGENUM];

  bool AreStall[STAGENUM];
  bool AreInvalid[STAGENUM];
  // int PipelinePCs[STAGENUM];

public:
  PipelineStates(const PipelineStates &) = delete;
  PipelineStates &operator=(const PipelineStates &) = delete;

  PipelineStates()
      : EXRdVal(0), MARdVal(0), DERs1Val(0), DERs2Val(0), DEImmVal(0) {}

  void dump();

  const RegVal &getEXRdVal() { return EXRdVal; }
  void setEXRdVal(RegVal &V) { EXRdVal = V; }

  const RegVal &getDERs1Val() { return DERs1Val; }
  void setDERs1Val(const RegVal &V) { DERs1Val = V; }

  const RegVal &getDERs2Val() { return DERs2Val; }
  void setDERs2Val(const RegVal &V) { DERs2Val = V; }

  const RegVal &getDEImmVal() { return DEImmVal; }
  void setDEImmVal(const RegVal &V) { DEImmVal = V; }

  const RegVal &getMARdVal() { return MARdVal; }
  void setMARdVal(const RegVal &V) { MARdVal = V; }

  const Address &getPCs(STAGES stage) { return PCs[stage]; }

  const unsigned &getFetchedInst() { return FetchedInst; }
  void setFetchedInst(const unsigned &V) { FetchedInst = V; }

  const std::optional<Address> takeBranchPC() {
    if (BranchPC) {
      Address BPC = *BranchPC;
      BranchPC = std::nullopt;
      return std::make_optional(BPC);
    } else {
      return std::nullopt;
    }
  }
  void setBranchPC(const Address &V) { BranchPC = V; }

  const bool isStall(const STAGES &S) { return AreStall[S]; }
  void setStall(const STAGES &S) { AreStall[S] = true; }

  const bool isInvalid(const STAGES &S) { return AreInvalid[S]; }
  void setInvalid(const STAGES &S) { AreInvalid[S] = true; }

  const std::unique_ptr<Instruction> &operator[](STAGES Stage) const {
    assert(Stage < STAGENUM && "Index out of bounds");
    return Insts[Stage];
  }

  std::unique_ptr<Instruction> &operator[](STAGES Stage) {
    assert(Stage < STAGENUM && "Index out of bounds");
    return Insts[Stage];
  }

  std::unique_ptr<Instruction> take(STAGES Stage) {
    return std::move(Insts[Stage]);
  }

  void push(std::unique_ptr<Instruction> InstPtr) {
    for (int Stage = STAGES::WB; STAGES::IF < Stage; --Stage) {
      Insts[Stage] = std::move(Insts[Stage - 1]);
    }
    Insts[STAGES::IF] = std::move(InstPtr);
  }

  bool isEmpty() {
    for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
      if (Insts[Stage] != nullptr)
        return false;
    }
    return true;
  }

  void pushPC(const Address PC) {
    for (int Stage = STAGES::WB; STAGES::IF < Stage; --Stage) {
      PCs[Stage] = std::move(PCs[Stage - 1]);
    }
    PCs[STAGES::IF] = std::move(PC);
  }
};

class RIPSimulator {
private:
  Memory Mem;
  unsigned CodeSize;
  Address PC;
  unsigned CycleNum;
  PipelineStates PS;
  GPRegisters GPRegs;
  Decoder Dec;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is);
  GPRegisters &getGPRegs() { return GPRegs; }
  // inherently unused arguments, but better to see dependencies
  void writeback(GPRegisters &, PipelineStates &);
  void memoryaccess(Memory &, PipelineStates &);
  void exec(PipelineStates &);
  void decode(GPRegisters &, PipelineStates &);
  void fetch(Memory &, PipelineStates &);

  void runFromDRAMBASE();
  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
