#include "Decoder.h"
#include <gtest/gtest.h>
#include <memory>

TEST(DecoderTest, CSRRW) {
  unsigned char INST_BYTES[] = {
      0x73, 0x10, 0x00, 0xc0, // csrrw x0, cycle, x0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrw");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
}
