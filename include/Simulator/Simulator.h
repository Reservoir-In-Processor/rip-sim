
#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "CPU.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include <memory>
#include <string>
#include <vector>

class Simulator {
private:
  CPU C;
  Memory M;
  GPRegisters GPRegs;
  CustomRegisters CRegs;
  std::vector<std::unique_ptr<Instruction>> Is;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  Simulator(std::istream &is) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    while (is.read(Buff, 4)) {
      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // TODO: separate this as Decoder
      unsigned Opcode = InstVal & 0x0000007f;
      unsigned Rd = (InstVal & 0x00000f80) >> 7;
      unsigned Rs1 = (InstVal & 0x000f8000) >> 15;
      unsigned Rs2 = (InstVal & 0x01f00000) >> 20;
      unsigned Funct3 = (InstVal & 0x00007000) >> 12;
      unsigned Funct7 = (InstVal & 0xfe000000) >> 25;

      switch (Opcode) {
      case 0b0010011:
        switch (Funct3) {
        case 0b000: // addi rd, rs1, imm
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("addi")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b010: // slti rd, rs1, imm
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("slti")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b011: // sltiu rd, rs1, imm
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("sltiu")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b100: // xori
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("xori")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b110: // ori
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("ori")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b111: // andi
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("andi")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b001: // slli
          Is.push_back(std::make_unique<IInstruction>(
              ITypeKinds.find("slli")->second, Rd, Rs1, InstVal >> 20));
          break;
        case 0b101:
          if (Funct7 == 0) // srli
                           // this will be 6 bit for RV64I
            Is.push_back(std::make_unique<IInstruction>(
                ITypeKinds.find("srli")->second, Rd, Rs1,
                (InstVal >> 20) & 0b11111));
          else // srai
            Is.push_back(std::make_unique<IInstruction>(
                ITypeKinds.find("srai")->second, Rd, Rs1,
                (InstVal >> 20) & 0b11111));
          break;
        default:
          assert(false && "unimplemented!");
          break;
        }
        break;
      case 0b1100111:
        assert(false && "unimplemented!");
        break;
      case 0b0000011:
        assert(false && "unimplemented!");
        break;
      default:
        assert(false && "unimplemented!");
        break;
      }
    }
    std::cerr << '\n';
  }
  GPRegisters &getGPRegs() { return GPRegs; }

  void exec() {
    unsigned PC = 0;
    for (const auto &I : Is) {
      I->exec(PC, GPRegs, M, CRegs);
    }
  }

  void dumpGPRegs() { GPRegs.dump(); }
};

#endif
