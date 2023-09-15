
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

#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

class Simulator {
private:
  CPU C;
  Memory M;
  unsigned CodeSize;
  Address PC;
  // FIXME: byref?
  GPRegisters GPRegs;
  CustomRegisters CRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  Simulator(const Simulator &) = delete;
  Simulator &operator=(const Simulator &) = delete;

  Simulator(std::istream &is) : PC(DRAM_BASE) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    // starts from DRAM_BASE
    Address P = DRAM_BASE;
    while (is.read(Buff, 4)) {
      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // Raw inst
      M.writeWord(P, InstVal);
      // TODO: separate this as Decoder
      unsigned Opcode = InstVal & 0x0000007f;
      unsigned Rd = (InstVal & 0x00000f80) >> 7;
      unsigned Rs1 = (InstVal & 0x000f8000) >> 15;
      unsigned Rs2 = (InstVal & 0x01f00000) >> 20;
      unsigned Funct3 = (InstVal & 0x00007000) >> 12;
      unsigned Funct7 = (InstVal & 0xfe000000) >> 25;

      // FIXME: enough to set just mnemonic? & mask for srai would be another
      // time
      // FIXME: nested switch to if.
      switch (Opcode) {
      case 0b0000011:
        switch (unsigned Imm = InstVal >> 20; Funct3) {
        // TODO: other load insts
        case 0b010: // lw rd ,offset(rs1)
          PCInstMap.insert(
              {P, std::make_unique<IInstruction>(ITypeKinds.find("lw")->second,
                                                 Rd, Rs1, Imm)});
          break;
        default:
          assert(false && "unimplemented!");
          break;
        }
        break;
      case 0b0010111: // auipc
        PCInstMap.insert(
            {P, std::make_unique<UInstruction>(UTypeKinds.find("auipc")->second,
                                               Rd, InstVal & 0xfffff000)});
        break;
      case 0b0110011:
        if (Funct3 == 0b000 && Funct7 == 0b0000000) { // add
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("add")->second,
                                                 Rd, Rs1, Rs2)});
        } else if (Funct3 == 0b000 && Funct7 == 0b0100000) { // sub
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("sub")->second,
                                                 Rd, Rs1, Rs2)});

        } else if (Funct3 == 0b001 && Funct7 == 0b0000000) { // sll
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("sll")->second,
                                                 Rd, Rs1, Rs2)});

        } else if (Funct3 == 0b010 && Funct7 == 0b0000000) { // slt
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("slt")->second,
                                                 Rd, Rs1, Rs2)});

        } else if (Funct3 == 0b011 && Funct7 == 0b0000000) { // sltu

          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(
                      RTypeKinds.find("sltu")->second, Rd, Rs1, Rs2)});
        } else if (Funct3 == 0b101 && Funct7 == 0b0000000) { // srl
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("srl")->second,
                                                 Rd, Rs1, Rs2)});

        } else if (Funct3 == 0b101 && Funct7 == 0b0100000) { // sra
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("sra")->second,
                                                 Rd, Rs1, Rs2)});

        } else if (Funct3 == 0b110 && Funct7 == 0b0000000) { // or
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("or")->second,
                                                 Rd, Rs1, Rs2)});
        } else if (Funct3 == 0b111 && Funct7 == 0b0000000) { // and
          PCInstMap.insert(
              {P, std::make_unique<RInstruction>(RTypeKinds.find("and")->second,
                                                 Rd, Rs1, Rs2)});
        } else
          assert(false && "unimplemented!");

        break;
      case 0b0010011:
        switch (unsigned Imm = InstVal >> 20; Funct3) {
        case 0b000: // addi rd, rs1, imm
          PCInstMap.insert(
              {P, std::make_unique<IInstruction>(
                      ITypeKinds.find("addi")->second, Rd, Rs1, Imm)});
          break;
        case 0b010: // slti rd, rs1, imm
          PCInstMap.insert(
              {P, std::make_unique<IInstruction>(
                      ITypeKinds.find("slti")->second, Rd, Rs1, Imm)});
          break;
        case 0b011: // sltiu rd, rs1, imm
          PCInstMap.insert(
              {P, std::make_unique<IInstruction>(
                      ITypeKinds.find("sltiu")->second, Rd, Rs1, Imm)});
          break;
        case 0b100: // xori
          PCInstMap.insert(
              {P, std::make_unique<IInstruction>(
                      ITypeKinds.find("xori")->second, Rd, Rs1, Imm)});
          break;
        case 0b110: // ori
          assert(false && "unimplemented!");
          break;
        case 0b111: // andi
          assert(false && "unimplemented!");
          break;
        case 0b001: // slli
          assert(false && "unimplemented!");
          break;
        case 0b101:
          if (Funct7 == 0) // srli
                           // this will be 6 bit for RV64I
            // Is.push_back(std::make_unique<IInstruction>(
            //     ITypeKinds.find("srli")->second, Rd, Rs1,
            //     (InstVal >> 20) & 0b11111));
            assert(false && "unimplemented!");
          else // srai
            // Is.push_back(std::make_unique<IInstruction>(
            //     ITypeKinds.find("srai")->second, Rd, Rs1,
            //     (InstVal >> 20) & 0b11111));
            assert(false && "unimplemented!");
          break;
        default:
          assert(false && "unimplemented!");
          break;
        }
        break;
      case 0b1100111: // jalr
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("jalr")->second,
                                               Rd, Rs1, InstVal >> 20)});
        break;
      case 0b0100011:
        // offset[11:5|4:0] = inst[31:25|11:7]
        switch (unsigned Offset =
                    (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
                Funct3) {
        case 0b010: // sw
          PCInstMap.insert(
              {P, std::make_unique<SInstruction>(STypeKinds.find("sw")->second,
                                                 Rs1, Rs2, Offset)});
          break;
        default:
          assert(false && "unimplemented!");
          break;
        }
        break;
      case 0b1100011:
        // imm[12|11|10:5|4:1] = inst[31|7|30:25|11:8]
        switch (unsigned Imm =
                    (InstVal & 0x80000000) >> 19 | ((InstVal & 0x80) << 4) |
                    ((InstVal >> 20) & 0x7e0) | ((InstVal >> 7) & 0x1e);
                Funct3) {
        case 0b000: // beq
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(BTypeKinds.find("beq")->second,
                                                 Rs1, Rs2, Imm)});
          break;
        case 0b001: // bne
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(BTypeKinds.find("bne")->second,
                                                 Rs1, Rs2, Imm)});
          break;
        case 0b100: // blt
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(BTypeKinds.find("blt")->second,
                                                 Rs1, Rs2, Imm)});
          break;
        case 0b101: // bge
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(BTypeKinds.find("bge")->second,
                                                 Rs1, Rs2, Imm)});
          break;
        case 0b110: // bltu
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(
                      BTypeKinds.find("bltu")->second, Rs1, Rs2, Imm)});
          break;
        case 0b111: // bgeu
          PCInstMap.insert(
              {P, std::make_unique<BInstruction>(
                      BTypeKinds.find("bgeu")->second, Rs1, Rs2, Imm)});
          break;
        }
        break;
      case 0b1101111: // jal
        // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
        PCInstMap.insert(
            {P, std::make_unique<JInstruction>(
                    JTypeKinds.find("jal")->second, Rd,
                    ((InstVal & 0x80000000) >> 11) | (InstVal & 0xff000) |
                        ((InstVal >> 9) & 0x800) | ((InstVal >> 20) & 0x7fe))});
        break;
      default:
#ifdef DEBUG
        dumpInstVal(InstVal);
#endif
        assert(false && "unimplemented!");
        break;
      }
      P += 4;
      CodeSize += 4;
    }
  }
  GPRegisters &getGPRegs() { return GPRegs; }

  void execFromDRAMBASE() {
    PC = DRAM_BASE;
    while (auto &I = PCInstMap[PC]) {
      I->exec(PC, GPRegs, M, CRegs);
#ifdef DEBUG
      std::cerr << "Inst:\n";
      I->pprint(std::cerr);
      std::cerr << "Regs after:\n";
      dumpGPRegs();
#endif
      // TODO: dump instruction detail.
    }
    std::cerr << "stop on no instraction address\n";
  }

  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
