#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

TEST(RIPSimulatorTest, ONEBITBP_CHECKPRED) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // addi x5, x0, 0
      0x93, 0x82, 0x12, 0x00, // addi x5, x5, 1
      0x13, 0x03, 0x30, 0x00, // addi x6, x0, 3
      0xe3, 0xcc, 0x62, 0xfe, // blt x5, x6, -8
      0x93, 0x80, 0x10, 0x00, // addi x1, x1, 1
  };

  const GPRegisters EXPECTED = {{1, 1}, {5, 3}, {6, 3}};

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // pred 0

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 1); // pred 1

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 1); // pred 1
}

TEST(RIPSimulatorTest, ONEBITBP_BB) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x00, 0x00, // addi x16, x0, 0
      0x93, 0x08, 0x20, 0x00, // addi x17, x0, 2
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1
      0xe3, 0x1e, 0x18, 0xff, // bne x16, x17, -4
      0x63, 0x86, 0x18, 0x01, // beq x17, x17, 8
      0x93, 0x08, 0x20, 0x00, // addi x17, x0, 2
      0x93, 0x08, 0x20, 0x00, // addi x17, x0, 2
  };

  const GPRegisters EXPECTED = {{16, 2}, {17, 2}};

  const Address EXPECTED_PC = DRAM_BASE + 4 * 7;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }

  EXPECT_EQ(RSim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << RSim.getPC();
}
