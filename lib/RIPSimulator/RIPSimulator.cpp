#include "RIPSimulator/RIPSimulator.h"
#include <cmath>
#include <iostream>
#include <nlohmann/json.hpp>
#include <set>

namespace {
/// BitWidth < 32
int signExtend(const unsigned Imm, unsigned BitWidth) {
  if ((Imm >> (BitWidth - 1)) & 1) {
    return (~0 ^ ((int)(pow(2, BitWidth) - 1))) | (signed)Imm;
  } else {
    return Imm;
  }
}

} // namespace

const std::set<std::string> CSR_INSTs = {"csrrw",  "csrrs",  "csrrc",
                                         "csrrwi", "csrrsi", "csrrci"};

RIPSimulator::RIPSimulator(std::istream &is,
                           std::unique_ptr<BranchPredictor> BP,
                           Address _DRAMSize,
                           std::unique_ptr<Statistics> _Stats,
                           Address _DRAMBase, std::optional<Address> SPIValue)
    : Mem(_DRAMSize, _DRAMBase), PC(_DRAMBase), Mode(ModeKind::Machine),
      NumStages(0), GPRegs(_DRAMSize, _DRAMBase, SPIValue), BP(std::move(BP)),
      Stats(std::move(_Stats)) {

  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = _DRAMBase;
  // Load binary into memory
  while (is.read(Buff, 4)) {
    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    Mem.writeWord(P, InstVal);
    P += 4;
    CodeSize += 4;
  }
}

void RIPSimulator::dumpStats() {
  std::cerr << "========== BEGIN STATS ============"
            << "\n";
  std::cerr << std::dec << "Total stages: " << NumStages << "\n";

  Stats->printAllStatistics(std::cerr);

  if (BP)
    BP->printStat();
  std::cerr << "=========== END STATS ============="
            << "\n";
  std::cerr << "\n";
}

void RIPSimulator::writeback(GPRegisters &, PipelineStates &) {
  const auto &Inst = PS[STAGES::WB];
  // FIXME: use it just in time, generally.
  std::string Mnemo = Inst->getMnemo();
  RegVal Res = 0;

  if (BTypeKinds.count(Mnemo) || STypeKinds.count(Mnemo) || Mnemo == "fence" ||
      Mnemo == "fence.i") { // FIXME: how about other insts?
    // Instructions without writeback
    // TODO: don't set any value and return early.
    PS.setWBImmVal(0);
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
    Mem.writeWord((unsigned)MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "sh") {
    Mem.writeHalfWord((unsigned)MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "sb") {
    Mem.writeByte((unsigned)MARdVal, PS.getEXRs2Val());
  } else if (Mnemo == "lw") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Word V = Mem.readWord(MARdVal);
    Res = (signed)V;
  } else if (Mnemo == "lh") {
    HalfWord V = Mem.readHalfWord((unsigned)MARdVal);
    Res = (signed short)V;
  } else if (Mnemo == "lbu") {
    Byte V = Mem.readByte((unsigned)MARdVal);
    Res = (unsigned char)V;
  } else if (Mnemo == "lhu") {
    HalfWord V = Mem.readHalfWord((unsigned)MARdVal);
    Res = (unsigned short)V;
  } else if (Mnemo == "lb") {
    Byte V = Mem.readByte((unsigned)MARdVal);
    Res = (signed char)V;
  }

  PS.setMARdVal(Res);
  PS.setMACSRVal(MACSRVal);
  PS.setMAImmVal(Imm);
}

const std::set<std::string> INVALID_EX = {"lbu",  "lhu",   "beq",   "blt",
                                          "bge",  "bltu",  "bgeu",  "jal",
                                          "jalr", "ecall", "ebreak"};

std::optional<Exception> RIPSimulator::exec(PipelineStates &) {
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
    // FIXME: we can obviously predict those address.
    Address nextPC = PS.getDERs1Val() + signExtend(PS.getDEImmVal(), 12);
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);

  } else if (Mnemo == "lb" || Mnemo == "lh" || Mnemo == "lw" ||
             Mnemo == "lbu" || Mnemo == "lhu") {
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
    // FIXME: checking Rs1 checks on this is annoying now.
    if (PS[STAGES::DE] && Inst->hasRd())
      if ((PS[STAGES::DE]->hasRs1() &&
           Inst->getRd() == PS[STAGES::DE]->getRs1()) ||
          (PS[STAGES::DE]->hasRs2() &&
           Inst->getRd() == PS[STAGES::DE]->getRs2())) {
        PS.setStall(STAGES::DE);
        PS.setStall(STAGES::IF);
      }
  } else if (Mnemo == "slli") { // FIXME: shamt
    RdVal = (unsigned)PS.getDERs1Val() << PS.getDEImmVal();
  } else if (Mnemo == "srli") {
    RdVal = (unsigned)PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "srai") {
    RdVal = PS.getDERs1Val() >> PS.getDEImmVal();
  } else if (Mnemo == "fence" || Mnemo == "fence.i") {
    // FIXME: currently expected to be nop
  } else if (Mnemo == "csrrw" || Mnemo == "csrrwi") {
    CV = PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrs" || Mnemo == "csrrsi") {
    CV = PS.getDECSRVal() | PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "csrrc" || Mnemo == "csrrci") {
    CV = PS.getDECSRVal() & ~PS.getDERs1Val();
    RdVal = PS.getDECSRVal();
  } else if (Mnemo == "ecall") {
    if (Mode == ModeKind::User) {
      return Exception::EnvironmentCallFromUMode;
    } else if (Mode == ModeKind::Supervisor) {
      return Exception::EnvironmentCallFromSMode;
    } else if (Mode == ModeKind::Machine) {
      return Exception::EnvironmentCallFromMMode;
    } else {
      // FIXME: is this illegal inst?
      return Exception::IllegalInstruction;
    }
  } else if (Mnemo == "ebreak") {
    return Exception::Breakpoint;
  } else if (Mnemo == "mret") {
    // FIXME: state read, multiple state writing (and mode change) happens,
    // how can we divide these into stages?

    Address nextPC = States.read(MEPC);
    // FIXME: Forwarding happens on "mret" reading ???
    if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
        (MEPC == PS[STAGES::MA]->getIImm())) {
      nextPC = PS.getMACSRVal();
      std::cerr << "Exception: Forwarding MEPC val from MA : "
                << "\n";
    }
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);
    // FIXME: add MSTATUS handle methods
    // FIXME: Forwarding happens on "mret" reading ???
    CSRVal MSTATUSVal = States.read(MSTATUS);
    if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
        (MSTATUS == PS[STAGES::MA]->getIImm())) {
      MSTATUSVal = PS.getMACSRVal();
      std::cerr << "Exception: Forwarding MSTATUS val from MA : "
                << "\n";
    }

    // Previous Privilege mode for Machine mode.
    ModeKind MPPVal = (ModeKind)((MSTATUSVal >> 11) & 0b11);

    if (MPPVal == ModeKind::User) {
      States.setMPREV(0);
      // FIXME: riscv-tests expects UserMode?
      // Mode = ModeKind::User;
    } else if (MPPVal == ModeKind::Supervisor) {
      // set MPREV=0
      States.setMPREV(0);
      Mode = ModeKind::Supervisor;
    } else if (MPPVal == ModeKind::Machine) {
      Mode = ModeKind::Machine;
    } else
      return Exception::IllegalInstruction;
    // set MIE to MPIE;
    // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of MSTATUS
    // MPIE: Previous Interrupt-Enable bit for machine mode. 7-th bit of MSTATUS
    bool MPIEVal = (bool)((MSTATUSVal >> 7) & 1);
    States.setMIE(MPIEVal);

    // Set MPIE to 1
    States.setMPIE(true);

    // Set MPP to 0
    States.setMPP((ModeKind)0);
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

    // FIXME: we can obviously predict those address.
    RdVal = PS.getPCs(EX) + 4;
    Address nextPC = PS.getPCs(EX) + signExtend(PS.getDEImmVal(), 20);
    PS.setBranchPC(nextPC);
    PS.setInvalid(DE);
    PS.setInvalid(IF);

    // B-type
  } else if (BTypeKinds.count(Mnemo)) {
    bool Cond;
    if (Mnemo == "beq") {
      Cond = PS.getDERs1Val() == PS.getDERs2Val();

    } else if (Mnemo == "bne") {
      Cond = PS.getDERs1Val() != PS.getDERs2Val();
    } else if (Mnemo == "blt") {
      Cond = PS.getDERs1Val() < PS.getDERs2Val();
    } else if (Mnemo == "bge") {
      Cond = PS.getDERs1Val() >= PS.getDERs2Val();
    } else if (Mnemo == "bltu") {
      Cond = (unsigned)PS.getDERs1Val() < (unsigned)PS.getDERs2Val();
    } else if (Mnemo == "bgeu") {
      Cond = (unsigned)PS.getDERs1Val() >= (unsigned)PS.getDERs2Val();
    } else {
      assert(false && "unreachable!");
    }

    signed Offset = PS.getDEImmVal();
    Address NextPC = PS.getPCs(EX) + Offset;
    if (!BP) {
      if (Cond) {
        PC = NextPC;
        PS.setInvalid(DE);
        PS.setInvalid(IF);
      }

    } else {
      bool Pred = BP->getPrevPred();

      if (Pred ^ Cond) {
        if (Cond) {
          PS.setBranchPC(NextPC);
        } else {
          PS.setBranchPC(PS.getPCs(EX) + 4);
        }
        PS.setInvalid(DE);
        PS.setInvalid(IF);
      }

      BP->StatsUpdate(Cond, Pred);
      BP->Learn(Cond, PS.getPCs(EX)); // FIXME: How to pass PS
    }

    // S-type
  } else if (Mnemo == "sb" || Mnemo == "sw" || Mnemo == "sh") {
    // FIXME: wrap add?
    RdVal = PS.getDERs1Val() + PS.getDEImmVal();
    PS.setEXRs2Val(PS.getDERs2Val());
    // U-type
  } else if (Mnemo == "lui") {
    RdVal = PS.getDEImmVal() << 12;
  } else if (Mnemo == "auipc") {
    RdVal = PS.getPCs(EX) + (PS.getDEImmVal() << 12);
  } else {
    assert(false && "unimplemented!");
  }

  PS.setEXRdVal(RdVal);
  PS.setEXCSRVal(CV);
  PS.setEXImmVal(Imm);

  return std::nullopt;
}

namespace {

static bool forwardRs1OnDE(const std::unique_ptr<Instruction> &Inst,
                           PipelineStates &PS, GPRegisters &GPRegs) {
  if (PS[STAGES::EX] && PS[STAGES::EX]->hasRd() &&
      Inst->getRs1() == PS[STAGES::EX]->getRd()) {
    // EX forward
    PS.setDERs1Val(PS.getEXRdVal());
    DEBUG_ONLY(std::cerr << "Forwarding Rs1 from EX: " << Inst->getMnemo()
                         << "\n");
    return true;
  }
  if (PS[STAGES::MA] && PS[STAGES::MA]->hasRd() &&
      Inst->getRs1() == PS[STAGES::MA]->getRd()) {
    // MA forward
    PS.setDERs1Val(PS.getMARdVal());
    DEBUG_ONLY(std::cerr << "Forwarding Rs1 from MA: " << Inst->getMnemo()
                         << "\n");
    return true;
  }
  return false;
}

static bool forwardCSROnDE(const std::unique_ptr<Instruction> &Inst,
                           PipelineStates &PS, GPRegisters &GPRegs) {
  if (!CSR_INSTs.count(Inst->getMnemo()))
    return false;
  if (PS[STAGES::EX] && CSR_INSTs.count(PS[STAGES::EX]->getMnemo()) &&
      (Inst->getIImm() == PS[STAGES::EX]->getIImm())) {

    PS.setDECSRVal(PS.getEXCSRVal());
    DEBUG_ONLY(std::cerr << "Forwarding CSR val from EX : " << Inst->getMnemo()
                         << " " << PS.getEXCSRVal() << "\n");
    return true;
  }
  if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
      (Inst->getIImm() == PS[STAGES::MA]->getIImm())) {
    PS.setDECSRVal(PS.getMACSRVal());
    DEBUG_ONLY(std::cerr << "Forwarding CSR val from MA: " << Inst->getMnemo()
                         << "\n");
    return true;
  }
  return false;
}

static bool forwardRs2OnDE(const std::unique_ptr<Instruction> &Inst,
                           PipelineStates &PS, GPRegisters &GPRegs) {

  if (PS[STAGES::EX] && PS[STAGES::EX]->hasRd() &&
      Inst->getRs2() == PS[STAGES::EX]->getRd()) {
    PS.setDERs2Val(PS.getEXRdVal());
    DEBUG_ONLY(std::cerr << "Forwarding Rs2 from EX: " << Inst->getMnemo()
                         << "\n");
    return true;
  } else if (PS[STAGES::MA] && PS[STAGES::MA]->hasRd() &&
             Inst->getRs2() == PS[STAGES::MA]->getRd()) {
    PS.setDERs2Val(PS.getMARdVal());
    DEBUG_ONLY(std::cerr << "Forwarding Rs2 from MA: " << Inst->getMnemo()
                         << "\n");
    return true;
  }
  return false;
}

} // namespace

// register access shuold be done in this phase, exec shuoldn't access
// GPRegs directly.
void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  // FIXME: PS indexing seems not consistent(it's not only instructions)
  const auto &Inst = PS[STAGES::DE];
  int Imm = 0;

  // Register access on Rs1
  // FIXME: we can forward if EX or MA is also immediate CSR instructions.
  if (Inst->getMnemo() == "csrrwi" || Inst->getMnemo() == "csrrsi" ||
      Inst->getMnemo() == "csrrci") {
    PS.setDERs1Val((unsigned int)Inst->getRs1());
  } else if (Inst->hasRs1() && !forwardRs1OnDE(Inst, PS, GPRegs)) {
    PS.setDERs1Val(GPRegs[Inst->getRs1()]);
  }

  // Register access on CSR
  if (CSR_INSTs.count(Inst->getMnemo()) && !forwardCSROnDE(Inst, PS, GPRegs)) {
    PS.setDECSRVal(States[Inst->getIImm()]);
  }

  // Register access on Rs2
  if (Inst->hasRs2() && !forwardRs2OnDE(Inst, PS, GPRegs)) {
    PS.setDERs2Val(GPRegs[Inst->getRs2()]);
  }

  // Decode immediate value
  if (ITypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getIImm(), 12);
  } else if (STypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getSImm(), 12);
  } else if (JTypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getJImm(), 20);
  } else if (BTypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getBImm(), 13);

    if (BP) {
      bool pred = BP->Predict(PS.getPCs(DE));
      BP->setPrevPred(pred);

      if (pred) {
        BP->setBranchPredPC(PS.getPCs(DE) + Imm);
        PS.setInvalid(IF);
      }
      DEBUG_ONLY(std::cerr << std::hex << "Branch Pred: " << pred << "\n";);
    }
  } else if (UTypeKinds.count(Inst->getMnemo())) {
    Imm = signExtend(Inst->getUImm(), 20);
  }
  PS.setDEImmVal(Imm);
  // TODO: stall 1 cycle if the inst is load;
}
void RIPSimulator::fetch(Memory &, PipelineStates &) {}

bool RIPSimulator::handleException(Exception &E) {
  PS.setInvalid(DE);
  PS.setInvalid(IF);
  Address ExceptionPC = PS.getPCs(EX);
  ModeKind PrevMode = Mode;
  unsigned Cause = E;
  // FIXME: temporary exit with break
  if (E == Exception::Breakpoint) {
    std::cerr << "break happens\n";
    return false;
  }
  // TODO: move those on exec and write back.
  // PC change should be on exec.
  if (Mode == ModeKind::Machine) {
    CSRVal VecVal = States.read(MTVEC);
    // FIXME: Forwarding happens on exception handling?
    if (PS[STAGES::EX] && CSR_INSTs.count(PS[STAGES::EX]->getMnemo()) &&
        (MTVEC == PS[STAGES::EX]->getIImm())) {
      VecVal = PS.getEXCSRVal();
      std::cerr << "Exception: Forwarding MTVEC val from EX : "
                << "\n";
    } else if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
               (MTVEC == PS[STAGES::MA]->getIImm())) {
      VecVal = PS.getMACSRVal();
      std::cerr << "Exception: Forwarding MTVEC val from MA : "
                << "\n";
    }

    PC = VecVal & (~1);

    States.write(MEPC, ExceptionPC & (~1));

    States.write(MCAUSE, Cause);

    // Machine Trap Value Register
    States.write(MTVAL, trap_val(E, ExceptionPC, PS[STAGES::EX]->getVal()));

    // set MPIE to MIE;
    // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of
    // MSTATUS MPIE: Previous Interrupt-Enable bit for machine mode. 7-th
    // bit of MSTATUS

    // FIXME: Forwarding happens on exception handling?
    CSRVal MSTATUSVal = States.read(MSTATUS);
    if (PS[STAGES::EX] && CSR_INSTs.count(PS[STAGES::EX]->getMnemo()) &&
        (MSTATUS == PS[STAGES::EX]->getIImm())) {
      MSTATUSVal = PS.getEXCSRVal();
      std::cerr << "Exception: Forwarding MSTATUS val from EX : "
                << "\n";
    } else if (PS[STAGES::MA] && CSR_INSTs.count(PS[STAGES::MA]->getMnemo()) &&
               (MSTATUS == PS[STAGES::MA]->getIImm())) {
      MSTATUSVal = PS.getMACSRVal();
      std::cerr << "Exception: Forwarding MSTATUS val from MA : "
                << "\n";
    }

    MSTATUSVal = States.read(MSTATUS);
    bool MIE = (bool)((MSTATUSVal >> 3) & 1);
    States.setMPIE(MIE);

    States.setMIE(0);

    States.setMPP(PrevMode);

  } else {
    assert(false && "Non-Machine mode is unimplemented!");
    return false;
  }
  return true;
}

void RIPSimulator::run(std::optional<Address> StartAddress,
                       std::optional<Address> EndAddress) {
  if (StartAddress)
    PC = *StartAddress;

  while (!proceedNStage(1)) {
    if (EndAddress && PC == *EndAddress)
      break;
  }

  if (Stats)
    dumpStats();
  else {
    DEBUG_ONLY(dumpStats());
  }
  return;
}

bool RIPSimulator::proceedNStage(unsigned N) {
  while (N--) {
    DEBUG_ONLY(std::cerr << std::dec << "Num Stages=" << getNumStages() << " "
                         << std::hex << "PC=0x" << PC << "\n");

    // actual fetch and decode
    // handle stall
    if (PS.isStall(STAGES::IF)) {
      PS.proceedPC(-1);
      PS.proceed(nullptr);
      PS.clearStall();
    } else {
      auto InstPtr = Dec.decode(Mem.readWord(PC));
      PS.proceedPC(PC);
      // FIXME: this should inherently be moved the following update of PC, but
      // some stages refers PC and moving this to latter would break.
      if (InstPtr) {
        PC += 4;
      }
      PS.proceed(std::move(InstPtr));
    }

    // exit if pipeline is empty.
    if (PS.isEmpty()) {
      break;
    }

    std::optional<Exception> Except = std::nullopt;

    if (PS[STAGES::WB] != nullptr)
      writeback(GPRegs, PS);

    if (PS[STAGES::MA] != nullptr)
      memoryaccess(Mem, PS);

    if (PS[STAGES::EX] != nullptr)
      Except = exec(PS);

    if (!PS.isStall(STAGES::DE) && PS[STAGES::DE] != nullptr)
      decode(GPRegs, PS);

    if (!PS.isStall(STAGES::IF) && PS[STAGES::IF] != nullptr)
      fetch(Mem, PS);

    // Exception handling
    if (Except && !handleException(*Except))
      // FIXME: For current use, stop on ebreak. It's better to define when
      // proceedNStage returns true.
      return true;

    std::optional<Address> NextPC;
    if (BP) {
      NextPC = BP->takeBranchPredPC();
    }
    if (auto BranchTarget = PS.takeBranchPC())
      NextPC = BranchTarget;
    if (NextPC) {
      PC = *NextPC;
      DEBUG_ONLY(std::cerr << std::hex << "Branch from 0x" << PC << " to "
                           << "0x" << *NextPC << "\n");
    }

    PS.fillBubble();
    NumStages++;

    // Statistics calculation
    if (Stats) {
      if (auto &EXInst = PS[STAGES::EX]) {
        std::string Mnemo = EXInst->getMnemo();
        Stats->addInst(Mnemo);
        if (BTypeKinds.count(Mnemo))
          Stats->addBDistAndReset();
        else
          Stats->incrementBDist();
      }
    }

    DEBUG_ONLY(PS.dump(); dumpGPRegs(););
  }
  return PS.isEmpty();
}

void RIPSimulator::runInteractively(std::optional<Address> StartAddress,
                                    std::optional<Address> EndAddress) {
  if (StartAddress)
    PC = *StartAddress;
  while (!proceedNStage(1)) {
    // FIXME: is this necessary? maybe input is only needed on prediction.
    std::string buf = "";
    std::cin >> buf;
    buf.clear();
    // FIXME: this end address is on IF, should be EX.
    if (EndAddress && PC == *EndAddress)
      break;
    PS.printJSON(std::cout);
  }
  // FIXME: call last
  PS.printJSON(std::cout);

  if (Stats)
    dumpStats();
  else {
    DEBUG_ONLY(dumpStats());
  }
  return;
}
