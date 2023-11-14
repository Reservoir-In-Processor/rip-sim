#include "RIPSimulator/RIPSimulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

TEST(RIPDhrystoneTest, DhryStone) { // FIXME: should it be separated?
  std::string FileName = "../rip-tests/dhry.bin";
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the requirements
  // for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp),
                    /*DRAMSize = */ 1LL << 28, /* DRAMBase = */ 0x8000,
                    /* SPIvalue = */ 1LL << 25);
  auto &PS = RSim.getPipelineStates();

  RSim.run();
  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x8490) << "PC unmatched!\n";
}

TEST(RIPDhrystoneTest, DhryStoneBareMetal) {
  std::string FileName = "../rip-tests/dhry-baremetal.bin";
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the requirements
  // for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp), /*DRAMSize = */ 1LL << 28);
  auto &PS = RSim.getPipelineStates();

  RSim.run();
  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x8084) << "PC unmatched!\n";
}