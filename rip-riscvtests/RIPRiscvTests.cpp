#include "RIPSimulator/RIPSimulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

class RIPRiscvTests : public ::testing::TestWithParam<std::string> {};

void RIPSimulator::runRiscvTests() {
  PC = DRAM_BASE;

  while (true) {

    if (PS.getPCs(STAGES::WB) == 0x804c) {
      break;
    }
    // actual fetch and decode
    if (!PS.isStall(STAGES::IF)) {
      auto InstPtr = Dec.decode(Mem.readWord(PC));
      PS.proceedPC(PC);
      if (InstPtr) {
        PC += 4;
      }
      PS.proceed(std::move(InstPtr));
    } else {
      PS.proceedPC(-1);
      PS.proceed(nullptr);
    }

    PS.clearStall();

    std::optional<Exception> E = std::nullopt;
    if (PS[STAGES::WB] != nullptr)
      writeback(GPRegs, PS);
    if (PS[STAGES::MA] != nullptr)
      memoryaccess(Mem, PS);
    if (PS[STAGES::EX] != nullptr)
      E = exec(PS);
    if (PS[STAGES::DE] != nullptr)
      decode(GPRegs, PS);
    if (PS[STAGES::IF] != nullptr)
      fetch(Mem, PS);

    // Exception handling
    // TODO: move those handler to some functions i.e. take_trap
    if (E && !handleException(*E))
      break;

    PS.fillBubble();

    if (PS.isEmpty()) {
      break;
    }
    // FIXME: might this be wrong if branch prediction happens.
    if (auto NextPC = PS.takeBranchPC()) {
      std::cerr << std::hex << "Branch from " << PC << " to ";
      PC = *NextPC;
      std::cerr << std::hex << PC << "\n";
    }
    CycleNum++;
#ifdef DEBUG
    PS.dump();
    dumpGPRegs();
#endif
  }
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

TEST_P(RIPRiscvTests, RIPRiscvTests) {
  std::string FileName = GetParam();
  auto Files = std::ifstream(FileName);
  RIPSimulator RSim(Files);
  RSim.run(/*StartAddress = */ DRAM_BASE, /*EndAddress = */ DRAM_BASE + 0x4c);
  const GPRegisters &Res = RSim.getGPRegs();
  EXPECT_EQ(Res[3], 1) << FileName << " failed\n";
}
INSTANTIATE_TEST_SUITE_P(
    RV32I, RIPRiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32ui-p-")));

INSTANTIATE_TEST_SUITE_P(
    RV32M, RIPRiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32um-p-")));