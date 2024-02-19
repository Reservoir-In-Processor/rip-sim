#include "RIPSimulator/RIPSimulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

std::string testDir = "../rip-tests";
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

TEST(RIPDhrystoneTest, DhryStone) { // FIXME: should it be separated?
  std::string FileName = findTest("dhry_.*\\.bin");
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the
  // requirements for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp),
                    /*DRAMSize = */ 1LL << 28,
                    /* Stats = */ std::make_unique<Statistics>(),
                    /* DRAMBase = */ 0x0000,
                    /* SPIvalue = */ 1LL << 25);
  auto &PS = RSim.getPipelineStates();

  RSim.run();
  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x0490) << "PC unmatched!\n";
}

TEST(RIPDhrystoneTest, DhryStoneBareMetal) {
  std::string FileName = findTest("dhry-baremetal_.*\\.bin");
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the
  // requirements
  // for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp),
                    /*DRAMSize = */ 1LL << 28,
                    /* Stats = */ std::make_unique<Statistics>(),
                    /* DRAMBase = */ 0x0000,
                    /* SPIvalue = */ 1LL << 25);
  auto &PS = RSim.getPipelineStates();

  RSim.run();

  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x0084) << "PC unmatched!\n";
}

TEST(RIPDhrystoneTest, DhryStoneExtended) { // FIXME: should it be separated?
  std::string FileName = findTest("dhry_.*\\.bin\\.rip");
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the
  // requirements for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp),
                    /*DRAMSize = */ 1LL << 28,
                    /* Stats = */ std::make_unique<Statistics>(),
                    /* DRAMBase = */ 0x0000,
                    /* SPIvalue = */ 1LL << 25);
  auto &PS = RSim.getPipelineStates();

  RSim.run();
  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x53c) << "PC unmatched!\n";
}

TEST(RIPDhrystoneTest, DhryStoneBareMetalExtended) {
  std::string FileName = findTest("dhry-baremetal_.*\\.bin\\.rip");
  auto Files = std::ifstream(FileName);
  // FIXME: access on above sp initial value happens, what is the requirements
  // for DRAMSize, DRAMBase, and sp init value?
  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(Files, /*BP = */ std::move(bp),
                    /*DRAMSize = */ 1LL << 28,
                    /* Stats = */ std::make_unique<Statistics>(),
                    /* DRAMBase = */ 0x0000,
                    /* SPIvalue = */ 1LL << 25);
  auto &PS = RSim.getPipelineStates();

  RSim.run();

  RSim.dumpGPRegs();
  const Address PC = PS.getPCs(STAGES::EX);
  EXPECT_EQ(PC, 0x104) << "PC unmatched!\n";
}