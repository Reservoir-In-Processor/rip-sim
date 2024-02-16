#include "Simulator/Simulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

const std::string testDir = "../rip-tests";
std::string findTest(std::string patStr) {
  std::regex pattern(patStr);
  try {
    for (const auto &entry : std::filesystem::directory_iterator(testDir)) {
      if (entry.is_regular_file()) {
        std::string fileName = entry.path().filename().string();
        if (std::regex_match(fileName, pattern)) {
          return entry.path().string();
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Standard exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown error occurred." << std::endl;
  }
  assert(false && "No test file found!");
}

TEST(DhrystoneTest, DhryStone) {
  std::string FileName = findTest("dhry_.*\\.bin");
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 28,
                /* DRAMBase = */ 0x0000,
                /* SPIvalue = */ 1LL << 25);
  Sim.run();
  Sim.dumpGPRegs();
  const Address PC = Sim.getPC();
  EXPECT_EQ(PC, 0x0490) << "PC unmatched!\n";
}
//
TEST(DhrystoneTest, DhryStoneBareMetal) {
  std::string FileName = findTest("dhry-baremetal_.*\\.bin");
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 28,
                /* DRAMBase = */ 0x0000,
                /* SPIvalue = */ 1LL << 25);
  Sim.run();
  Sim.dumpGPRegs();
  const Address PC = Sim.getPC();
  EXPECT_EQ(PC, 0x0084) << "PC unmatched!\n";
}

TEST(DhrystoneTest, DhryStoneExtended) {
  std::string FileName = findTest("dhry_.*\\.bin\\.rip");
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 28,
                /* DRAMBase = */ 0x0000,
                /* SPIvalue = */ 1LL << 25);
  Sim.run();
  Sim.dumpGPRegs();
  const Address PC = Sim.getPC();
  EXPECT_EQ(PC, 0x0540) << "PC unmatched!\n";
  EXPECT_EQ(1, 2) << "intentional fail\n";
}

TEST(DhrystoneTest, DhryStoneBareMetalExtended) {
  std::string FileName = findTest("dhry-baremetal_.*\\.bin\\.rip");
  auto Files = std::ifstream(FileName);
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 28,
                /* DRAMBase = */ 0x0000,
                /* SPIvalue = */ 1LL << 25);
  Sim.run();
  Sim.dumpGPRegs();
  const Address PC = Sim.getPC();
  EXPECT_EQ(PC, 0x108) << "PC unmatched!\n";
  EXPECT_EQ(1, 2) << "intentional fail\n";
}
