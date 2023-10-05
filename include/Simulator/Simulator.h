
#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "Decoder.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include <memory>
#include <string>
#include <vector>

class Simulator {
private:
  Decoder Dec;
  Memory M;
  unsigned CodeSize;
  Address PC;
  GPRegisters GPRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  Simulator(std::istream &is);

  GPRegisters &getGPRegs() { return GPRegs; }

  void execFromDRAMBASE() {
    PC = DRAM_BASE;
    while (auto &I = PCInstMap[PC]) {
      I->exec(PC, GPRegs, M);
#ifdef DEBUG
      std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
      I->pprint(std::cerr);
      std::cerr << "Regs after:\n";
      dumpGPRegs();
#endif
    }
    std::cerr << "stop on no instraction address\n";
  }

  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
