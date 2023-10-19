#include "Simulator/Simulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

TEST(DhrystoneTest, DhryStone) {
  std::string FileName = "../rip-tests/dhry.bin";
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 30, /*DRAMBase = */ 0x0000,
                /*SPIValue = */ 1LL << 28);
  Sim.run();
  Sim.dumpGPRegs();
  const Address PC = Sim.getPC();
  EXPECT_EQ(PC, 0x4a4) << "PC unmatched!\n";
}