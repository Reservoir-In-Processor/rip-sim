#include "Simulator/Simulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

class RiscvTests : public ::testing::TestWithParam<std::string> {};
void Simulator::execRISCVTESTS() {
  PC = DRAM_BASE;
  while (auto I = Dec.decode(Mem.readWord(PC))) {
    if (PC == 0x804c)
      break;
#ifdef DEBUG
      // std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
      // I->pprint(std::cerr);
#endif
    // TODO: non-machine mode
    if (auto E = I->exec(PC, GPRegs, Mem, States, Mode)) {
      Address ExceptionPC = PC;
      ModeKind PrevMode = Mode;
      unsigned Cause = *E;
      if (Mode == ModeKind::Machine) {
        PC = States.read(MTVEC) & (~1);

        States.write(MEPC, ExceptionPC & (~1));

        States.write(MCAUSE, Cause);

        // Machine Trap Value Register
        States.write(MTVAL, trap_val(*E, ExceptionPC, I->getVal()));

        // set MPIE to MIE;
        // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of
        // MSTATUS MPIE: Previous Interrupt-Enable bit for machine mode. 7-th
        // bit of MSTATUS
        CSRVal MSTATUSVal = States.read(MSTATUS);
        bool MIE = (bool)((MSTATUSVal >> 3) & 1);
        States.write(MSTATUS, (MSTATUSVal & 0xffffff7f) | (MIE << 7));

        // Set MIE to 0
        States.write(MSTATUS, MSTATUSVal & 0xfffffff7);

        // set MPP to prev mode
        States.write(MSTATUS, (MSTATUSVal & 0xffffe7ff) | (PrevMode << 11));

      } else {
        assert(false && "Non-Machine mode is unimplemented!");
        return;
      }
    }
#ifdef DEBUG
    std::cerr << "Regs after:\n";
    dumpGPRegs();
#endif
  }
#ifdef DEBUG
  std::cerr << "finish with:\n";
  dumpGPRegs();
#endif
}

std::vector<std::string> getBinFilesWithPrefix(const std::string &directory,
                                               const std::string &prefix) {
  std::vector<std::string> binFiles;

  for (const auto &entry : std::filesystem::directory_iterator(directory)) {
    if (entry.is_regular_file() && entry.path().extension() == ".bin" &&
        entry.path().filename().string().substr(0, prefix.size()) == prefix) {
      binFiles.push_back(entry.path().string());
    }
  }

  return binFiles;
}

TEST_P(RiscvTests, RiscvTests) {
  std::string FileName = GetParam();
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files);
  Sim.execRISCVTESTS();
  const GPRegisters &Res = Sim.getGPRegs();
  EXPECT_EQ(Res[3], 1) << FileName << " failed\n";
}
INSTANTIATE_TEST_SUITE_P(
    RV32I, RiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32ui-p-")));

INSTANTIATE_TEST_SUITE_P(
    RV32M, RiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32um-p-")));