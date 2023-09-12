#ifndef INSTRUCTION_TYPES_H
#define INSTRUCTION_TYPES_H

#include <bitset>
#include <map>

// classes to assemble
class ISBType {
private:
  // FIXME: mnemonic isn't necessary?
  const std::string mnemonic;
  const std::bitset<3> funct3;
  const std::bitset<7> opcode;

public:
  ISBType(const std::string &mnemonic, const unsigned funct3,
          const unsigned opcode)
      : mnemonic(mnemonic), funct3(funct3), opcode(opcode) {}
  const std::string &getMnemo() const { return mnemonic; }
  const std::bitset<3> &getFunct3() const { return funct3; }
  const std::bitset<7> &getOpcode() const { return opcode; }
};

const std::map<const std::string, const ISBType> ITypeKinds = {
    {"addi",  {"addi",  0b000, 0b0010011}},
    {"slti",  {"slti",  0b010, 0b0010011}},
    {"sltiu", {"sltiu", 0b011, 0b0010011}},
    {"xori",  {"xori",  0b100, 0b0010011}},
    {"ori",   {"ori",   0b110, 0b0010011}},
    {"andi",  {"andi",  0b111, 0b0010011}},
    {"jalr",  {"jalr",  0b000, 0b1100111}},
    {"lb",    {"lb",    0b000, 0b0000011}},
    {"lh",    {"lh",    0b001, 0b0000011}},
    {"lw",    {"lw",    0b010, 0b0000011}},
    {"lbu",   {"lbu",   0b100, 0b0000011}},
    {"lhu",   {"lhu",   0b101, 0b0000011}},
    {"slli",  {"slli",  0b001, 0b0010011}},
    {"srli",  {"srli",  0b101, 0b0010011}},
    {"srai",  {"srai",  0b101, 0b0010011}},
};

const std::map<std::string, ISBType> STypeKinds = {
    {"sb", {"sb", 0b000, 0b0100011}},
    {"sh", {"sh", 0b001, 0b0100011}},
    {"sw", {"sw", 0b010, 0b0100011}},
};

const std::map<std::string, ISBType> BTypeKinds = {
    {"beq", {"beq", 0b000, 0b1100011}}, // Branch EQual
    {"bne", {"bne", 0b001, 0b1100011}},
    {"blt", {"blt", 0b100, 0b1100011}}, // Branch Less Than
    {"bge", {"bge", 0b101, 0b1100011}},
    {"bltu", {"bltu", 0b110, 0b1100011}},
    {"bgeu", {"bgeu", 0b111, 0b1100011}},
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
  const std::string &getMnemo() const { return mnemonic; }
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

class UJType {
private:
  const std::string mnemonic;
  const std::bitset<7> opcode;

public:
  UJType(const std::string &mnemonic, const unsigned opcode)
      : mnemonic(mnemonic), opcode(opcode) {}
  const std::string &getMnemo() const { return mnemonic; }
  const std::bitset<7> &getOpcode() const { return opcode; }
};

const std::map<std::string, UJType> UTypeKinds = {
    {"lui", {"lui", 0b0110111}},    // Load Upper Immediate
    {"auipc", {"auipc", 0b0010111}} // Add Upper Immediate to PC
};

const std::map<std::string, UJType> JTypeKinds = {
    {"jal", {"jal", 0b1101111}} // Jump And Link
};

#endif
