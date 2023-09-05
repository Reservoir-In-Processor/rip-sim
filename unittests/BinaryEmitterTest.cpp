#include "BinaryEmitter.h"
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <string>

// https://github.com/d0iasm/rvemu/blob/07994c5261f69a130b6abcea70ba1ed0e2c9bbaf/tests/rv32i.rs#L11-L13
TEST(BinaryEmitterTest, ITypes) {
  auto ss = std::stringstream("addi, x16, x0, 5\n"
                              "addi, x17, x0, 3\n"
                              "slti x17, x16, -2\n"
                              "sltiu, x17, x16, 5 \n"
                              "xori, x17, x16, 6 \n"
                              "ori, x17, x16, 6\n"
                              "andi, x17, x16, 7\n"
                              "jalr x18, x0, 44\n"
                              "lb x18, 4(x0)\n"
                              "lh x18, 4(x0)\n"
                              "lbu x18, 4(x0)\n"
                              "lw x18, 4(x0)\n"
                              "slli x17, x16, 3\n"
                              "srli x17, x16, 2\n"
                              "srai x17, x16, 2\n");
  unsigned char Expected[] = {
      0x13, 0x08, 0x50, 0x00, // addi, x16, x0, 5
      0x93, 0x08, 0x30, 0x00, // addi, x17, x0, 3
      0x93, 0x28, 0xe8, 0xff, // slti x17, x16, -2
      0x93, 0x38, 0x58, 0x00, // sltiu, x17, x16, 5
      0x93, 0x48, 0x68, 0x00, // xori, x17, x16, 6
      0x93, 0x68, 0x68, 0x00, // ori, x17, x16, 6
      0x93, 0x78, 0x78, 0x00, // andi, x17, x16, 7
      0x67, 0x09, 0xc0, 0x02, // jalr x18, x0, 44
      0x03, 0x09, 0x40, 0x00, // lb x18, 4(x0)
      0x03, 0x19, 0x40, 0x00, // lh x18, 4(x0)
      0x03, 0x49, 0x40, 0x00, // lbu x18, 4(x0)
      0x03, 0x29, 0x40, 0x00, // lw x18, 4(x0)
      0x93, 0x18, 0x38, 0x00, // slli x17, x16, 3
      0x93, 0x58, 0x28, 0x00, // srli x17, x16, 2
      0x93, 0x58, 0x28, 0x40, // srai x17, x16, 2
  };

  BinaryEmitter BE(ss);
  std::ostringstream OSS;
  BE.emitBinary(OSS);
  std::string Bin = OSS.str();
  // TODO: debug byte util
  int cnt = 0;
  for (char c : Bin) {
    std::cerr << std::hex << std::setw(2) << std::setfill('0')
              << (0xff & static_cast<unsigned char>(c)) << ' ';
    ++cnt;
    if (cnt % 4 == 0)
      std::cerr << "\n";
  }

  EXPECT_EQ(std::memcmp(Bin.data(), Expected, sizeof(Expected)), 0);
}

TEST(BinaryEmitterTest, RType) {
  auto ss = std::stringstream("add x2, x3, x4\n"
                              "sub x2, x3, x4\n"
                              "sll x18, x16, x17\n"
                              "slt x18, x16, x17\n"
                              "sltu x18, x17, x16\n"
                              "xor x18, x16, x17\n"
                              "srl x18, x16, x17\n"
                              "sra x18, x16, x17\n"
                              "or x18, x16, x17\n"
                              "and x18, x16, x17\n");

  unsigned char Expected[] = {
      0x33, 0x81, 0x41, 0x00, // add x2, x3, x4
      0x33, 0x81, 0x41, 0x40, // sub x2, x3, x4
      0x33, 0x19, 0x18, 0x01, // sll x18, x16, x17
      0x33, 0x29, 0x18, 0x01, // slt x18, x16, x17
      0x33, 0xb9, 0x08, 0x01, // sltu x18, x17, x16
      0x33, 0x49, 0x18, 0x01, // xor x18, x16, x17
      0x33, 0x59, 0x18, 0x01, // srl x18, x16, x17
      0x33, 0x59, 0x18, 0x41, // sra x18, x16, x17
      0x33, 0x69, 0x18, 0x01, // or x18, x16, x17
      0x33, 0x79, 0x18, 0x01, // and x18, x16, x17
  };

  BinaryEmitter BE(ss);
  std::ostringstream OSS;
  BE.emitBinary(OSS);
  std::string Bin = OSS.str();

  EXPECT_EQ(std::memcmp(Bin.data(), Expected, sizeof(Expected)), 0);
}