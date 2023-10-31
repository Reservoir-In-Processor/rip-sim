#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

const Address DRAM_BASE = 0x8000;
TEST(RIPSimulatorTest, FLUSHTEST) {
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
  RSim.run();
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

TEST(RIPSimulatorTest, FLUSHTEST2) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x70, 0x00, // li gp, 7
      0x93, 0x00, 0xf0, 0xff, // li ra,-1
      0x13, 0x01, 0xe0, 0xff, // li sp, -2
      0x63, 0xc4, 0x20, 0x00, // blt ra,sp, 8
      0x63, 0x14, 0x30, 0x00, // bne zero,gp, 8
      0x63, 0x1a, 0x30, 0x20, // bne zero,gp,44c
      0xe3, 0xce, 0x20, 0xfe, // blt ra,sp, -4
  };

  const Address EXPECTED_PC = DRAM_BASE + 4 * 7;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));
  RSim.run();

  EXPECT_EQ(RSim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << RSim.getPC();
}

TEST(RIPSimulatorTest, OneBITBP_CHECKPRED) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // 00, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x00, 0x00, // 04, addi t1, x0, 0 j = 0
      0x93, 0x03, 0x30, 0x00, // 08, addi t2, x0, 3 n = 3
      0x13, 0x0e, 0x00, 0x00, // 0c, addi t3, x0, 0 sum = 0

      0x63, 0xda, 0x72, 0x00, // 10, bge t0, t2, 20 for i < 3
      0x33, 0x0e, 0x5e, 0x00, // 14, add t3, t3, t0 sum = sum + i
      0x33, 0x0e, 0x6e, 0x00, // 18, add t3, t3, t1 sum = sum + j
      0x93, 0x82, 0x12, 0x00, // 1c, addi t0, t0, 1 i = i + 1
      0x6f, 0xf0, 0x1f, 0xff, // 20, jal x0, -16

      0x93, 0x02, 0x00, 0x00, // 24, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x13, 0x00, // 28, addi t1, t1, 1 j = j + 1
      0x63, 0x54, 0x73, 0x00, // 2c, bge t1, t2, 8 for j < 3:
      0x6f, 0xf0, 0x1f, 0xfe, // 30, jal x0, -32

  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<OneBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));

  DEBUG_ONLY(std::cerr << "------------ loop1 ------------"
                       << "\n");

  RSim.proceedNStage(6);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  DEBUG_ONLY(std::cerr << "------------ loop2 ------------"
                       << "\n");

  RSim.proceedNStage(4);
  EXPECT_EQ(RSim.getBPPred(), 1); // should be miss in 10

  RSim.proceedNStage(9);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  DEBUG_ONLY(std::cerr << "------------ loop3 ------------"
                       << "\n");

  RSim.proceedNStage(4);
  EXPECT_EQ(RSim.getBPPred(), 1); // should be miss in 10

  RSim.proceedNStage(9);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 2c

  RSim.run();
}

TEST(RIPSimulatorTest, TWOBITBP_CHECKPRED) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // 00, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x00, 0x00, // 04, addi t1, x0, 0 j = 0
      0x93, 0x03, 0x30, 0x00, // 08, addi t2, x0, 3 n = 3
      0x13, 0x0e, 0x00, 0x00, // 0c, addi t3, x0, 0 sum = 0

      0x63, 0xda, 0x72, 0x00, // 10, bge t0, t2, 20 for i < 3
      0x33, 0x0e, 0x5e, 0x00, // 14, add t3, t3, t0 sum = sum + i
      0x33, 0x0e, 0x6e, 0x00, // 18, add t3, t3, t1 sum = sum + j
      0x93, 0x82, 0x12, 0x00, // 1c, addi t0, t0, 1 i = i + 1
      0x6f, 0xf0, 0x1f, 0xff, // 20, jal x0, -16

      0x93, 0x02, 0x00, 0x00, // 24, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x13, 0x00, // 28, addi t1, t1, 1 j = j + 1
      0x63, 0x54, 0x73, 0x00, // 2c, bge t1, t2, 8 for j < 3:
      0x6f, 0xf0, 0x1f, 0xfe, // 30, jal x0, -32

  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<TwoBitBranchPredictor>();

  RIPSimulator RSim(ss, std::move(bp));

  DEBUG_ONLY(std::cerr << "------------ loop1 ------------"
                       << "\n");

  RSim.proceedNStage(6);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  DEBUG_ONLY(std::cerr << "------------ loop2 ------------"
                       << "\n");

  RSim.proceedNStage(4);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  DEBUG_ONLY(std::cerr << "------------ loop3 ------------"
                       << "\n");

  RSim.proceedNStage(4);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 2c

  RSim.run();
}

TEST(RIPSimulatorTest, GSHARE_CHECKPRED) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // 00, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x00, 0x00, // 04, addi t1, x0, 0 j = 0
      0x93, 0x03, 0x30, 0x00, // 08, addi t2, x0, 3 n = 3
      0x13, 0x0e, 0x00, 0x00, // 0c, addi t3, x0, 0 sum = 0

      0x63, 0xda, 0x72, 0x00, // 10, bge t0, t2, 20 for i < 3
      0x33, 0x0e, 0x5e, 0x00, // 14, add t3, t3, t0 sum = sum + i
      0x33, 0x0e, 0x6e, 0x00, // 18, add t3, t3, t1 sum = sum + j
      0x93, 0x82, 0x12, 0x00, // 1c, addi t0, t0, 1 i = i + 1
      0x6f, 0xf0, 0x1f, 0xff, // 20, jal x0, -16

      0x93, 0x02, 0x00, 0x00, // 24, addi t0, x0, 0 i = 0
      0x13, 0x03, 0x13, 0x00, // 28, addi t1, t1, 1 j = j + 1
      0x63, 0x54, 0x73, 0x00, // 2c, bge t1, t2, 8 for j < 3:
      0x6f, 0xf0, 0x1f, 0xfe, // 30, jal x0, -32

  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  std::unique_ptr<BranchPredictor> bp =
      std::make_unique<GshareBranchPredictor>();

  GshareBranchPredictor *gsharep =
      dynamic_cast<GshareBranchPredictor *>(bp.get());

  std::cerr << gsharep->getBranchHistory() << "\n";

  RIPSimulator RSim(ss, std::move(bp));
  auto &PS = RSim.getPipelineStates();

  DEBUG_ONLY(std::cerr << "------------ loop1 ------------"
                       << "\n");

  // BranchHistory is: 00010 00010 00010 00011

  RSim.proceedNStage(6);          // NS=5
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=12
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=19
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=26
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);          // NS=31
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  EXPECT_EQ(gsharep->getBranchHistory(), 0b1);

  EXPECT_EQ(gsharep->getBHTIndex(PS.getPCs(STAGES::DE)), 10);

  DEBUG_ONLY(std::cerr << "------------ loop2 ------------"
                       << "\n");

  RSim.proceedNStage(4);          // NS=35
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=42
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=49
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=56
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);          // NS=61
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 2c

  EXPECT_EQ(gsharep->getBranchHistory(), 0b100001);
  EXPECT_EQ(gsharep->getBHTIndex(PS.getPCs(STAGES::DE)), 42);

  DEBUG_ONLY(std::cerr << "------------ loop3 ------------"
                       << "\n");

  RSim.proceedNStage(4);          // NS=65
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=72
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=79
  EXPECT_EQ(RSim.getBPPred(), 0); // should be hit in 10

  RSim.proceedNStage(7);          // NS=86
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 10

  RSim.proceedNStage(5);          // NS=91
  EXPECT_EQ(RSim.getBPPred(), 0); // should be miss in 2c
  EXPECT_EQ(gsharep->getBHTIndex(PS.getPCs(STAGES::DE)), 42);

  RSim.run();
  EXPECT_EQ(gsharep->getBranchHistory(), 0b0001000011);
}
