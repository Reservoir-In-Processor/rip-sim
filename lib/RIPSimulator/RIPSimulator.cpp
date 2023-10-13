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
      std::cerr << "Rs1Val=" << DERs1Val << ", ";
      std::cerr << "Rs2Val=" << DERs2Val << ", ";
      std::cerr << "ImmVal=" << DEImmVal << ", ";
      std::cerr << "CSRVal=" << DECSRVal << "\n";
      break;

    case STAGES::EX:
      std::cerr << "CSRVal=" << EXCSRVal << ", ";
      std::cerr << "RdVal=" << EXRdVal << "\n";
      break;

    case STAGES::MA:
      std::cerr << "CSRVal=" << MACSRVal << ", ";
      std::cerr << "RdVal=" << MARdVal << "\n";

      break;

    default:
      std::cerr << "\n";
      break;
    }
  }
}
const std::set<std::string> CSR_INSTs = {"csrrw",  "csrrs",  "csrrc",
                                         "csrrwi", "csrrsi", "csrrci"};

RIPSimulator::RIPSimulator(std::istream &is) : PC(DRAM_BASE), StagesNum(0) {
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

  if (Mnemo == "sw" || Mnemo == "sh" ||
      Mnemo == "sb") { // FIXME: how about other insts?
    // Instructions without writeback

  } else if (Mnemo == "jalr") {
    Res = PS.getPCs(MA);
    GPRegs.write(Inst->getRd(), PS.getPCs(MA));

  } else if (CSR_INSTs.count(Mnemo)) {
    RegVal RdVal = PS.getMARdVal();
    RegVal CV = PS.getMACSRVal();

    RegVal CSRAddr = PS.getMAImmVal() & 0xfff;

    States.write(CSRAddr, CV);
    GPRegs.write(Inst->getRd(), RdVal);

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
  const RegVal MACSRVal = PS.getEXCSRVal();
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
    // FIXME: unsigned to signed safe cast (not implementation defined way)
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
    // FIXME: Forwarding should be on decode.
    // } else if (Mnemo == "csrrs") {
    //   if (PS[STAGES::WB] && Imm == PS[STAGES::WB]->getImm()) {
    //     std::cerr << "Forwarding from WB to MA."
    //               << "\n";
    //     Res = PS.getWBImmVal();
    // }
  }

  PS.setMARdVal(Res);
  PS.setMACSRVal(MACSRVal);
  PS.setMAImmVal(Imm);
}

const std::set<std::string> INVALID_EX = {"lbu",  "lhu",   "beq",   "blt",
                                          "bge",  "bltu",  "bgeu",  "jal",
                                          "jalr", "ecall", "ebreak"};

void RIPSimulator::exec(PipelineStates &) {
  const auto &Inst = PS[STAGES::EX];
  RegVal RdVal = 0;
  RegVal CV = 0;
  RegVal Imm = PS.getDEImmVal();

  std::string Mnemo = Inst->getMnemo();
  // I-type
  if (Mnemo == "addi") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "slti") {
    RdVal = PS.getDERs1Val() < PS.getDEImmVal();
  } else if (Mnemo == "sltiu") {
    RdVal = (unsigned)PS.getDERs1Val() < (unsigned)PS.getDEImmVal();
  } else if (Mnemo == "xori") {
    RdVal = PS.getDERs1Val() ^ PS.getDEImmVal();
  } else if (Mnemo == "ori") {
    RdVal = PS.getDERs1Val() | PS.getDEImmVal();
  } else if (Mnemo == "andi") {
    RdVal = PS.getDERs1Val() & PS.getDEImmVal();
  } else if (Mnemo == "jalr") {
    RdVal = PS.getPCs(EX) + 4;

    Address nextPC = PS.getDERs1Val() + signExtend(PS.getDEImmVal(), 12);
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);

  } else if (Mnemo == "lb") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lh") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lw") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lbu") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "lhu") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
  } else if (Mnemo == "slli") { // FIXME: shamt
    RdVal = (unsigned)PS.getDERs1Val() << PS.getDEImmVal();
  } else if (Mnemo == "srli") {
    RdVal = (unsigned)PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "srai") {
    RdVal = PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "fence") {
    // FIXME: currently expected to be nop
  } else if (Mnemo == "fence.i") {
    // FIXME: currently expected to be nop
  } else if (Mnemo == "csrrw") {
    CV = PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrs") {
    CV = PS.getDECSRVal() | PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrc") {
    CV = PS.getDECSRVal() & ~PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrwi") {
    CV = PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrsi") {
    CV = PS.getDERs1Val() | PS.getDECSRVal();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrci") {
    CV = PS.getDECSRVal() & ~PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
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
    RdVal = PS.getDERs1Val() + PS.getDERs2Val();
  } else if (Mnemo == "sub") {
    RdVal = PS.getDERs1Val() - PS.getDERs2Val();
  } else if (Mnemo == "sll") {
    RdVal = PS.getDERs1Val() << (PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "slt") {
    RdVal = PS.getDERs1Val() < PS.getDERs2Val();
  } else if (Mnemo == "sltu") {
    RdVal = (unsigned)PS.getDERs1Val() < (unsigned)PS.getDERs2Val();
  } else if (Mnemo == "xor") {
    RdVal = PS.getDERs1Val() ^ PS.getDERs2Val();
  } else if (Mnemo == "srl") {
    RdVal = (unsigned)PS.getDERs1Val() >> (PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "sra") {
    RdVal = (signed)PS.getDERs1Val() >> (signed)(PS.getDERs2Val() & 0b11111);
  } else if (Mnemo == "or") {
    RdVal = PS.getDERs1Val() | PS.getDERs2Val();
  } else if (Mnemo == "and") {
    RdVal = PS.getDERs1Val() & PS.getDERs2Val();
  } else if (Mnemo == "mul") {
    RdVal = ((signed long long)PS.getDERs1Val() *
             (signed long long)PS.getDERs2Val()) &
            0xffffffff;
  } else if (Mnemo == "mulh") {
    RdVal = ((signed long long)PS.getDERs1Val() *
             (signed long long)PS.getDERs2Val()) >>
            32;
  } else if (Mnemo == "mulhsu") {
    RdVal = ((signed long long)PS.getDERs1Val() *
             (unsigned long long)(unsigned int)PS.getDERs2Val()) >>
            32;
  } else if (Mnemo == "mulhu") {
    RdVal = ((unsigned long long)(unsigned int)PS.getDERs1Val() *
             (unsigned long long)(unsigned int)PS.getDERs2Val()) >>
            32;
  } else if (Mnemo == "div") {
    // FIXME: set DV zero register?
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      RdVal = -1;
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      RdVal = std::numeric_limits<std::int32_t>::min();
    } else {
      RdVal = Dividend / Divisor;
    }
  } else if (Mnemo == "divu") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      RdVal = std::numeric_limits<std::uint32_t>::max();
    } else {
      RdVal = (unsigned int)Dividend / (unsigned int)Divisor;
    }
  } else if (Mnemo == "rem") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      RdVal = Dividend;
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      RdVal = 0;
    } else {
      RdVal = PS.getDERs1Val() % PS.getDERs2Val();
    }
  } else if (Mnemo == "remu") {
    RegVal Divisor = PS.getDERs2Val(), Dividend = PS.getDERs1Val();
    if (Divisor == 0) {
      RdVal = Dividend;
    } else {
      RdVal = (unsigned int)PS.getDERs1Val() % (unsigned int)PS.getDERs2Val();
    }

    // J-type
  } else if (Mnemo == "jal") {
    RdVal = PS.getPCs(EX) + 4;
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
    if (PS.getDERs1Val() < PS.getDERs2Val()) { // Branch
      BP.setEXBranched(true);

      if (BP.getDEBranchPred()) {
        std::cerr << "Branch pred: hit "
                  << "\n";

      } else {

        std::cerr << "Branch pred: miss"
                  << "\n";
        Address nextPC = PS.getPCs(EX) + PS.getDEImmVal();
        PS.setBranchPC(nextPC);
        PS.setInvalid(DE);
        PS.setInvalid(IF);
      }
    } else { // not Branch

      BP.setEXBranched(false);
      if (BP.getDEBranchPred() == false) {
        std::cerr << "Branch pred: hit "
                  << "\n";

      } else {
        std::cerr << "Branch pred: miss"
                  << "\n";
        Address nextPC = PS.getPCs(EX) + 4;
        PS.setBranchPC(nextPC);
        PS.setInvalid(DE);
        PS.setInvalid(IF);
      }
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
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

  } else if (Mnemo == "sh") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

  } else if (Mnemo == "sw") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
    RegVal Rs2 = PS.getDERs2Val();
    PS.setEXRs2Val(Rs2);

    // U-type
  } else if (Mnemo == "lui") {
    RdVal = PS.getDEImmVal() << 12;
  } else if (Mnemo == "auipc") {
    RdVal = PS.getPCs(EX) + (PS.getDEImmVal() << 12);

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

  PS.setEXRdVal(RdVal);
  PS.setEXCSRVal(CV);
  PS.setEXImmVal(Imm);
}

// register access shuold be done in this phase, exec shuoldn't access
// GPRegs directly.
void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  // FIXME: PS indexing seems not consistent(it's not only instructions)
  const auto &Inst = PS[STAGES::DE];
  int Imm = 0;

  // Register access on Rs1
  if (Inst->getMnemo() == "csrrwi" || Inst->getMnemo() == "csrrsi" ||
      Inst->getMnemo() == "csrrci") {
    PS.setDERs1Val((unsigned int)Inst->getRs1());
  } else if (PS[STAGES::EX] && Inst->getRs1() == PS[STAGES::EX]->getRd()) {
    // EX forward
    PS.setDERs1Val(PS.getEXRdVal());
    std::cerr << "Forwarding Rs1 from EX: " << Inst->getMnemo() << "\n";
  } else if (PS[STAGES::MA] && Inst->getRs1() == PS[STAGES::MA]->getRd()) {
    // MA forward
    PS.setDERs1Val(PS.getMARdVal());
    std::cerr << "Forwarding Rs1 from MA: " << Inst->getMnemo() << "\n";
  } else {
    PS.setDERs1Val(GPRegs[Inst->getRs1()]);
  }

  // Register access on CSR
  if (PS[STAGES::EX] && CSR_INSTs.count(PS[STAGES::EX]->getMnemo()) &&
      (Inst->getImm() == PS[STAGES::EX]->getImm())) {

    PS.setDECSRVal(PS.getEXCSRVal());
    std::cerr << "Forwarding CSR val from EX : " << Inst->getMnemo() << " "
              << PS.getEXCSRVal() << "\n";

  } else if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
             (Inst->getImm() == PS[STAGES::MA]->getImm())) {

    PS.setDECSRVal(PS.getMACSRVal());
    std::cerr << "Forwarding CSR val from MA: " << Inst->getMnemo() << "\n";

  } else {
    PS.setDECSRVal(States[Inst->getImm()]);
  }

  // Register access on Rs2
  if (PS[STAGES::EX] && Inst->getRs2() == PS[STAGES::EX]->getRd()) {
    PS.setDERs2Val(PS.getEXRdVal());
    std::cerr << "Forwarding Rs2 from EX: " << Inst->getMnemo() << "\n";
  } else if (PS[STAGES::MA] && Inst->getRs2() == PS[STAGES::MA]->getRd()) {
    PS.setDERs2Val(PS.getMARdVal());
    std::cerr << "Forwarding Rs2 from MA: " << Inst->getMnemo() << "\n";
  } else {
    PS.setDERs2Val(GPRegs[Inst->getRs2()]);
  }

  // Decode immediate value
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
    Imm = signExtend(Imm, 20);

  } else if (BTypeKinds.count(Inst->getMnemo())) {
    Imm = (Inst->getVal() > 0x80000000) >> 19 | ((Inst->getVal() & 0x80) << 4) |
          ((Inst->getVal() >> 20) & 0x7e0) | ((Inst->getVal() >> 7) & 0x1e);
    Imm = signExtend(Imm, 12);

    bool BranchPred = BP.getEXBranched();
    BP.setDEBranchPred(BranchPred);

  } else if (UTypeKinds.count(Inst->getMnemo())) {
    Imm = (Inst->getVal() & 0xfffff000) >> 12;
    Imm = signExtend(Imm, 20);
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

const std::set<std::string> BranchInsts = {"beq",  "blt",  "bge",
                                           "bltu", "bgeu", "bne"};

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
      std::cerr << std::dec << "Total stages: " << StagesNum << "\n";
      break;
    }

    // Branch prediction
    if (PS[STAGES::DE] && BranchInsts.count(PS[STAGES::DE]->getMnemo())) {
      std::cerr << "Branch " << BP.getDEBranchPred() << "\n";

      if (BP.getDEBranchPred()) {
        PC = PS.getPCs(DE) + PS.getDEImmVal();
      }
    }

    if (auto NextPC = PS.takeBranchPC()) {
      std::cerr << std::hex << "Branch from 0x" << PC << " to ";
      PC = *NextPC;
      std::cerr << std::hex << "0x" << PC << "\n";
    }

    StagesNum++;

#ifdef DEBUG
    PS.dump();
    dumpGPRegs();
    dumpCSRegs();
#endif
  }
}
