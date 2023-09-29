
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
  unsigned EXDestReg;
  unsigned MADestReg;
  unsigned FetchedInst;
  Reg EXResult;
  std::shared_ptr<Instruction> Insts[STAGENUM];
  bool AreStall[STAGENUM];

public:
  PipelineStates(const PipelineStates &) = delete;
  PipelineStates &operator=(const PipelineStates &) = delete;

  PipelineStates() {}
  void dump() { assert(false && "unimplemented!"); }

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
  PipelineStates PS;
  GPRegisters GPRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is) : PC(DRAM_BASE) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    // starts from DRAM_BASE
    Address P = DRAM_BASE;
    // TODO: Write once
    while (is.read(Buff, 4)) {
      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // Raw inst
      M.writeWord(P, InstVal);
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
    unsigned CycleNum = 0;
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
