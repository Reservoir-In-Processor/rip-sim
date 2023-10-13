#include "Decoder.h"
#include <gtest/gtest.h>
#include <memory>

TEST(DecoderTest, BType) {
  unsigned char BEQ[] = {
      0x63, 0x06, 0x18, 0x01, // beq x16, x17, 12
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(BEQ));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "beq");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  // TODO: all insts
}

TEST(DecoderTest, SType) {
  unsigned char SW[] = {
      0x23, 0x2e, 0x01, 0xff, // sw x16, -4(sp)
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(SW));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "sw");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  // TODO: all insts
}

TEST(DecoderTest, IType) {
  unsigned char ADDI[] = {
      0x93, 0x01, 0x50, 0x00, // addi x3, x0, 5
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(ADDI));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "addi");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  unsigned char LW[] = {
      0x03, 0x29, 0xc1, 0xff, // lw x18, -4(sp)
  };

  InstVal = *(reinterpret_cast<unsigned *>(LW));

  InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "lw");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  // TODO: all insts
}

TEST(DecoderTest, RType) {
  unsigned char ADD[] = {
      0xb3, 0x82, 0x41, 0x00, // add x5, x3, x4
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(ADD));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "add");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  // TODO: all insts
}

TEST(DecoderTest, UType) {
  unsigned char AUIPC[] = {
      0x97, 0x08, 0x00, 0x00, // auipc x17, 0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(AUIPC));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "auipc");
  EXPECT_EQ(InstPtr->getVal(), InstVal);

  unsigned char LUI[] = {
      0x37, 0x28, 0x00, 0x00, // lui x16, 2
  };

  InstVal = *(reinterpret_cast<unsigned *>(LUI));

  InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "lui");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

/* CSR */

// ecall
TEST(DecoderTest, ECALL) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x00, 0x00, // ecall
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "ecall");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}
// ebreak
TEST(DecoderTest, EBREAK) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x10, 0x00, // ebreak
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "ebreak");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}
// uret,
TEST(DecoderTest, URET) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x20, 0x00, // uret
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "uret");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}
// sret,
TEST(DecoderTest, SRET) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x20, 0x10, // sret
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "sret");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

TEST(DecoderTest, MRET) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x20, 0x30, // mret
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "mret");
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

TEST(DecoderTest, CSRRCI) {
  unsigned char INST_BYTES[] = {
      0x73, 0x70, 0x00, 0xc0, // csrrci x0, cycle, 0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrci");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}
// csrrsi

TEST(DecoderTest, CSRRSI) {
  unsigned char INST_BYTES[] = {
      0x73, 0x60, 0x00, 0xc0, // csrrsi x0, cycle, 0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrsi");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

TEST(DecoderTest, CSRRWI) {
  unsigned char INST_BYTES[] = {
      0x73, 0x50, 0x00, 0xc0, // csrrwi x0, cycle, 0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrwi");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

TEST(DecoderTest, CSRRC) {
  unsigned char INST_BYTES[] = {
      0x73, 0x30, 0x00, 0xc0, // csrrw x0, cycle, x0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrc");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

TEST(DecoderTest, CSRRS) {
  unsigned char INST_BYTES[] = {
      0x73, 0x20, 0x00, 0xc0, // csrrs x0, cycle, x0
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "csrrs");
  EXPECT_EQ(InstPtr->getRd(), 0);
  EXPECT_EQ(InstPtr->getRs1(), 0);
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}

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
  EXPECT_EQ(InstPtr->getVal(), InstVal);
}
