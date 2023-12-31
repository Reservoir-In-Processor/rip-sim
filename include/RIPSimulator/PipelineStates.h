#ifndef PIPELINESTATES_H
#define PIPELINESTATES_H

#include <CommonTypes.h>
#include <Instructions.h>
#include <iostream>
#include <map>
#include <string>

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

  // Decode
  RegVal DERs1Val;
  RegVal DERs2Val;
  RegVal DECSRVal;
  RegVal DEImmVal;

  // Execution
  RegVal EXRdVal;
  RegVal EXRs2Val;
  RegVal EXImmVal;
  RegVal EXCSRVal;

  // Memory Access
  RegVal MARdVal;
  RegVal MAImmVal;
  RegVal MACSRVal;

  // Write Back
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
      : DERs2Val(0), DECSRVal(0), DEImmVal(0), EXRdVal(0), EXRs2Val(0),
        EXImmVal(0), EXCSRVal(0), MARdVal(0), MAImmVal(0), MACSRVal(0),
        WBImmVal(0), StalledStages{0}, InvalidStages{0} {}

  void dump();
  void printJSON(std::ostream &);

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

#endif