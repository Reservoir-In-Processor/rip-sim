
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

  inline const GPRegisters &getGPRegs() const { return GPRegs; }

  void execFromDRAMBASE();

  inline const void dumpGPRegs() const { GPRegs.dump(); }
  inline const Address &getPC() const { return PC; }
};

#endif
