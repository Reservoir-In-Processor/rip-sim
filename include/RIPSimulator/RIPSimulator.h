
#ifndef RIPSIMULATOR_H
#define RIPSIMULATOR_H
#include "BranchPredictor.h"
#include "Decoder.h"
#include "Exceptions.h"
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
  IF, // Instruction Fetch
  DE, // DEcode
  EX, // EXecution
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

  // For operands
  RegVal EXRdVal;
  RegVal EXRs2Val;
  RegVal EXImmVal;
  RegVal EXCSRVal;
  RegVal MARdVal;
  RegVal MAImmVal;
  RegVal MACSRVal;
  RegVal DERs1Val;
  RegVal DERs2Val;
  RegVal DECSRVal;
  RegVal DEImmVal;
  RegVal WBImmVal;

  std::optional<Address> BranchPC;

  // pipeline bubbles are nullptr
  std::unique_ptr<Instruction> Insts[STAGENUM];
  Address PCs[STAGENUM];

  bool StalledStages[STAGENUM];
  bool InvalidStages[STAGENUM];

public:
  PipelineStates(const PipelineStates &) = delete;
  PipelineStates &operator=(const PipelineStates &) = delete;

  PipelineStates()
      : EXRdVal(0), EXRs2Val(0), EXImmVal(0), EXCSRVal(0), MARdVal(0),
        MAImmVal(0), MACSRVal(0), DERs2Val(0), DECSRVal(0), DEImmVal(0),
        WBImmVal(0), StalledStages{0}, InvalidStages{0} {}

  void dump();

  const RegVal &getEXRdVal() { return EXRdVal; }
  void setEXRdVal(const RegVal &V) { EXRdVal = V; }

  const RegVal &getEXRs2Val() { return EXRs2Val; }
  void setEXRs2Val(const RegVal &V) { EXRs2Val = V; }

  const RegVal &getEXImmVal() { return EXImmVal; }
  void setEXImmVal(const RegVal &V) { EXImmVal = V; }

  const RegVal &getEXCSRVal() { return EXCSRVal; }
  void setEXCSRVal(RegVal &V) { EXCSRVal = V; }

  const RegVal &getDERs1Val() { return DERs1Val; }
  void setDERs1Val(const RegVal &V) { DERs1Val = V; }

  const RegVal &getDERs2Val() { return DERs2Val; }
  void setDERs2Val(const RegVal &V) { DERs2Val = V; }

  const RegVal &getDECSRVal() { return DECSRVal; }
  void setDECSRVal(const RegVal &V) { DECSRVal = V; }

  const RegVal &getDEImmVal() { return DEImmVal; }
  void setDEImmVal(const RegVal &V) { DEImmVal = V; }

  const RegVal &getMARdVal() { return MARdVal; }
  void setMARdVal(const RegVal &V) { MARdVal = V; }

  const RegVal &getMAImmVal() { return MAImmVal; }
  void setMAImmVal(const RegVal &V) { MAImmVal = V; }

  const RegVal &getMACSRVal() { return MACSRVal; }
  void setMACSRVal(const RegVal &V) { MACSRVal = V; }

  const RegVal &getWBImmVal() { return WBImmVal; }
  void setWBImmVal(const RegVal &V) { WBImmVal = V; }

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

  const bool isStall(const STAGES &S) { return StalledStages[S]; }
  // FIMXE: stall is contiguous, so set the stage and following stages may be
  // enough?
  void setStall(const STAGES &S) { StalledStages[S] = true; }
  void clearStall() {
    for (int Stage = STAGES::WB; STAGES::IF <= Stage; --Stage)
      StalledStages[Stage] = false;
  }

  const bool isInvalid(const STAGES &S) { return InvalidStages[S]; }
  void setInvalid(const STAGES &S) { InvalidStages[S] = true; }

  const std::unique_ptr<Instruction> &operator[](STAGES Stage) const {
    assert(Stage < STAGENUM && "Index out of bounds");
    return Insts[Stage];
  }

  std::unique_ptr<Instruction> &operator[](STAGES Stage) {
    assert(Stage < STAGENUM && "Index out of bounds");
    return Insts[Stage];
  }

  void proceed(std::unique_ptr<Instruction> InstPtr) {
    for (int Stage = STAGES::WB; STAGES::IF < Stage; --Stage) {
      if (isStall((STAGES)(Stage - 1)))
        return;
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

  void fillBubble() {
    for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
      if (InvalidStages[Stage]) {
        Insts[Stage] = nullptr;
        InvalidStages[Stage] = false;
      }
    }
  }

  void proceedPC(const Address PC) {
    for (int Stage = STAGES::WB; STAGES::IF < Stage; --Stage) {
      if (isStall((STAGES)(Stage - 1)))
        return;
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
  CSRs States;
  ModeKind Mode;
  unsigned NumStages;
  PipelineStates PS;
  GPRegisters GPRegs;
  Decoder Dec;
  std::unique_ptr<BranchPredictor> BP;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is, std::unique_ptr<BranchPredictor> BP = nullptr);
  bool getBPPred() {
    if (BP)
      return BP->getPrevPred();
    else
      assert(false && "No Branch Predictor!");
  }

  GPRegisters &getGPRegs() { return GPRegs; }
  PipelineStates &getPipelineStates() { return PS; }
  // FIXME: is it correct to define CSRs?
  inline const CSRs &getCSRs() const { return States; }
  unsigned getNumStages() { return NumStages; }
  // inherently unused arguments, but better to see dependencies
  void writeback(GPRegisters &, PipelineStates &);
  void memoryaccess(Memory &, PipelineStates &);
  std::optional<Exception> exec(PipelineStates &);
  void decode(GPRegisters &, PipelineStates &);
  void fetch(Memory &, PipelineStates &);
  void dumpStats();
  // FIXME: currently return recoverable or not.
  bool handleException(Exception &E);

  void runFromDRAMBASE();
  bool proceedNStage(unsigned N);

  void dumpGPRegs() { GPRegs.dump(); }
  void dumpCSRegs() { States.dump(); }
  Address &getPC() { return PC; }
  void setPC(Address Ad) { PC = Ad; };
};

#endif
