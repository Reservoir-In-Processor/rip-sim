
#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "CSR.h"
#include "Decoder.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include "Statistics.h"
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
  Statistics Stats;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  Simulator(std::istream &is, Address DRAMSize = 1 << 10,
            Address DRAMBase = 0x8000,
            std::optional<Address> SPIValue = std::nullopt);

  inline const GPRegisters &getGPRegs() const { return GPRegs; }
  inline const CSRs &getCSRs() const { return States; }

  void run(std::optional<Address> StartAddress = std::nullopt,
           std::optional<Address> EndAddress = std::nullopt);
  void execRISCVTESTS();
  // void execDhrystone();

  inline const void dumpGPRegs() const { GPRegs.dump(); }
  inline const void dumpStats() {
    std::cerr << "========== BEGIN STATS ============"
              << "\n";
    Stats.printAllStatistics(std::cerr);

    std::cerr << "=========== END STATS ============="
              << "\n";
    std::cerr << "\n";
  }
  inline const Address &getPC() const { return PC; }
};

#endif
