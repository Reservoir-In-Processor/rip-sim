#include "RIPSimulator/RIPSimulator.h"
#include <cmath>
#include <set>

namespace {
// BitWidth < 32
int signExtend(const unsigned Imm, unsigned BitWidth) {
  if ((Imm >> (BitWidth - 1)) & 1) {
    return (~0 ^ ((int)(pow(2, BitWidth) - 1))) | (signed)Imm;
  } else {
    return Imm;
  }
}

} // namespace

void PipelineStates::dump() {
  // TODO: dump stall,
  for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
    std::cerr << StageNames[(STAGES)Stage] << ": ";
    if (Insts[Stage] != nullptr) {
      std::cerr << std::hex << "PC=0x" << PCs[Stage] << " ";
      Insts[Stage]->mprint(std::cerr);
      std::cerr << ", ";
    } else
      std::cerr << "Bubble, ";

    // TODO: dump stage specific info.
    switch (Stage) {
    case STAGES::DE:
      std::cerr << "DERs1Val=" << DERs1Val << ", ";
      std::cerr << "DERs2Val=" << DERs2Val << ", ";
      std::cerr << "DEImmVal=" << DEImmVal << "\n";
      break;
    case STAGES::EX:
      std::cerr << "EXRdVal=" << EXRdVal << "\n";
      break;
    case STAGES::MA:
      std::cerr << "MARdVal=" << MARdVal << "\n";
      break;
    default:
      std::cerr << "\n";
      break;
    }
  }
}

RIPSimulator::RIPSimulator(std::istream &is) : PC(DRAM_BASE), CycleNum(0) {
  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = DRAM_BASE;
  // Load binary into memory
  while (is.read(Buff, 4)) {
    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    Mem.writeWord(P, InstVal);
    P += 4;
    CodeSize += 4;
  }
}

void RIPSimulator::writeback(GPRegisters &, PipelineStates &) {
  const auto &Inst = PS[STAGES::WB];
  std::string Mnemo = Inst->getMnemo();
  RegVal Res = 0;

  if (Mnemo == "sw" || Mnemo == "sh" || Mnemo == "sb") {
    // Instructions without writeback
  } else if (Mnemo == "jalr") {
    Res = PS.getPCs(MA);
    GPRegs.write(Inst->getRd(), PS.getPCs(MA));
  } else if (Mnemo == "csrrw") {
    // FIXME: is it correct calculation in this stage?
    CSRVal CV = PS.getMARdVal();   // CV = CSR[Imm]
    RegVal CSRAddr = PS.getMAImmVal();
    Res = GPRegs[Inst->getRs1()];

    States.write(CSRAddr,
                 GPRegs[Inst->getRs1()]); // CSR[CSRAddr] = GPReg[rs1]
    GPRegs.write(Inst->getRd(), CV);      // Reg[Rd] = CSR[Imm]

  } else if (Mnemo == "csrrs") {
    // FIXME: is it correct calculation in this stage?
    CSRVal CV = PS.getMARdVal(); // CV = CSR[Imm]
    Res = CV | GPRegs[Inst->getRs1()];

    States.write(
        PS.getMARdVal(),
        CV | GPRegs[Inst->getRs1()]); // CSR[Imm] = CSR[Imm] | GPReg[rs1]
    GPRegs.write(Inst->getRd(), CV);

  } else if (Mnemo == "csrrc") {
    // FIXME: is it correct calculation in this stage?
    CSRVal CV = PS.getMARdVal(); // CV = CSR[Imm]
    Res = CV & !GPRegs[Inst->getRs1()];

    States.write(
        PS.getMARdVal(),
        CV & ~GPRegs[Inst->getRs1()]); // CSR[Imm] = CSR[Imm] & ~GPReg[rs1]
    GPRegs.write(Inst->getRd(), CV);

  } else {
    // Instructions with writeback
    Res = PS.getMARdVal();
    GPRegs.write(Inst->getRd(), PS.getMARdVal());
  }
  PS.setWBImmVal(Res);
}

void RIPSimulator::memoryaccess(Memory &, PipelineStates &) {
  const auto &Inst = PS[STAGES::MA];
  const RegVal MARdVal = PS.getEXRdVal();
  RegVal Res = MARdVal;
  unsigned Imm = PS.getEXImmVal();
  std::string Mnemo = Inst->getMnemo();

  if (Mnemo == "sw") {
    Mem.writeWord(MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "sh") {
    Mem.writeHalfWord(MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "sb") {
    Mem.writeByte(MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "lw") {
    Word V = Mem.readWord(PS.getEXRdVal());
    Res = (signed)V;
  } else if (Mnemo == "lh") {
    HalfWord V = Mem.readHalfWord(PS.getEXRdVal());
    Res = (signed short)V;
  } else if (Mnemo == "lbu") {
    Byte V = Mem.readByte(PS.getEXRdVal());
    Res = (unsigned char)V;
  } else if (Mnemo == "lhu") {
    HalfWord V = Mem.readHalfWord(PS.getEXRdVal());
    Res = (unsigned short)V;
  } else if (Mnemo == "lb") {
    Byte V = Mem.readByte(PS.getEXRdVal());
    Res = (signed char)V;
  } else if (Mnemo == "csrrs") {
    if (PS[STAGES::WB] && Imm == PS[STAGES::WB]->getImm()) {
      std::cerr << "Forwarding from WB to MA."
                << "\n";
      Res = PS.getWBImmVal();
    }
  }

  PS.setMARdVal(Res);
  PS.setMAImmVal(Imm);
}

const std::set<std::string> INVALID_EX = {"lbu",  "lhu",   "beq",   "blt",
                                          "bge",  "bltu",  "bgeu",  "jal",
                                          "jalr", "ecall", "ebreak"};

void RIPSimulator::exec(PipelineStates &) {
  const auto &Inst = PS[STAGES::EX];
  RegVal Res = 0;
  RegVal Imm = PS.getDEImmVal();

  std::string Mnemo = Inst->getMnemo();
  // I-type
  if (Mnemo == "addi") {
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "slti") {
    Res = (signed)PS.getDERs1Val() < PS.getDEImmVal();
  } else if (Mnemo == "sltiu") {
    Res = (unsigned)PS.getDERs1Val() < (unsigned)PS.getDEImmVal();
  } else if (Mnemo == "xori") {
    Res = (unsigned)PS.getDERs1Val() ^ PS.getDEImmVal();
  } else if (Mnemo == "ori") {
    Res = (unsigned)PS.getDERs1Val() | PS.getDEImmVal();
  } else if (Mnemo == "andi") {
    Res = (unsigned)PS.getDERs1Val() & PS.getDEImmVal();
  } else if (Mnemo == "jalr") {
    // FIXME: should addresss calculation be wrapped?
    Res = PS.getPCs(EX) + 4;

    Address nextPC = PS.getDERs1Val() + signExtend(PS.getDEImmVal(), 12);
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);

  } else if (Mnemo == "lb") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lh") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lw") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lbu") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lhu") {
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "slli") { // FIXME: shamt
    // FIXME: sext?
    Res = (unsigned)PS.getDERs1Val() << PS.getDEImmVal();
  } else if (Mnemo == "srli") {
    Res = (unsigned)PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "srai") {
    Res = (signed)PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "fence") {
    // FIXME: currently expected to be nop
  } else if (Mnemo == "fence.i") {
    // FIXME: currently expected to be nop
  } else if (Mnemo == "csrrw") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
  } else if (Mnemo == "csrrs") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
  } else if (Mnemo == "csrrc") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
  } else if (Mnemo == "csrrwi") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
  } else if (Mnemo == "csrrsi") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
  } else if (Mnemo == "csrrci") {
    Res = States.read(PS.getDEImmVal() & 0xfff); // Res = CSR[Imm]
    // } else if (Mnemo == "ecall") {
    //   if (Mode == ModeKind::User) {
    //     return Exception::EnvironmentCallFromUMode;
    //   } else if (Mode == ModeKind::Supervisor) {
    //     return Exception::EnvironmentCallFromSMode;
    //   } else if (Mode == ModeKind::Machine) {
    //     return Exception::EnvironmentCallFromMMode;
    //   } else {
    //     // FIXME: is this illegal inst?
    //     return Exception::IllegalInstruction;
    //   }
    // } else if (Mnemo == "ebreak") {
    //   return Exception::Breakpoint;

    // R-type
  } else if (Mnemo == "add") {
    Res = PS.getDERs1Val() + PS.getDERs2Val();
  } else if (Mnemo == "sub") {
    Res = PS.getDERs1Val() - PS.getDERs2Val();
  } else if (Mnemo == "sll") {
    Res = PS.getDERs1Val() << (PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "slt") {
    Res = PS.getDERs1Val() < PS.getDERs2Val();
  } else if (Mnemo == "sltu") {
    Res = (unsigned)PS.getDERs1Val() < (unsigned)PS.getDERs2Val();
  } else if (Mnemo == "xor") {
    Res = PS.getDERs1Val() ^ PS.getDERs2Val();
  } else if (Mnemo == "srl") {
    Res = (unsigned)PS.getDERs1Val() >> (PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "sra") {
    Res = (signed)PS.getDERs1Val() >> (signed)(PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "or") {
    Res = PS.getDERs1Val() | PS.getDERs2Val();
  } else if (Mnemo == "and") {
    Res = PS.getDERs1Val() & PS.getDERs2Val();
  } else if (Mnemo == "mul") {
    Res = ((signed long long)PS.getDERs1Val() *
           (signed long long)PS.getDERs2Val()) &
          0xffffffff;
  } else if (Mnemo == "mulh") {
    Res = ((signed long long)PS.getDERs1Val() *
           (signed long long)PS.getDERs2Val()) >>
          32;
  } else if (Mnemo == "mulhsu") {
    Res = ((signed long long)PS.getDERs1Val() *
           (unsigned long long)(unsigned int)PS.getDERs2Val()) >>
          32;
  } else if (Mnemo == "mulhu") {
    Res = ((unsigned long long)(unsigned int)PS.getDERs1Val() *
           (unsigned long long)(unsigned int)PS.getDERs2Val()) >>
          32;
  } else if (Mnemo == "div") {
    // FIXME: set DV zero register?
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      Res = -1;
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      Res = std::numeric_limits<std::int32_t>::min();
    } else {
      Res = Dividend / Divisor;
    }
  } else if (Mnemo == "divu") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      Res = std::numeric_limits<std::uint32_t>::max();
    } else {
      Res = (unsigned int)Dividend / (unsigned int)Divisor;
    }
  } else if (Mnemo == "rem") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      Res = Dividend;
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      Res = 0;
    } else {
      Res = PS.getDERs1Val() % PS.getDERs2Val();
    }
  } else if (Mnemo == "remu") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      Res = Dividend;
    } else {
      Res = (unsigned int)PS.getDERs1Val() % (unsigned int)PS.getDERs2Val();
    }

    // J-type
  } else if (Mnemo == "jal") {
    Res = PS.getPCs(EX) + 4;
    Address nextPC = PS.getPCs(EX) + signExtend(PS.getDEImmVal(), 20);
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);

    // B-type
  } else if (Mnemo == "beq") {
    if (PS.getDERs1Val() == PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
  } else if (Mnemo == "bne") {
    if (PS.getDERs1Val() != PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
  } else if (Mnemo == "blt") {
    if (PS.getDERs1Val() < PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
  } else if (Mnemo == "bge") {
    if (PS.getDERs1Val() >= PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
  } else if (Mnemo == "bltu") {
    if ((unsigned)PS.getDERs1Val() < (unsigned)PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
  } else if (Mnemo == "bgeu") {
    if ((unsigned)PS.getDERs1Val() >= (unsigned)PS.getDERs2Val()) {
      Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
      PS.setBranchPC(nextPC);
      PS.setInvalid(DE);
      PS.setInvalid(IF);
    }
    // S-type
  } else if (Mnemo == "sb") {
    // FIXME: wrap add?
    Res = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

  } else if (Mnemo == "sh") {
    Res = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

  } else if (Mnemo == "sw") {
    Res = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

    // U-type
  } else if (Mnemo == "lui") {
    Res = PS.getDEImmVal() << 12;
  } else if (Mnemo == "auipc") {
    Res = PS.getPCs(EX) + (PS.getDEImmVal() & 0xfffff000);
  } else {
    assert(false && "unimplemented!");
  }

  // if (INVALID_EX.find(Inst->getMnemo()) != INVALID_EX.end()) {
  //   // FIXME: invalide itself, is this right?
  //   PS.setInvalid(EX);
  //   PS.setInvalid(DE);
  //   PS.setInvalid(IF);
  //   // TODO: reset PC
  // }
  // // TODO: implemnet Branched PC
  // if (false)
  //   PS.setBranchPC(PC + 4);

  PS.setEXRdVal(Res);
  PS.setEXImmVal(Imm);
}

// register access shuold be done in this phase, exec shuoldn't access
// GPRegs directly.
void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  // FIXME: PS indexing seems not consistent(it's not only instructions)
  const auto &Inst = PS[STAGES::DE];
  unsigned Imm = 0;

  // FIXME: this is incorrect, because U-type and J-type instr's Rs1 or Rs2
  // is a part of immediate. Check if the inst is one of those.

  // Register access on Rs1
  if (PS[STAGES::MA] && Inst->getRs1() == PS[STAGES::MA]->getRd()) {
    // MA forward
    PS.setDERs1Val(PS.getMARdVal());
    std::cerr << "Forwarding from MA\n";

  } else if (PS[STAGES::EX] && Inst->getRs1() == PS[STAGES::EX]->getRd()) {
    // EX forward
    PS.setDERs1Val(PS.getEXRdVal());
    std::cerr << "Forwarding from EX\n";

  } else {
    PS.setDERs1Val(GPRegs[Inst->getRs1()]);
  }

  // Register access on Rs2
  if (PS[STAGES::MA] && Inst->getRs2() == PS[STAGES::MA]->getRd()) {
    PS.setDERs2Val(PS.getMARdVal());
    std::cerr << "Forwarding from MA\n";

  } else if (PS[STAGES::EX] && Inst->getRs2() == PS[STAGES::EX]->getRd()) {
    PS.setDERs2Val(PS.getEXRdVal());
    std::cerr << "Forwarding from EX\n";

  } else {
    PS.setDERs2Val(GPRegs[Inst->getRs2()]);
  }

  // Decode immdediate value
  if (ITypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getImm(), 12);

  } else if (STypeKinds.count(Inst->getMnemo())) {
    unsigned Offset =
        (Inst->getVal() & 0xfe000000) >> 20 | ((Inst->getVal() >> 7) & 0x1f);
    Imm = signExtend(Offset, 12);

  } else if (RTypeKinds.count(Inst->getMnemo())) {
    // no Imm
  } else if (JTypeKinds.count(Inst->getMnemo())) {
    Imm = ((Inst->getVal() & 0x80000000) >> 11) | (Inst->getVal() & 0xff000) |
          ((Inst->getVal() >> 9) & 0x800) | ((Inst->getVal() >> 20) & 0x7fe);

  } else if (BTypeKinds.count(Inst->getMnemo())) {
    Imm = (Inst->getVal() > 0x80000000) >> 19 | ((Inst->getVal() & 0x80) << 4) |
          ((Inst->getVal() >> 20) & 0x7e0) | ((Inst->getVal() >> 7) & 0x1e);
  } else if (UTypeKinds.count(Inst->getMnemo())) {
    Imm = (Inst->getVal() & 0xfffff000) >> 12;
  } else {
    assert(false && "unimplemented!");
  }
  PS.setDEImmVal(Imm);
  // TODO: stall 1 cycle if the inst is load;
}
void RIPSimulator::fetch(Memory &, PipelineStates &) {
  // const auto &Inst = PS[STAGES::IF];
  // FIXME: how and when can I change PC?
}

void RIPSimulator::runFromDRAMBASE() {
  PC = DRAM_BASE;

  while (true) {
    auto InstPtr = Dec.decode(Mem.readWord(PC));
    PS.pushPC(PC);
    if (InstPtr) {
      PC += 4;
    }
    PS.push(std::move(InstPtr));
    // FIXME: might this be wrong if branch prediction happens.
    CycleNum++;
    if (PS[STAGES::WB] != nullptr)
      writeback(GPRegs, PS);
    if (PS[STAGES::MA] != nullptr)
      memoryaccess(Mem, PS);
    if (PS[STAGES::EX] != nullptr)
      exec(PS);
    if (PS[STAGES::DE] != nullptr)
      decode(GPRegs, PS);
    if (PS[STAGES::IF] != nullptr)
      fetch(Mem, PS);

    PS.fillBubble();

    if (PS.isEmpty()) {
      break;
    }
    if (auto NextPC = PS.takeBranchPC()) {
      std::cerr << std::hex << "Branch from " << PC << " to ";
      PC = *NextPC;
      std::cerr << std::hex << PC << "\n";
    }
#ifdef DEBUG
    PS.dump();
    dumpGPRegs();
    dumpCSRegs();
#endif
  }
}
