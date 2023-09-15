#include "Simulator/Simulator.h"
#include <gtest/gtest.h>

TEST(SimulatorTest, ADDI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi, x17, x0, 3
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 3}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i]);
  }
}

TEST(SimulatorTest, SLTI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0xd0, 0xff, // addi, x16, x0, -3
      0x93, 0x28, 0xe8, 0xff, // slti  x17, x16, -2
      0x93, 0x27, 0xd8, 0xff, // slti  x15, x16, -3
  };

  const GPRegisters EXPECTED = {{16, -3}, {17, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, SLTIU) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x38, 0x68, 0x00, // sltiu, x17, x16, 6
      0x93, 0x37, 0x58, 0x00, // sltiu x15, x16, 5
  };

  const GPRegisters EXPECTED = {{16, 5}, {17, 1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, XORI) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x50, 0x00, // addi x16, x0, 5
      0x93, 0x48, 0x38, 0x00, // xori x17, x16, 3
      0x93, 0x47, 0xf8, 0xff, // xori x15, x16, -1
  };

  const GPRegisters EXPECTED = {{15, -6}, {16, 5}, {17, 6}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, ADD) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
      0x13, 0x02, 0x60, 0x00, // addi x4, x0, 6
      0xb3, 0x82, 0x41, 0x00, // add x5, x3, x4
  };

  const GPRegisters EXPECTED = {{3, 5}, {4, 6}, {5, 11}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, SUB) {
  const unsigned char BYTES[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
      0x13, 0x02, 0x60, 0x00, // addi x4, x0, 6
      0xb3, 0x82, 0x41, 0x40, // sub x2, x3, x4
  };

  const GPRegisters EXPECTED = {{3, 5}, {4, 6}, {5, -1}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, AUIPC) {
  const unsigned char BYTES[] = {
      0x97, 0x08, 0x00, 0x00, // auipc x17, 0
      0x17, 0x28, 0x00, 0x00, // auipc x16, 2
  };

  const GPRegisters EXPECTED = {{16, DRAM_BASE + 4 + 0x2000}, {17, DRAM_BASE}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, SWLW) {
  const unsigned char BYTES[] = {
      0x13, 0x08, 0x00, 0x80, // addi x16, x0, -2048
      0x93, 0x08, 0x30, 0x00, // addi x17, x0, 3
      0x23, 0x2e, 0x01, 0xff, // sw x16, -4(sp)
      0x03, 0x29, 0xc1, 0xff, // lw x18, -4(sp)
  };

  const GPRegisters EXPECTED = {{16, -2048}, {17, 3}, {18, -2048}};
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
}

TEST(SimulatorTest, JALR) {
  const unsigned char BYTES[] = {
      0x67, 0x09, 0xc0, 0x02, // jalr x18, x0, 44
  };

  const GPRegisters EXPECTED = {{18, DRAM_BASE + 4}};
  const Address EXPECTED_PC = 44;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BEQ) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BNE) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BLT) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BGE) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BLTU) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, BGEU) {
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

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}

TEST(SimulatorTest, JAL) {
  const unsigned char BYTES[] = {
      0x6f, 0x09, 0xc0, 0x00, // jal x18, 12
  };

  const GPRegisters EXPECTED = {{18, DRAM_BASE + 4}};
  const Address EXPECTED_PC = DRAM_BASE + 12;
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(BYTES), sizeof(BYTES));

  Simulator Sim(ss);
  Sim.execFromDRAMBASE();
  GPRegisters &Res = Sim.getGPRegs();

  for (unsigned i = 0; i < 32; ++i) {
    EXPECT_EQ(Res[i], EXPECTED[i])
        << "Register:" << i << ", expected: " << EXPECTED[i]
        << ", got: " << Res[i];
  }
  EXPECT_EQ(Sim.getPC(), EXPECTED_PC)
      << "PC"
      << ", expected: " << EXPECTED_PC << ", got: " << Sim.getPC();
}