#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <bitset>
#include <map>
class IType {
private:
  // FIXME: mnemonic isn't necessary?
  const std::string mnemonic;
  const std::bitset<3> funct3;
  const std::bitset<7> opcode;

public:
  IType(const std::string &mnemonic, const unsigned funct3,
        const unsigned opcode)
      : mnemonic(mnemonic), funct3(funct3), opcode(opcode) {}
  const std::string &getMnemonic() const { return mnemonic; }
  const std::bitset<3> &getFunct3() const { return funct3; }
  const std::bitset<7> &getOpcode() const { return opcode; }
};

const std::map<std::string, IType> ITypeKinds = {
    {"addi", {"addi", 0b000, 0b0010011}},
    {"slti", {"slti", 0b010, 0b0010011}},
    {"sltiu", {"sltiu", 0b011, 0b0010011}},
    {"xori", {"xori", 0b100, 0b0010011}},
    {"ori", {"ori", 0b110, 0b0010011}},
    {"andi", {"andi", 0b111, 0b0010011}},
    {"jalr", {"jalr", 0b000, 0b1100111}},
    {"lb", {"lb", 0b000, 0b0000011}},
    {"lh", {"lh", 0b001, 0b0000011}},
    {"lw", {"lw", 0b010, 0b0000011}},
    {"lbu", {"lbu", 0b100, 0b0000011}},
    {"lhu", {"lhu", 0b101, 0b0000011}},
    {"slli", {"slli", 0b001, 0b0010011}},
    {"srli", {"srli", 0b101, 0b0010011}},
    {"srai", {"srai", 0b101, 0b0010011}},
};

class RType {
private:
  const std::string mnemonic;
  const std::bitset<7> funct7;
  const std::bitset<3> funct3;
  const std::bitset<7> opcode;

public:
  RType(const std::string &mnemonic, const unsigned funct7,
        const unsigned funct3, const unsigned opcode)
      : mnemonic(mnemonic), funct7(funct7), funct3(funct3), opcode(opcode) {}
  const std::string &getMnemonic() const { return mnemonic; }
  const std::bitset<7> &getFunct7() const { return funct7; }
  const std::bitset<3> &getFunct3() const { return funct3; }
  const std::bitset<7> &getOpcode() const { return opcode; }
};
const std::map<std::string, RType> RTypeKinds = {
    {"add", {"add", 0b0000000, 0b000, 0b0110011}},
    {"sub", {"sub", 0b0100000, 0b000, 0b0110011}},
    {"sll", {"sll", 0b0000000, 0b001, 0b0110011}},
    {"slt", {"slt", 0b0000000, 0b010, 0b0110011}},
    {"sltu", {"sltu", 0b0000000, 0b011, 0b0110011}},
    {"xor", {"xor", 0b0000000, 0b100, 0b0110011}},
    {"srl", {"srl", 0b0000000, 0b101, 0b0110011}},
    {"sra", {"sra", 0b0100000, 0b101, 0b0110011}},
    {"or", {"or", 0b0000000, 0b110, 0b0110011}},
    {"and", {"and", 0b0000000, 0b111, 0b0110011}},
};
#endif