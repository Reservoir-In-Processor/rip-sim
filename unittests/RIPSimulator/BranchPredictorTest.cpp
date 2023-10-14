#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

TEST(RIPSimulatorTest, FOR_LOOP) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // addi x5, x0, 0
      0x93, 0x82, 0x12, 0x00, // addi x5, x5, 1
      0x13, 0x03, 0x50, 0x00, // addi x6, x0, 5
      0xe3, 0xcc, 0x62, 0xfe, // blt x5, x6, -8
      0x93, 0x80, 0x10, 0x00, // addi x1, x1, 1
      0x13, 0x00, 0x00, 0x00, // addi x0, x0, 0
      0x13, 0x00, 0x00, 0x00, // addi x0, x0, 0
  };

  const GPRegisters EXPECTED = {{1, 1}, {5, 5}, {6, 5}};

  const Address EXPECTED_PC = DRAM_BASE + 4 * 7;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));
  // RIPSimulator RSim(ss);
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
