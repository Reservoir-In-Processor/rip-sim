
#ifndef RIPSIMULATOR_H
#define RIPSIMULATOR_H
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include <memory>
#include <string>
#include <vector>

const unsigned STAGENUM = 5;
enum STAGES {
  IF,
  DE,
  EX,
  MA,
  WB,
};

class PipelineStates {
private:
  // member are the values we want to dump for every stages
  unsigned FetchedInst;
  // previous cycle values.
  RegVal EXRdVal;
  RegVal MARdVal;

  RegVal DERs1Val;
  RegVal DERs2Val;
  Address NextPC;
  // pipeline bubbles are nullptr
  std::shared_ptr<Instruction> Insts[STAGENUM];
  // better way to invalidate values or stall
  bool AreStall[STAGENUM];
  bool AreInvalid[STAGENUM];

public:
  PipelineStates(const PipelineStates &) = delete;
  PipelineStates &operator=(const PipelineStates &) = delete;

  PipelineStates() {}
  void dump() { assert(false && "unimplemented!"); }

  const RegVal &getEXRdVal() { return EXRdVal; }
  void setEXRdVal(RegVal &V) { EXRdVal = V; }

  const RegVal &getDERs1Val() { return DERs1Val; }
  void setDERs1Val(const RegVal &V) { DERs1Val = V; }

  const RegVal &getDERs2Val() { return DERs2Val; }
  void setDERs2Val(const RegVal &V) { DERs2Val = V; }

  const RegVal &getMARdVal() { return MARdVal; }
  void setMARdVal(const RegVal &V) { MARdVal = V; }

  const unsigned &getFetchedInst() { return FetchedInst; }
  void setFetchedInst(const unsigned &V) { FetchedInst = V; }

  const Address &getNextPC() { return NextPC; }
  void setNextPC(const Address &V) { NextPC = V; }

  const bool isStall(const STAGES &S) { return AreStall[S]; }
  void setStall(const STAGES &S) { AreStall[S] = true; }

  const bool isInvalid(const STAGES &S) { return AreInvalid[S]; }
  void setInvalid(const STAGES &S) { AreInvalid[S] = true; }

  const std::shared_ptr<Instruction> &operator[](unsigned Stage) const {
    assert(Stage < STAGENUM && "Index out of bounds");
    return Insts[Stage];
  }
};

class RIPSimulator {
private:
  Memory M;
  unsigned CodeSize;
  Address PC;
  unsigned CycleNum;
  PipelineStates PS;
  GPRegisters GPRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is) : PC(DRAM_BASE), CycleNum(0) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    // starts from DRAM_BASE
    Address P = DRAM_BASE;
    // TODO: Write once
    while (is.read(Buff, 4)) {
      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // Raw inst
      M.writeWord(P, InstVal);
      CodeSize += 4;
    }
  }
  GPRegisters &getGPRegs() { return GPRegs; }
  // inherently unused arguments, but better to see dependencies
  void writeback(GPRegisters &, PipelineStates &);
  void memoryaccess(Memory &, PipelineStates &);
  void exec(PipelineStates &);
  void decode(GPRegisters &, PipelineStates &);
  void fetch(Memory &, PipelineStates &);

  void runFromDRAMBASE() {
    PC = DRAM_BASE;
    while (true) {
      writeback(GPRegs, PS);

      // TODO: write address info to CRegs?
      memoryaccess(M, PS);

      // exec
      exec(PS);

      decode(GPRegs, PS);

      fetch(M, PS);
      // TODO: dump all queued instruction, and CRegs?
#ifdef DEBUG
      PS.dump();
      CycleNum++;
#endif
    }
  }

  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
