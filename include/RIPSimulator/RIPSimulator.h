
#ifndef RIPSIMULATOR_H
#define RIPSIMULATOR_H
#include "BranchPredictor.h"
#include "Decoder.h"
#include "Exceptions.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "PipelineStates.h"
#include "Registers.h"
#include "Statistics.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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

  // options
  std::unique_ptr<BranchPredictor> BP;
  std::unique_ptr<Statistics> Stats;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is, std::unique_ptr<BranchPredictor> BP = nullptr,
               Address _DRAMSize = 1 << 10,
               std::unique_ptr<Statistics> Stats = std::make_unique<
                   Statistics>(), // FIXME: default option is not dump, is this
                                  // good? and currently reversed.
               Address DRAMBase = 0x8000,
               std::optional<Address> SPIValue = std::nullopt);
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
  // FIXME: currently return recoverable or not.
  bool handleException(Exception &E);

  void run(std::optional<Address> StartAddress = std::nullopt,
           std::optional<Address> EndAddress = std::nullopt);

  void runInteractively(std::optional<Address> StartAddress = std::nullopt,
                        std::optional<Address> EndAddress = std::nullopt);
  bool proceedNStage(unsigned N);

  void dumpGPRegs() { GPRegs.dump(); }
  void dumpCSRegs() { States.dump(); }
  void dumpStats();
  Address &getPC() { return PC; }
  void setPC(Address Ad) { PC = Ad; };
};

#endif
