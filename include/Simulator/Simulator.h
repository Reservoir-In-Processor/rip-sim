
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

#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

class Simulator {
private:
  Decoder Dec;
  Memory M;
  unsigned CodeSize;
  Address PC;
  // FIXME: byref?
  GPRegisters GPRegs;
  CustomRegisters CRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  // move this def to .cpp
  Simulator(std::istream &is) : PC(DRAM_BASE) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    // starts from DRAM_BASE
    Address P = DRAM_BASE;
    while (is.read(Buff, 4)) {

      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // Currently, Instruction level simulator doesn't use raw inst code, only
      // use cached instruction class values.
      M.writeWord(P, InstVal);
      Dec.decode(InstVal);
      P += 4;
      CodeSize += 4;
    }
  }
  GPRegisters &getGPRegs() { return GPRegs; }

  void execFromDRAMBASE() {
    PC = DRAM_BASE;
    while (auto &I = PCInstMap[PC]) {
      I->exec(PC, GPRegs, M, CRegs);
#ifdef DEBUG
      std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
      I->pprint(std::cerr);
      std::cerr << "Regs after:\n";
      dumpGPRegs();
#endif
      // TODO: dump instruction detail.
    }
    std::cerr << "stop on no instraction address\n";
  }

  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
