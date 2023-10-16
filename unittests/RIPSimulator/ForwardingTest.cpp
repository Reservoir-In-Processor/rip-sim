#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

TEST(RIPSimulatorTest, RS1_FORWARD) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi  x16, x0, 5
      0x13, 0x08, 0x30, 0x00, // addi  x16, x0, 3
      0x13, 0x09, 0xa8, 0x00, // addi  x18, x16, 10
  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  RIPSimulator RSim(ss);
  RSim.proceedNStage(4);

  PipelineStates &PS = RSim.getPipelineStates();

  EXPECT_EQ(PS.getDERs1Val(), 3);
}

TEST(RIPSimulatorTest, CSR_FORWARD) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0xe0, 0x00, // addi x5, x0, 14
      0x73, 0x90, 0x52, 0x30, // csrrw x0, mtvec, x5
      0x73, 0x25, 0x50, 0x30, // csrrs x10, mtvec, x0
  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  RIPSimulator RSim(ss);
  RSim.proceedNStage(4);

  PipelineStates &PS = RSim.getPipelineStates();

  EXPECT_EQ(PS.getDECSRVal(), 14);
}

TEST(RIPSimulatorTest, RS2_FORWARD) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
      0x13, 0x02, 0x60, 0x00, // addi x4, x0, 6
      0xb3, 0x82, 0x41, 0x00, // add x5, x3, x4
  };

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  RIPSimulator RSim(ss);
  RSim.proceedNStage(4);

  PipelineStates &PS = RSim.getPipelineStates();

  EXPECT_EQ(PS.getDERs1Val(), 5);
  EXPECT_EQ(PS.getDERs2Val(), 6);
}