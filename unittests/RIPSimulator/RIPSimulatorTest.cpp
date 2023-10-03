
#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

TEST(RIPSimulatorTest, ADDI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi, x17, x0, 3
  };

  // const GPRegisters EXPECTED = {{16, 5}, {17, 3}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  // RIPSimulator RSim(ss);

  // FIXME: pass test and compare each cycle states if possible
}
