
#include "RIPSimulator/RIPSimulator.h"
#include <gtest/gtest.h>

TEST(RIPSimulatorTest, ADDI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi, x17, x0, 3
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 3}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();

  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, ADDI_FORWARD) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x08, 0x38, 0x00, // addi, x17, x16, 3
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 8}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));
  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();

  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, SLTI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi, x16, x0, -3
      0x93, 0x28, 0xe8, 0xff, // slti  x17, x16, -2
      0x93, 0x27, 0xd8, 0xff, // slti  x15, x16, -3
  };

  const GPRegisters EXPECTED = {{16, -3}, {17, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, SLTIU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x38, 0x68, 0x00, // sltiu, x17, x16, 6
      0x93, 0x37, 0x58, 0x00, // sltiu x15, x16, 5
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, XORI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x48, 0x38, 0x00, // xori x17, x16, 3
      0x93, 0x47, 0xf8, 0xff, // xori x15, x16, -1
  };

  const GPRegisters EXPECTED = {{15, -6}, {16, 5}, {17, 6}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, ORI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x68, 0x38, 0x00, // ori x17, x16, 3
      0x93, 0x67, 0xf8, 0xff, // ori x15, x16, -1
  };

  const GPRegisters EXPECTED = {{15, -1}, {16, 5}, {17, 7}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, ANDI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x78, 0x38, 0x00, // andi x17, x16, 3
      0x93, 0x77, 0xf8, 0xff, // andi x15, x16, -1
  };

  const GPRegisters EXPECTED = {{15, 5}, {16, 5}, {17, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, SLLI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x18, 0x38, 0x00, // slli x17, x16, 3
      0x93, 0x17, 0x18, 0x00, // slli x15, x16, 1
  };

  const GPRegisters EXPECTED = {{15, 10}, {16, 5}, {17, 40}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, SRAI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x80, 0xff, // addi x16, x0, -8
      0x93, 0x58, 0x28, 0x40, // srai x17, x16, 2
  };

  const GPRegisters EXPECTED = {{16, -8}, {17, -2}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, SRLI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x80, 0xff, // addi x16, x0, -8
      0x93, 0x58, 0x28, 0x00, // srli x17, x16, 2
  };
  // -8 = 0b11..1000
  // -8 l>> 2 = 0b0011...1110 = 1073741822

  const GPRegisters EXPECTED = {{16, -8}, {17, 1073741822}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, ADD) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
      0x13, 0x02, 0x60, 0x00, // addi x4, x0, 6
      0xb3, 0x82, 0x41, 0x00, // add x5, x3, x4
  };

  const GPRegisters EXPECTED = {{3, 5}, {4, 6}, {5, 11}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, SUB) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
      0x13, 0x02, 0x60, 0x00, // addi x4, x0, 6
      0xb3, 0x82, 0x41, 0x40, // sub x2, x3, x4
  };

  const GPRegisters EXPECTED = {{3, 5}, {4, 6}, {5, -1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, OR) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x33, 0x69, 0x18, 0x01, // xor x18, x16, x17
  };

  const GPRegisters EXPECTED = {{16, 3}, {17, 5}, {18, 7}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, AND) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x33, 0x79, 0x18, 0x01, // and x18, x16, x17
  };

  const GPRegisters EXPECTED = {{16, 3}, {17, 5}, {18, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, XOR) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x33, 0x49, 0x18, 0x01, // xor x18, x16, x17
  };

  const GPRegisters EXPECTED = {{16, 3}, {17, 5}, {18, 6}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(RIPSimulatorTest, BEQ) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x30, 0x00, // addi x17, x0, 3
      0x63, 0x16, 0x18, 0x01, // bne x16, x17, 12
      0x63, 0x06, 0x18, 0x01, // beq x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, 3}, {17, 3}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, BNE) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi x17, x0, 3
      0x63, 0x06, 0x18, 0x01, // beq x16, x17, 12
      0x63, 0x16, 0x18, 0x01, // bne x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 3}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, BLT) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi x16, x0, -3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x56, 0x18, 0x01, // bge x16, x17, 12
      0x63, 0x46, 0x18, 0x01, // blt x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, -3}, {17, 5}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, BGE) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x60, 0x00, // addi x16, x0, 6
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x46, 0x18, 0x01, // blt x16, x17, 12
      0x63, 0x56, 0x18, 0x01, // bge x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, 6}, {17, 5}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, BLTU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x76, 0x18, 0x01, // bgeu x16, x17, 12
      0x63, 0x66, 0x18, 0x01, // bltu x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, 3}, {17, 5}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, BGEU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi x16, x0, -3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x66, 0x18, 0x01, // bltu x16, x17, 12
      0x63, 0x76, 0x18, 0x01, // bgeu x16, x17, 12
  };

  const GPRegisters EXPECTED = {{16, -3}, {17, 5}};
  const Address EXPECTED_PC = DRAM_BASE + 12 + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, JAL) {
  const unsigned char BYTES[] = {
      0x6f, 0x09, 0xc0, 0x00, // jal x18, 12
  };

  const GPRegisters EXPECTED = {{18, DRAM_BASE + 4}};
  const Address EXPECTED_PC = DRAM_BASE + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
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

TEST(RIPSimulatorTest, ZERO1) {
  const unsigned char BYTES[] = {
      0x13, 0x00, 0x50, 0x00, // addi, x0, x0, 5
  };

  const GPRegisters EXPECTED = {};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, ZERO2) {
  const unsigned char BYTES[] = {
      0x6f, 0x00, 0xc0, 0x00, // jal x0, 12
  };

  const GPRegisters EXPECTED = {};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, MUL) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0xf0, 0xff, // addi x3, x0, -1
      0x13, 0x02, 0xf0, 0xff, // addi x4, x0, -1
      0xb3, 0x82, 0x41, 0x02, // mul x5, x3, x4
  };

  const GPRegisters EXPECTED = {{3, -1}, {4, -1}, {5, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, MULH) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0xf0, 0xff, // addi x3, x0, -1
      0x13, 0x02, 0x10, 0x00, // addi x4, x0, 1
      0xb3, 0x92, 0x41, 0x02, // mulh x5, x3, x4
  };

  const GPRegisters EXPECTED = {{3, -1}, {4, 1}, {5, -1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, MULHSU) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0xf0, 0xff, // addi x3, x0, -1
      0x13, 0x02, 0xf0, 0xff, // addi x4, x0, -1
      0xb3, 0xa2, 0x41, 0x02, // mulhsu x5, x3, x4
  };
  //  -1 * (2**32 - 1) = 0b11111111 11111111 00000000 00000001

  const GPRegisters EXPECTED = {{3, -1}, {4, -1}, {5, -1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, MULHU) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0xf0, 0xff, // addi x3, x0, -1
      0x13, 0x02, 0xf0, 0xff, // addi x4, x0, -1
      0xb3, 0xb2, 0x41, 0x02, // mulhu x5, x3, x4
  };
  // unsigned(-1) = 4294967295
  // unsigned(-1) = 4294967295
  // 4294967295 * 4294967295 = 18446744065119617025
  // bin(18446744065119617025) >> 32 = 4294967294
  // bin(4294967294) = 0b 1111.... 1110 = unsigned(-2)

  const GPRegisters EXPECTED = {{3, -1}, {4, -1}, {5, -2}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, DIV) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x90, 0x00, // addi x3, x0, 9
      0x13, 0x02, 0xe0, 0xff, // addi x4, x0,-2
      0xb3, 0xc2, 0x41, 0x02, // div x5, x3, x4
  };

  //   9 = -2 * -4 + 1

  const GPRegisters EXPECTED = {{3, 9}, {4, -2}, {5, -4}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, DIVU) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x90, 0x00, // addi x3, x0, 9
      0x13, 0x02, 0xe0, 0xff, // addi x4, x0,-2
      0xb3, 0xd2, 0x41, 0x02, // divu x5, x3, x4
  };

  // 9 = (2**(32) - 2) * 0 + 9

  const GPRegisters EXPECTED = {{3, 9}, {4, -2}, {5, 0}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, REM) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x90, 0x00, // addi x3, x0, 9
      0x13, 0x02, 0xc0, 0xff, // addi x4, x0, -4
      0xb3, 0xe2, 0x41, 0x02, // rem x5, x3, x4
  };

  //   9 = (-4) * (-2) + 1

  const GPRegisters EXPECTED = {{3, 9}, {4, -4}, {5, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, REMU) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x90, 0x00, // addi x3, x0, 9
      0x13, 0x02, 0xc0, 0xff, // addi x4, x0, -4
      0xb3, 0xf2, 0x41, 0x02, // remu x5, x3, x4
  };

  //   9 = (2**(32)-4) * 0 + 9

  const GPRegisters EXPECTED = {{3, 9}, {4, -4}, {5, 9}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}
