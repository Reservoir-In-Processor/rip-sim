#include "Decoder.h"

std::unique_ptr<Instruction> Decoder::decode(unsigned InstVal) {
  // Raw inst
  unsigned Opcode = InstVal & 0x0000007f;
  unsigned Rd = (InstVal & 0x00000f80) >> 7;
  unsigned Rs1 = (InstVal & 0x000f8000) >> 15;
  unsigned Rs2 = (InstVal & 0x01f00000) >> 20;
  unsigned Funct3 = (InstVal & 0x00007000) >> 12;
  unsigned Funct7 = (InstVal & 0xfe000000) >> 25;

  std::unique_ptr<Instruction> InstPtr;
  switch (Opcode) {
  case 0b0000011:
    if (Funct3 == 0b000) { // lb
      unsigned Imm = InstVal >> 20;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("lb")->second,
                                               Rd, Rs1, Imm);
    } else if (Funct3 == 0b001) { // lh
      unsigned Imm = InstVal >> 20;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("lh")->second,
                                               Rd, Rs1, Imm);
    } else if (Funct3 == 0b010) { // lw
      unsigned Imm = InstVal >> 20;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("lw")->second,
                                               Rd, Rs1, Imm);
    } else if (Funct3 == 0b100) { // lbu
      unsigned Imm = InstVal >> 20;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("lbu")->second,
                                               Rd, Rs1, Imm);
    } else if (Funct3 == 0b101) { // lhu
      unsigned Imm = InstVal >> 20;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("lhu")->second,
                                               Rd, Rs1, Imm);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }
    break;

  case 0b0001011:
    if (Rs2 == 0) {
      // extended instruction. nop on simulator
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("extx")->second,
                                               0, 0, 0);
    } else if (Rs2 == 1) {
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("ext")->second,
                                               0, 0, 0);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }
    break;

  case 0b0001111:
    if (Funct3 == 0b000) { // fence
                           // FIXME: Currently mapped to addi x0, x0, 0 (nop)
      unsigned Imm = 0;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm);
    } else if (Funct3 == 0b001) { // fence.i
      // FIXME: Currently mapped to addi x0, x0, 0 (nop)
      unsigned Imm = 0;
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }
    break;

  case 0b0010111: // auipc
    InstPtr = std::make_unique<UInstruction>(UTypeKinds.find("auipc")->second,
                                             Rd, (InstVal & 0xfffff000) >> 12);
    break;
  case 0b0110011:
    if (Funct3 == 0b000 && Funct7 == 0b0000000) { // add
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("add")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b000 && Funct7 == 0b0100000) {
      // sub
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("sub")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b001 && Funct7 == 0b0000000) { // sll
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("sll")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b010 && Funct7 == 0b0000000) { // slt
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("slt")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b011 && Funct7 == 0b0000000) { // sltu

      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("sltu")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b101 && Funct7 == 0b0000000) { // srl
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("srl")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b101 && Funct7 == 0b0100000) { // sra
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("sra")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b110 && Funct7 == 0b0000000) { // or
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("or")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b111 && Funct7 == 0b0000000) { // and
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("and")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b000 && Funct7 == 0b0000001) { // mul
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("mul")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b001 && Funct7 == 0b0000001) { // mulh
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("mulh")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b010 && Funct7 == 0b0000001) { // mulhsu
      InstPtr = std::make_unique<RInstruction>(
          RTypeKinds.find("mulhsu")->second, Rd, Rs1, Rs2);
    } else if (Funct3 == 0b011 && Funct7 == 0b0000001) { // mulhu
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("mulhu")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b100 && Funct7 == 0b0000000) { // xor
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("xor")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b100 && Funct7 == 0b0000001) { // div
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("div")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b101 && Funct7 == 0b0000001) { // divu
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("divu")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b110 && Funct7 == 0b0000001) { // rem
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("rem")->second,
                                               Rd, Rs1, Rs2);
    } else if (Funct3 == 0b111 && Funct7 == 0b0000001) {
      // remu
      InstPtr = std::make_unique<RInstruction>(RTypeKinds.find("remu")->second,
                                               Rd, Rs1, Rs2);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }

    break;
  case 0b0010011:
    switch (unsigned Imm = InstVal >> 20; Funct3) {
    case 0b000: // addi rd, rs1, imm
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b010: // slti rd, rs1, imm
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("slti")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b011: // sltiu rd, rs1, imm
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("sltiu")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b100: // xori
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("xori")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b110: // ori
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("ori")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b111: // andi
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("andi")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b001: // slli
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("slli")->second,
                                               Rd, Rs1, Imm);
      break;
    case 0b101:
      if (Funct7 == 0) { // srli
                         // this will be 6 bit for RV64I
        InstPtr =
            std::make_unique<IInstruction>(ITypeKinds.find("srli")->second, Rd,
                                           Rs1, (InstVal >> 20) & 0b11111);
      } else { // srai
        InstPtr =
            std::make_unique<IInstruction>(ITypeKinds.find("srai")->second, Rd,
                                           Rs1, (InstVal >> 20) & 0b11111);
      }
      break;
    default:
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
      break;
    }
    break;
  case 0b0110111: // lui
    InstPtr = std::make_unique<UInstruction>(UTypeKinds.find("lui")->second, Rd,
                                             (InstVal & 0xfffff000) >> 12);
    break;
  case 0b1100111: // jalr
    InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("jalr")->second,
                                             Rd, Rs1, InstVal >> 20);
    break;
  case 0b0100011:
    if (Funct3 == 0b000) { // sb
                           // offset[11:5|4:0] = inst[31:25|11:7]
      unsigned Offset = (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
      InstPtr = std::make_unique<SInstruction>(STypeKinds.find("sb")->second,
                                               Rs1, Rs2, Offset);
    } else if (Funct3 == 0b001) { // sh
                                  // offset[11:5|4:0] = inst[31:25|11:7]
      unsigned Offset = (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
      InstPtr = std::make_unique<SInstruction>(STypeKinds.find("sh")->second,
                                               Rs1, Rs2, Offset);
    } else if (Funct3 == 0b010) { // sw
                                  // offset[11:5|4:0] = inst[31:25|11:7]
      unsigned Offset = (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
      InstPtr = std::make_unique<SInstruction>(STypeKinds.find("sw")->second,
                                               Rs1, Rs2, Offset);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }
    break;
  case 0b1100011:
    // imm[12|11|10:5|4:1] = inst[31|7|30:25|11:8]
    switch (unsigned Imm = (InstVal & 0x80000000) >> 19 |
                           ((InstVal & 0x80) << 4) | ((InstVal >> 20) & 0x7e0) |
                           ((InstVal >> 7) & 0x1e);
            Funct3) {
    case 0b000: // beq
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("beq")->second,
                                               Rs1, Rs2, Imm);
      break;
    case 0b001: // bne
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("bne")->second,
                                               Rs1, Rs2, Imm);
      break;
    case 0b100: // blt
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("blt")->second,
                                               Rs1, Rs2, Imm);
      break;
    case 0b101: // bge
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("bge")->second,
                                               Rs1, Rs2, Imm);
      break;
    case 0b110: // bltu
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("bltu")->second,
                                               Rs1, Rs2, Imm);
      break;
    case 0b111: // bgeu
      InstPtr = std::make_unique<BInstruction>(BTypeKinds.find("bgeu")->second,
                                               Rs1, Rs2, Imm);
      break;
    }
    break;
  case 0b1101111: // jal
                  // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
    InstPtr = std::make_unique<JInstruction>(
        JTypeKinds.find("jal")->second, Rd,
        ((InstVal & 0x80000000) >> 11) | (InstVal & 0xff000) |
            ((InstVal >> 9) & 0x800) | ((InstVal >> 20) & 0x7fe));
    break;
  case 0b1110011:          // csr
    if (Funct3 == 0b000) { // ecall, ebreak, mret, sret, etc...
      unsigned Funct12 = Funct7 << 5 | Rs2;
      if (Funct12 == 0) { // ecall
        InstPtr = std::make_unique<IInstruction>(
            ITypeKinds.find("ecall")->second, Rd, Rs1, InstVal >> 20);
      } else if (Funct12 == 1) { // ebreak
        InstPtr = std::make_unique<IInstruction>(
            ITypeKinds.find("ebreak")->second, Rd, Rs1, InstVal >> 20);
      } else if (Funct12 == 2) { // uret
        InstPtr = std::make_unique<IInstruction>(
            ITypeKinds.find("uret")->second, Rd, Rs1, InstVal >> 20);
      } else if (Funct12 == 0b100000010) { // sret
        InstPtr = std::make_unique<IInstruction>(
            ITypeKinds.find("sret")->second, Rd, Rs1, InstVal >> 20);
      } else if (Funct12 == 0b1100000010) { // mret
        InstPtr = std::make_unique<IInstruction>(
            ITypeKinds.find("mret")->second, Rd, Rs1, InstVal >> 20);
      } else {
        DEBUG_ONLY(dumpInstVal(InstVal));
        return nullptr;
      }
      break;
    } else if (Funct3 == 0b001) { // csrrw
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("csrrw")->second,
                                               Rd, Rs1, InstVal >> 20);
    } else if (Funct3 == 0b010) { // csrrs
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("csrrs")->second,
                                               Rd, Rs1, InstVal >> 20);
    } else if (Funct3 == 0b011) { // csrrc
      InstPtr = std::make_unique<IInstruction>(ITypeKinds.find("csrrc")->second,
                                               Rd, Rs1, InstVal >> 20);
    } else if (Funct3 == 0b101) { // csrrwi
      InstPtr = std::make_unique<IInstruction>(
          ITypeKinds.find("csrrwi")->second, Rd, Rs1, InstVal >> 20);
    } else if (Funct3 == 0b110) { // csrrsi
      InstPtr = std::make_unique<IInstruction>(
          ITypeKinds.find("csrrsi")->second, Rd, Rs1, InstVal >> 20);
    } else if (Funct3 == 0b111) { // csrrci
      InstPtr = std::make_unique<IInstruction>(
          ITypeKinds.find("csrrci")->second, Rd, Rs1, InstVal >> 20);
    } else {
      DEBUG_ONLY(dumpInstVal(InstVal));
      return nullptr;
    }
    break;
  default:
    DEBUG_ONLY(dumpInstVal(InstVal));
    return nullptr;
    break;
  }
  return InstPtr;
};
