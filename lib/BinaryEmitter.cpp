#include "BinaryEmitter.h"
#include "Instructions.h"
#include "Registers.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#ifdef DEBUG
#include "Debug.h"
#endif
std::pair<int, std::string> parseOffsetReg(const std::string &input) {
  std::string numStr, identifier;

  size_t i = 0;
  while (i < input.size() && (std::isdigit(input[i]) || input[i] == '-')) {
    numStr.push_back(input[i]);
    i++;
  }

  while (i < input.size() && (std::isalnum(input[i]) || input[i] == '_')) {
    identifier.push_back(input[i]);
    i++;
  }

  int num;
  try {
    num = std::stoi(numStr);
  } catch (const std::exception &e) {
    throw std::runtime_error("Failed to parse number from string.");
  }

  return {num, identifier};
}

std::optional<std::bitset<5>> findReg(const std::string &RegStr) {
  if (auto RI = GPRegs.find(RegStr); RI != GPRegs.end())
    return RI->second;
  assert(false && "invalid register name");
  return std::nullopt;
}

// TODO: move this to support and debug only

void BinaryEmitter::emitBinary(std::ostream &os) {
  while (AP.parseLine()) {
    auto &Toks = AP.getTokens();
    auto &Mnemo = Toks[0];
    unsigned Inst = 0;
    if (auto IT = ITypeKinds.find(Mnemo); IT != ITypeKinds.end()) {
      assert((Toks.size() == 4 || Toks.size() == 3) && "Wrong token number");

      IType ITemp = IT->second;
      std::bitset<3> Funct3 = ITemp.getFunct3();
      std::bitset<7> Opcode = ITemp.getOpcode();

      std::bitset<5> Rd, Rs1;
      std::bitset<12> Imm;
      Rd = *findReg(Toks[1]);

      // handle offset for loads
      if (Toks.size() == 3) {
        assert((Mnemo == "lb" || Mnemo == "lh" || Mnemo == "lw" ||
                Mnemo == "lbu" || Mnemo == "lhu") &&
               "invarid offset(reg) notation except loads");
        auto OffReg = parseOffsetReg(Toks[2]);
        Rs1 = std::bitset<5>(OffReg.second);
        Imm = OffReg.first;
      } else {
        Rs1 = *findReg(Toks[2]);
        Imm = stoi(Toks[3]);
      }

      // srai immediate
      if (Mnemo == "srai")
        Imm |= 1 << 10;

      // TODO: move this to constructor?
      Inst = (Imm.to_ulong() << 20) | (Rs1.to_ulong() << 15) |
             (Funct3.to_ulong() << 12) | (Rd.to_ulong() << 7) |
             Opcode.to_ulong();
    } else if (auto IT = RTypeKinds.find(Mnemo); IT != RTypeKinds.end()) {
      assert(Toks.size() == 4 && "Wrong token number");

      RType RTemp = IT->second;
      std::bitset<7> Funct7 = RTemp.getFunct7();
      std::bitset<3> Funct3 = RTemp.getFunct3();
      std::bitset<7> Opcode = RTemp.getOpcode();

      std::bitset<5> Rd, Rs1, Rs2;

      Rd = *findReg(Toks[1]);
      Rs1 = *findReg(Toks[2]);
      Rs2 = *findReg(Toks[3]);

      // TODO: move this to constructor?
      Inst = (Funct7.to_ulong() << 25) | (Rs2.to_ulong() << 20) |
             (Rs1.to_ulong() << 15) | (Funct3.to_ulong() << 12) |
             (Rd.to_ulong() << 7) | Opcode.to_ulong();

    } else {
      std::cerr << Mnemo << "\n";
      assert(false && "unimplemented!");
    }

// TODO: create DEBUGEXPR MACRO to shorten this
#ifdef DEBUG
    debugInst(Toks, Inst);
#endif
    os.write(reinterpret_cast<char *>(&Inst), 4);
  }
}