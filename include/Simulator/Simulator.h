
#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "CSR.h"
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
  Memory Mem;
  unsigned CodeSize;
  Address PC;
  CSRs States;
  ModeKind Mode;
  GPRegisters GPRegs;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  Simulator(std::istream &is);

  inline const GPRegisters &getGPRegs() const { return GPRegs; }
  inline const CSRs &getCSRs() const { return States; }

  void execFromDRAMBASE();
  void execRISCVTESTS();

  inline const void dumpGPRegs() const { GPRegs.dump(); }
  inline const Address &getPC() const { return PC; }
};

#endif
