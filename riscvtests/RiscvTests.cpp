#include "Simulator/Simulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

class RiscvTests : public ::testing::TestWithParam<std::string> {};
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
  Simulator Sim(Files, /* DRAMSize = */ 1 << 15);
  Sim.run(/*StartAddress = */ 0x8000, /*EndAddress = */ 0x8000 + 0x4c);
  const GPRegisters &Res = Sim.getGPRegs();
  EXPECT_EQ(Res[3], 1) << FileName << " failed\n";
}
INSTANTIATE_TEST_SUITE_P(
    RV32I, RiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32ui-p-")));

INSTANTIATE_TEST_SUITE_P(
    RV32M, RiscvTests,
    ::testing::ValuesIn(getBinFilesWithPrefix("../rip-tests", "rv32um-p-")));