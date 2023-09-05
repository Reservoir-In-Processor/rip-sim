#include "AsmParser.h"
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(ParserTest, Basics) {
  auto ss = std::stringstream("addi a0, a0, 2047\n"
                              "xori a0, a0, 4\n");
  AsmParser AP(ss);

  EXPECT_EQ(0, AP.getLineCounter());
  EXPECT_TRUE(AP.parseLine());

  const auto &Addi = AP.getTokens();
  EXPECT_EQ(Addi[0], "addi") << "should addi";
  EXPECT_EQ(Addi[1], "a0") << "should a0";
  EXPECT_EQ(Addi[2], "a0") << "should a0";
  EXPECT_EQ(Addi[3], "2047") << "should 2047";

  EXPECT_TRUE(AP.parseLine());
  const auto &Xori = AP.getTokens();
  EXPECT_EQ(Xori[0], "xori") << "should xori";
  EXPECT_EQ(Xori[1], "a0") << "should a0";
  EXPECT_EQ(Xori[2], "a0") << "should a0";
  EXPECT_EQ(Xori[3], "4") << "should 4";
}

TEST(ParserTest, Labels) {
  auto ss = std::stringstream("    add     a5,a1,a2\n"
                              "    mv      a5,a0\n"
                              ".L3:\n"
                              "    bne     a5,a2,.L3\n");
  AsmParser AP(ss);
  // Expect two strings not to be equal.

  auto &Labels = AP.getLabels();
  EXPECT_NE(Labels.find(".L3"), Labels.end());
  EXPECT_EQ(AP.getLabels().find(".L3")->second, 0x08);
  EXPECT_TRUE(AP.parseLine());
  const auto &Add = AP.getTokens();
  EXPECT_EQ(Add[0], "add") << "should add";
  EXPECT_EQ(Add[1], "a5") << "should a5";
  EXPECT_EQ(Add[2], "a1") << "should a1";
  EXPECT_EQ(Add[3], "a2") << "should a2";
  EXPECT_TRUE(AP.parseLine());
  const auto &Mv = AP.getTokens();
  EXPECT_EQ(Mv[0], "mv") << "should mv";
  EXPECT_EQ(Mv[1], "a5") << "should a5";
  EXPECT_EQ(Mv[2], "a0") << "should a0";
  EXPECT_TRUE(AP.parseLine());
  const auto &Bne = AP.getTokens();
  EXPECT_EQ(Bne[0], "bne") << "should bne";
  EXPECT_EQ(Bne[1], "a5") << "should a5";
  EXPECT_EQ(Bne[2], "a2") << "should a2";
  EXPECT_EQ(Bne[3], ".L3") << "should .L3";
}

TEST(ParserTest, LoadStore) {
  auto ss = std::stringstream("lh x18, 4(x0)\n");
  AsmParser AP(ss);

  EXPECT_TRUE(AP.parseLine());
  const auto &LoadHalf = AP.getTokens();
  EXPECT_EQ(LoadHalf[0], "lh");
  EXPECT_EQ(LoadHalf[1], "x18");
  EXPECT_EQ(LoadHalf[2], "4(x0)");
}