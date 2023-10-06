#include "Decoder.h"
#include <gtest/gtest.h>
#include <memory>
// ecall
TEST(DecoderTest, ECALL) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x00, 0x00, // ecall
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "ecall");
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
}

TEST(DecoderTest, MRET) {
  unsigned char INST_BYTES[] = {
      0x73, 0x00, 0x20, 0x30, // mret
  };

  unsigned InstVal = *(reinterpret_cast<unsigned *>(INST_BYTES));

  Decoder Dec;
  std::unique_ptr<Instruction> InstPtr = Dec.decode(InstVal);
  EXPECT_EQ(InstPtr->getMnemo(), "mret");
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
}
