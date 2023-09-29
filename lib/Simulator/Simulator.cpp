
#include "Simulator/Simulator.h"
#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

Simulator::Simulator(std::istream &is) : PC(DRAM_BASE) {
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
      if (Funct3 == 0b000) { // lb
        unsigned Imm = InstVal >> 20;
        PCInstMap.insert({P, std::make_unique<IInstruction>(
                                 ITypeKinds.find("lb")->second, Rd, Rs1, Imm)});
      } else if (Funct3 == 0b001) { // lh
        unsigned Imm = InstVal >> 20;
        PCInstMap.insert({P, std::make_unique<IInstruction>(
                                 ITypeKinds.find("lh")->second, Rd, Rs1, Imm)});
      } else if (Funct3 == 0b010) { // lw
        unsigned Imm = InstVal >> 20;
        PCInstMap.insert({P, std::make_unique<IInstruction>(
                                 ITypeKinds.find("lw")->second, Rd, Rs1, Imm)});
      } else if (Funct3 == 0b100) { // lbu
        unsigned Imm = InstVal >> 20;
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("lbu")->second,
                                               Rd, Rs1, Imm)});
      } else if (Funct3 == 0b101) { // lhu
        unsigned Imm = InstVal >> 20;
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("lhu")->second,
                                               Rd, Rs1, Imm)});
      } else
        assert(false && "unimplemented!");
      break;

    case 0b0001111:
      if (Funct3 == 0b000) { // fence
                             // FIXME: Currently mapped to addi x0, x0, 0 (nop)
        unsigned Imm = 0;
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm)});

      } else if (Funct3 == 0b001) { // fence.i
        // FIXME: Currently mapped to addi x0, x0, 0 (nop)
        unsigned Imm = 0;
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm)});

      } else
        assert(false && "unimplemented!");
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
            {P, std::make_unique<RInstruction>(RTypeKinds.find("sltu")->second,
                                               Rd, Rs1, Rs2)});
      } else if (Funct3 == 0b101 && Funct7 == 0b0000000) { // srl
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("srl")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b101 && Funct7 == 0b0100000) { // sra
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("sra")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b110 && Funct7 == 0b0000000) { // or
        PCInstMap.insert({P, std::make_unique<RInstruction>(
                                 RTypeKinds.find("or")->second, Rd, Rs1, Rs2)});
      } else if (Funct3 == 0b111 && Funct7 == 0b0000000) { // and
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("and")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b000 && Funct7 == 0b0000001) { // mul
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("mul")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b001 && Funct7 == 0b0000001) { // mulh
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("mulh")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b010 && Funct7 == 0b0000001) { // mulhsu
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(
                    RTypeKinds.find("mulhsu")->second, Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b011 && Funct7 == 0b0000001) { // mulhu
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("mulhu")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b100 && Funct7 == 0b0000001) { // div
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("div")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b101 && Funct7 == 0b0000001) { // divu
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("divu")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b110 && Funct7 == 0b0000001) { // rem
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("rem")->second,
                                               Rd, Rs1, Rs2)});

      } else if (Funct3 == 0b111 && Funct7 == 0b0000001) { // remu
        PCInstMap.insert(
            {P, std::make_unique<RInstruction>(RTypeKinds.find("remu")->second,
                                               Rd, Rs1, Rs2)});

      } else
        assert(false && "unimplemented!");

      break;
    case 0b0010011:
      switch (unsigned Imm = InstVal >> 20; Funct3) {
      case 0b000: // addi rd, rs1, imm
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("addi")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b010: // slti rd, rs1, imm
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("slti")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b011: // sltiu rd, rs1, imm
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("sltiu")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b100: // xori
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("xori")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b110: // ori
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("ori")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b111: // andi
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("andi")->second,
                                               Rd, Rs1, Imm)});
        break;
      case 0b001: // slli
        PCInstMap.insert(
            {P, std::make_unique<IInstruction>(ITypeKinds.find("slli")->second,
                                               Rd, Rs1, Imm)});
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
#ifdef DEBUG
        dumpInstVal(InstVal);
#endif
        assert(false && "unimplemented!");
        break;
      }
      break;
    case 0b0110111: // lui
      PCInstMap.insert(
          {P, std::make_unique<UInstruction>(UTypeKinds.find("lui")->second, Rd,
                                             InstVal & 0xfffff000)});
      break;

    case 0b1100111: // jalr
      PCInstMap.insert(
          {P, std::make_unique<IInstruction>(ITypeKinds.find("jalr")->second,
                                             Rd, Rs1, InstVal >> 20)});
      break;
    case 0b0100011:
      if (Funct3 == 0b000) { // sb
                             // offset[11:5|4:0] = inst[31:25|11:7]
        unsigned Offset =
            (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
        PCInstMap.insert(
            {P, std::make_unique<SInstruction>(STypeKinds.find("sb")->second,
                                               Rs1, Rs2, Offset)});
      } else if (Funct3 == 0b001) { // sh
                                    // offset[11:5|4:0] = inst[31:25|11:7]
        unsigned Offset =
            (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
        PCInstMap.insert(
            {P, std::make_unique<SInstruction>(STypeKinds.find("sh")->second,
                                               Rs1, Rs2, Offset)});
      } else if (Funct3 == 0b010) { // sw
                                    // offset[11:5|4:0] = inst[31:25|11:7]
        unsigned Offset =
            (InstVal & 0xfe000000) >> 20 | ((InstVal >> 7) & 0x1f);
        PCInstMap.insert(
            {P, std::make_unique<SInstruction>(STypeKinds.find("sw")->second,
                                               Rs1, Rs2, Offset)});
      } else {
#ifdef DEBUG
        dumpInstVal(InstVal);
#endif
        assert(false && "unimplemented!");
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
            {P, std::make_unique<BInstruction>(BTypeKinds.find("bltu")->second,
                                               Rs1, Rs2, Imm)});
        break;
      case 0b111: // bgeu
        PCInstMap.insert(
            {P, std::make_unique<BInstruction>(BTypeKinds.find("bgeu")->second,
                                               Rs1, Rs2, Imm)});
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