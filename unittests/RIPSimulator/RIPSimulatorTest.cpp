
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

TEST(RIPSimulatorTest, AUIPC) {
  const unsigned char BYTES[] = {
      0x97, 0x08, 0x00, 0x00, // auipc x17, 0
      0x17, 0x28, 0x00, 0x00, // auipc x16, 2
  };

  const GPRegisters EXPECTED = {{16, DRAM_BASE + 4 + 0x2000}, {17, DRAM_BASE}};
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

TEST(RIPSimulatorTest, SWLW) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x00, 0x80, // addi x16, x0, -2048
      0x93, 0x08, 0x30, 0x00, // addi x17, x0, 3
      0x23, 0x2e, 0x01, 0xff, // sw x16, -4(sp)
      0x03, 0x29, 0xc1, 0xff, // lw x18, -4(sp)
  };

  const GPRegisters EXPECTED = {{16, -2048}, {17, 3}, {18, -2048}};
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

TEST(RIPSimulatorTest, SHLHLHU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x00, 0x80, // addi x16, x0, -2048
      0x23, 0x1e, 0x01, 0xff, // sh x16, -4(sp)
      0x83, 0x18, 0xc1, 0xff, // lh x17, -4(sp)
      0x03, 0x59, 0xc1, 0xff, // lhu x18, -4(sp)
  };
  // -2048 = 0xfffff800,
  // 63480 = 0x0000f800
  const GPRegisters EXPECTED = {{16, -2048}, {17, -2048}, {18, 63488}};
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

TEST(RIPSimulatorTest, SBLBLBU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xf0, 0xff, // addi x16, x0, -1
      0x23, 0x0e, 0x01, 0xff, // sb x16, -4(sp)
      0x83, 0x08, 0xc1, 0xff, // lb x17, -4(sp)
      0x03, 0x49, 0xc1, 0xff, // lbu x18, -4(sp)
  };
  const GPRegisters EXPECTED = {{16, -1}, {17, -1}, {18, 255}};
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

TEST(RIPSimulatorTest, JALR) {
  const unsigned char BYTES[] = {
      0x17, 0x02, 0x00, 0x00, // auipc x4, 0
      0x67, 0x02, 0xc2, 0x02, // jalr x4, x4, 44
  };

  const GPRegisters EXPECTED = {{4, DRAM_BASE + 8}};
  const Address EXPECTED_PC = DRAM_BASE + 44;
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

TEST(RIPSimulatorTest, JALR2) {
  const unsigned char BYTES[] = {
      0x17, 0x02, 0x00, 0x00, // 0x00, auipc x4, 0 // x4 = 0x00
      0x67, 0x02, 0xc2, 0x00, // 0x04, jalr x4, x4, 12
      // PC = 0x8000 + 12, x4  = 08

      0x13, 0x08, 0x18, 0x00, // 0x08, addi x16, x16, 1 // should be skipped
      0x13, 0x02, 0x12, 0x00, // 0x0c, addi x4, x4, 1 // shouldn't be skipped
  };

  const GPRegisters EXPECTED = {{4, DRAM_BASE + 4 + 4 + 1}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 4;
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

TEST(RIPSimulatorTest, BEQ2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // 0x00, addi x16, x0, 3
      0x93, 0x08, 0x30, 0x00, // 0x04, addi x17, x0, 3
      0x63, 0x16, 0x18, 0x01, // 0x08, bne x16, x17, 12 miss
      0x63, 0x06, 0x18, 0x01, // 0x0c, beq x16, x17, 12 miss
      // jump to 0x0c + 12 = 0x18

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 4}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, BNE2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi x17, x0, 3
      0x63, 0x06, 0x18, 0x01, // beq x16, x17, 12
      0x63, 0x16, 0x18, 0x01, // bne x16, x17, 12 // jump

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1
  };

  const GPRegisters EXPECTED = {{16, 7}, {17, 4}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, BLT2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi x16, x0, -3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x56, 0x18, 0x01, // bge x16, x17, 12
      0x63, 0x46, 0x18, 0x01, // blt x16, x17, 12

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1

  };

  const GPRegisters EXPECTED = {{16, -1}, {17, 6}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, BGE2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x60, 0x00, // addi x16, x0, 6
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x46, 0x18, 0x01, // blt x16, x17, 12
      0x63, 0x56, 0x18, 0x01, // bge x16, x17, 12

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1

  };

  const GPRegisters EXPECTED = {{16, 8}, {17, 6}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, BLTU2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x30, 0x00, // addi x16, x0, 3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x76, 0x18, 0x01, // bgeu x16, x17, 12
      0x63, 0x66, 0x18, 0x01, // bltu x16, x17, 12

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 6}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, BGEU2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi x16, x0, -3
      0x93, 0x08, 0x50, 0x00, // addi x17, x0, 5
      0x63, 0x66, 0x18, 0x01, // bltu x16, x17, 12
      0x63, 0x76, 0x18, 0x01, // bgeu x16, x17, 12

      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // addi x16, x16, 1 // shouldn't be skipped
      0x93, 0x88, 0x18, 0x00, // addi x17, x17, 1

  };

  const GPRegisters EXPECTED = {{16, -1}, {17, 6}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 9;
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

TEST(RIPSimulatorTest, JAL2) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // 0x00 addi x16, x0, -3
      0x6f, 0x09, 0xc0, 0x00, // 0x04 jal x18, 12

      0x13, 0x08, 0x18, 0x00, // 0x08, addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // 0x0c, addi x16, x16, 1 // should be skipped
      0x13, 0x08, 0x18, 0x00, // 0x10, addi x16, x16, 1 // shouldn't be skipped
      0x13, 0x08, 0x18, 0x00, // 0x14, addi x16, x16, 1 // shouldn't be skipped
  };

  const GPRegisters EXPECTED = {{16, -1}, {18, DRAM_BASE + 8}};
  const Address EXPECTED_PC = DRAM_BASE + 4 * 6;
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

TEST(RIPSimulatorTest, LUI) {
  const unsigned char BYTES[] = {
      0x37, 0x28, 0x00, 0x00, // lui x16, 2
      0xb7, 0xf0, 0xff, 0x7f, // lui	x1, 0x7ffff
      0xb7, 0x03, 0x00, 0x80, // lui	x7, 0x80000
  };
  // rd ={upimm, 12'b0}
  // x16 = 2 * 2^(12) = 8192

  const GPRegisters EXPECTED = {{1, 0x7ffff000}, {7, 0x80000000}, {16, 8192}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  RIPSimulator RSim(ss);
  RSim.runFromDRAMBASE();
  const GPRegisters &Res = RSim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(RIPSimulatorTest, FENCE_AS_NOP) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x0f, 0x00, 0x30, 0x02, // fence r, rw
      0x0f, 0x10, 0x00, 0x00, // fence.i
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

TEST(RIPSimulatorTest, CSRRS_CSRRWI) {
  const unsigned char BYTES[] = {
      0x73, 0x50, 0x26, 0x34, // csrrwi x0, mcause, 12
      0x73, 0x2f, 0x20, 0x34, // csrrs x30, mcause, x0
      0x73, 0xd0, 0x46, 0xf1, // csrrwi x0, mhartid, 13
      0x73, 0x25, 0x40, 0xf1, // csrrs x10, mhartid, x0
  };

  const GPRegisters EXPECTED = {{10, 13}, {30, 12}};
  const CSRs EXPECTED_C = {{MCAUSE, 12}, {MHARTID, 13}};
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
  const CSRs &ResC = RSim.getCSRs();
  for (CSRAddress CA : {MCAUSE, MHARTID}) {
    EXPECT_EQ(ResC[CA], EXPECTED_C[CA]);
  }
}

TEST(RIPSimulatorTest, CSRRS_CSRRSI) {
  const unsigned char BYTES[] = {
      0x73, 0x60, 0x26, 0x34, // csrrsi x0, mcause, 12
      0x73, 0x2f, 0x20, 0x34, // csrrs x30, mcause, x0
  };

  const GPRegisters EXPECTED = {{30, 12}};
  const CSRs EXPECTED_C = {{MCAUSE, 12}};
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
  const CSRs &ResC = RSim.getCSRs();
  for (CSRAddress CA : {MCAUSE, MHARTID}) {
    EXPECT_EQ(ResC[CA], EXPECTED_C[CA]);
  }
}

TEST(RIPSimulatorTest, CSRRS_CSRRCI) {
  const unsigned char BYTES[] = {
      0x13, 0x00, 0x10, 0x00, // addi x0, x0, 1
      0xf3, 0x11, 0x20, 0x34, // csrrw x3, mcause, x0
      0xf3, 0x71, 0x21, 0x34, // csrrci x3, mcause, 2
  };

  const GPRegisters EXPECTED = {{3, 1}};
  const CSRs EXPECTED_C = {{MCAUSE, 1}};
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
  const CSRs &ResC = RSim.getCSRs();
  for (CSRAddress CA : {MCAUSE, MHARTID}) {
    EXPECT_EQ(ResC[CA], EXPECTED_C[CA]);
  }
}

TEST(RIPSimulatorTest, CSRRW) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0xe0, 0x00, // addi x5, x0, 14
      0x73, 0x90, 0x52, 0x30, // csrrw x0, mtvec, x5
      0x73, 0x25, 0x50, 0x30, // csrrs x10, mtvec, x0
      0x13, 0x03, 0xf0, 0x00, // addi x6, x0, 15
      0x73, 0x10, 0x03, 0x18, // csrrw x0, satp, x6
      0x73, 0x2f, 0x00, 0x18, // csrrs x30, satp, x0
  };

  const GPRegisters EXPECTED = {{5, 14}, {6, 15}, {10, 14}, {30, 15}};
  const CSRs EXPECTED_C = {
      {MTVEC, 14},
      {SATP, 15},
  };
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
  const CSRs &ResC = RSim.getCSRs();
  for (CSRAddress CA : {MTVEC, SATP}) {
    EXPECT_EQ(ResC[CA], EXPECTED_C[CA]);
  }
}

TEST(RIPSimulatorTest, CSRRC) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0xe0, 0x00, // addi x5, x0, 14
      0x73, 0x90, 0x52, 0x30, // csrrw x0, mtvec, x5
      0x73, 0x35, 0x50, 0x30, // csrrc x10, mtvec, x0
  };

  const GPRegisters EXPECTED = {{5, 14}, {10, 14}};
  const CSRs EXPECTED_C = {
      {MTVEC, 14},
  };
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
  const CSRs &ResC = RSim.getCSRs();
  for (CSRAddress CA : {MTVEC, SATP}) {
    EXPECT_EQ(ResC[CA], EXPECTED_C[CA]);
  }
}

// TEST(RIPSimulatorTest, ECALLMMODE) {
//   const unsigned char BYTES[] = {
//       0x17, 0x02, 0x00, 0x00, // auipc x4, 0
//       0x13, 0x02, 0xc2, 0x01, // addi x4, x4, 28
//       0x73, 0x10, 0x52, 0x30, // csrrw x0, mtvec, x4
//       0x73, 0x00, 0x00, 0x00, // ecall
//   };

//   const GPRegisters EXPECTED = {{4, DRAM_BASE + 28}};
//   const CSRs EXPECTED_C = {
//       // FIXME: if last is illegal inst, then MEPC shouldn't be ecall's?
//       {MEPC, DRAM_BASE + 12},
//       {MCAUSE, Exception::EnvironmentCallFromMMode},
//       {MTVAL, 0},
//       {MTVEC, DRAM_BASE + 28},
//       // 1. default MSTATUS is zero.
//       // 2. prev mode is machine mode,
//       // 3. MIE and MPIE is zero
//       {MSTATUS, 0b1100000000000},
//   };
//   const Address EXPECTED_PC = DRAM_BASE + 28;
//   std::stringstream ss;
//   ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

//   RIPSimulator RSim(ss);
//   RSim.runFromDRAMBASE();
//   const GPRegisters &Res = RSim.getGPRegs();

//   for (unsigned i = 0; i < 32; ++i) {
//     EXPECT_EQ(Res[i], EXPECTED[i]);
//   }

//   const CSRs &ResC = RSim.getCSRs();
//   for (CSRAddress CA : {
//            MEPC,
//            MCAUSE,
//            MTVAL,
//            MTVEC,
//            MSTATUS,
//        }) {
//     EXPECT_EQ(ResC[CA], EXPECTED_C[CA])
//         << "Register:" << CSRNames[CA] << ", expected: " << EXPECTED_C[CA]
//         << ", got: " << ResC[CA];
//   }

//   EXPECT_EQ(RSim.getPC(), EXPECTED_PC)
//       << "PC"
//       << ", expected: " << EXPECTED_PC << ", got: " << RSim.getPC();
// }

// TEST(RIPSimulatorTest, MRET) {
//   const unsigned char BYTES[] = {
//       0x17, 0x02, 0x00, 0x00, // auipc x4, 0
//       0x13, 0x02, 0xc2, 0x01, // addi x4, x4, 28
//       0x73, 0x10, 0x52, 0x30, // csrrw x0, mtvec, x4
//       0x73, 0x00, 0x00, 0x00, // ecall
//       0x93, 0x00, 0x10, 0x00, // addi x1, x0, 1
//       0x93, 0x04, 0x90, 0x00, // addi x9, x0, 9
//       0x6f, 0x05, 0xa0, 0x02, // jal x10, 42
//       0x73, 0x25, 0x10, 0x34, // csrrs x10, mepc, x0
//       0x13, 0x05, 0x45, 0x00, // addi x10, x10, 4
//       0x73, 0x10, 0x15, 0x34, // csrrw x0, mepc, x10
//       0x73, 0x00, 0x20, 0x30, // mret
//   };

//   const GPRegisters EXPECTED = {
//       {1, 1}, {4, DRAM_BASE + 28}, {9, 9}, {10, DRAM_BASE + 28}};

//   const CSRs EXPECTED_C = {
//       {MCAUSE, Exception::EnvironmentCallFromMMode},
//       {MTVAL, 0},
//       {MTVEC, DRAM_BASE + 28},
//       // 1. default MSTATUS is zero.
//       // 2. prev mode is machine mode,
//       // 3. MIE is zero, because default MPIE is zero
//       // 4. MPIE is 1 and MPP is zero by mret.
//       {MSTATUS, 0b10000000},
//   };
//   const Address EXPECTED_PC = DRAM_BASE + 66;
//   std::stringstream ss;
//   ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

//   RIPSimulator RSim(ss);
//   RSim.runFromDRAMBASE();
//   const GPRegisters &Res = RSim.getGPRegs();

//   for (unsigned i = 0; i < 32; ++i) {
//     EXPECT_EQ(Res[i], EXPECTED[i])
//         << "Register:" << i << ", expected: " << EXPECTED[i]
//         << ", got: " << Res[i];
//   }

//   const CSRs &ResC = RSim.getCSRs();
//   for (CSRAddress CA : {
//            MCAUSE,
//            MTVAL,
//            MTVEC,
//            MSTATUS,
//        }) {
//     EXPECT_EQ(ResC[CA], EXPECTED_C[CA])
//         << "Register:" << CSRNames[CA] << ", expected: " << EXPECTED_C[CA]
//         << ", got: " << ResC[CA];
//     ;
//   }

//   EXPECT_EQ(RSim.getPC(), EXPECTED_PC)
//       << "PC"
//       << ", expected: " << EXPECTED_PC << ", got: " << RSim.getPC();
// }

TEST(RIPSimulatorTest, FOR_LOOP) {
  const unsigned char BYTES[] = {
      0x93, 0x02, 0x00, 0x00, // addi x5, x0, 0
      0x93, 0x82, 0x12, 0x00, // addi x5, x5, 1
      0x13, 0x03, 0x50, 0x00, // addi x6, x0, 5
      0xe3, 0xcc, 0x62, 0xfe, // blt x5, x6, -8
                              // 0x13, 0x03, 0xa0, 0x00, // addi x0, x0, 0
                              // 0x13, 0x03, 0xa0, 0x00, // addi x0, x0, 0
                              // 0x13, 0x03, 0xa0, 0x00, // addi x0, x0, 0
  };

  const GPRegisters EXPECTED = {{5, 5}, {6, 5}};

  const Address EXPECTED_PC = DRAM_BASE + 4 * 4;
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
