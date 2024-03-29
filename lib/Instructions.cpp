#include "Instructions.h"
#include <limits>

std::optional<Exception> IInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = IT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "addi") {
    // To avoid signed overflow.
    union {
      unsigned un;
      int in;
    } u = {.un = (unsigned)GPRegs[Rs1.to_ulong()] + (unsigned)ImmI};
    GPRegs.write(Rd.to_ulong(), u.in);
    PC += 4;
  } else if (Mnemo == "slti") {
    GPRegs.write(Rd.to_ulong(), (signed)GPRegs[Rs1.to_ulong()] < ImmI);
    PC += 4;
  } else if (Mnemo == "sltiu") {
    GPRegs.write(Rd.to_ulong(),
                 (unsigned)GPRegs[Rs1.to_ulong()] < (unsigned)ImmI);
    PC += 4;
  } else if (Mnemo == "xori") {
    GPRegs.write(Rd.to_ulong(), (unsigned)GPRegs[Rs1.to_ulong()] ^ ImmI);
    PC += 4;
  } else if (Mnemo == "ori") {
    // FIXME: sext?
    GPRegs.write(Rd.to_ulong(), (unsigned)GPRegs[Rs1.to_ulong()] | ImmI);
    PC += 4;
  } else if (Mnemo == "andi") {
    GPRegs.write(Rd.to_ulong(), (unsigned)GPRegs[Rs1.to_ulong()] & ImmI);
    PC += 4;
  } else if (Mnemo == "jalr") {
    // FIXME: should addresss calculation be wrapped?
    Address CurPC = PC;
    PC = (GPRegs[Rs1.to_ulong()] + ImmI) & ~1;
    GPRegs.write(Rd.to_ulong(), CurPC + 4);
  } else if (Mnemo == "lb") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Byte V = Mem.readByte(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (signed char)V);
    PC += 4;
  } else if (Mnemo == "lh") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    HalfWord V = Mem.readHalfWord(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (signed short)V);
    PC += 4;
  } else if (Mnemo == "lw") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Word V = Mem.readWord(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (signed)V);
    PC += 4;
  } else if (Mnemo == "lbu") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Byte V = Mem.readByte(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (unsigned char)V);
    PC += 4;
  } else if (Mnemo == "lhu") {
    HalfWord V = Mem.readHalfWord(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (unsigned short)V);
    PC += 4;
  } else if (Mnemo == "slli") { // FIXME: shamt
    // FIXME: sext?
    GPRegs.write(Rd.to_ulong(),
                 (unsigned)GPRegs[Rs1.to_ulong()] << Imm.to_ulong());
    PC += 4;
  } else if (Mnemo == "srli") {
    GPRegs.write(Rd.to_ulong(),
                 (unsigned)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong());
    PC += 4;
  } else if (Mnemo == "srai") {
    GPRegs.write(Rd.to_ulong(),
                 (signed)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong());
    PC += 4;
  } else if (Mnemo == "fence") {
    // FIXME: currently expected to be nop
    PC += 4;
  } else if (Mnemo == "fence.i") {
    // FIXME: currently expected to be nop
    PC += 4;
  } else if (Mnemo == "csrrw") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    States.write(CA, GPRegs[Rs1.to_ulong()]);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
  } else if (Mnemo == "csrrs") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    States.write(CA, GPRegs[Rs1.to_ulong()] | CV);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
  } else if (Mnemo == "csrrc") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    States.write(CA, GPRegs[Rs1.to_ulong()] & !CV);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
  } else if (Mnemo == "csrrwi") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    CSRVal ZImm = Rs1.to_ulong();
    States.write(CA, ZImm);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
  } else if (Mnemo == "csrrsi") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    CSRVal ZImm = Rs1.to_ulong();
    States.write(CA, CV | ZImm);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
  } else if (Mnemo == "csrrci") {
    CSRAddress CA = Imm.to_ulong() & 0xfff;
    CSRVal CV = States.read(CA);
    CSRVal ZImm = Rs1.to_ulong();
    States.write(CA, CV & !ZImm);
    GPRegs.write(Rd.to_ulong(), CV);
    PC += 4;
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
  } else if (Mnemo == "uret") {
    // TODO: uret
    assert(false && "uret: unimplemented!");
    return Exception::IllegalInstruction;
  } else if (Mnemo == "sret") {
    // TODO: sret
    assert(false && "sret: unimplemented!");
    return Exception::IllegalInstruction;
  } else if (Mnemo == "mret") {
    // FIXME: make constant on CSR.h
    PC = States.read(MEPC);
    // FIXME: add MSTATUS handle methods?
    CSRVal MSTATUSVal = States.read(MSTATUS);
    // Previous Privilege mode for Machine mode.
    ModeKind MPP = (ModeKind)((MSTATUSVal >> 11) & 0b11);
    if (MPP == ModeKind::User) {
      // FIXME: add methods?
      // set MPREV=0
      // MPREV: Modify privilege bit. 17-th bit of MSTATUS
      States.write(MSTATUS, MSTATUS & 0xfffdffff);
      // FIXME: riscv-tests expects UserMode?
      // Mode = ModeKind::User;
    } else if (MPP == ModeKind::Supervisor) {
      // set MPREV=0
      std::cerr << "Supervisor!\n";
      States.write(MSTATUS, MSTATUS & 0xfffdffff);
      Mode = ModeKind::Supervisor;
    } else if (MPP == ModeKind::Machine) {
      Mode = ModeKind::Machine;
    } else
      return Exception::IllegalInstruction;
    // set MIE to MPIE;
    // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of MSTATUS
    // MPIE: Previous Interrupt-Enable bit for machine mode. 7-th bit of MSTATUS
    bool MPIE = (bool)((MSTATUSVal >> 7) & 1);
    CSRVal ResVal = (MSTATUSVal & 0xfffffff7) | (MPIE << 3);

    // Set MPIE to 1
    ResVal |= 0b10000000;

    // Set MPP to 0
    ResVal &= 0xffffe7ff;
    States.write(MSTATUS, ResVal);
  } else if (Mnemo == "ext") {
    PC += 4;
    return R0;
  } else if (Mnemo == "extx") {
    PC += 4;
    return R1;
  } else {
    assert(false && "unimplemented! or not exist");
    return Exception::IllegalInstruction;
  }
  return std::nullopt;
}

std::optional<Exception> RInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = RT.getMnemo();
  if (Mnemo == "add") {
    // To avoid signed overflow.
    union {
      unsigned un;
      int in;
    } u = {.un = (unsigned)GPRegs[Rs1.to_ulong()] +
                 (unsigned)GPRegs[Rs2.to_ulong()]};
    GPRegs.write(Rd.to_ulong(), u.in);
    PC += 4;
  } else if (Mnemo == "sub") {
    // To avoid signed overflow.
    union {
      unsigned un;
      int in;
    } u = {.un = (unsigned)GPRegs[Rs1.to_ulong()] -
                 (unsigned)GPRegs[Rs2.to_ulong()]};
    GPRegs.write(Rd.to_ulong(), u.in);
    PC += 4;
  } else if (Mnemo == "sll") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] << (GPRegs[Rs2.to_ulong()] & 0b11111));
    PC += 4;
  } else if (Mnemo == "slt") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] < GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "sltu") {
    GPRegs.write(Rd.to_ulong(), (unsigned)GPRegs[Rs1.to_ulong()] <
                                    (unsigned)GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "xor") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] ^ GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "srl") {
    GPRegs.write(Rd.to_ulong(), (unsigned)GPRegs[Rs1.to_ulong()] >>
                                    (GPRegs[Rs2.to_ulong()] & 0b11111));
    PC += 4;
  } else if (Mnemo == "sra") {
    GPRegs.write(Rd.to_ulong(), (signed)GPRegs[Rs1.to_ulong()] >>
                                    (signed)(GPRegs[Rs2.to_ulong()] & 0b11111));
    PC += 4;
  } else if (Mnemo == "or") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] | GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "and") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] & GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "mul") {
    GPRegs.write(Rd.to_ulong(), ((signed long long)GPRegs[Rs1.to_ulong()] *
                                 (signed long long)GPRegs[Rs2.to_ulong()]) &
                                    0xffffffff);
    PC += 4;
  } else if (Mnemo == "mulh") {
    GPRegs.write(Rd.to_ulong(), ((signed long long)GPRegs[Rs1.to_ulong()] *
                                 (signed long long)GPRegs[Rs2.to_ulong()]) >>
                                    32);
    PC += 4;
  } else if (Mnemo == "mulhsu") {
    GPRegs.write(Rd.to_ulong(),
                 ((signed long long)GPRegs[Rs1.to_ulong()] *
                  (unsigned long long)(unsigned int)GPRegs[Rs2.to_ulong()]) >>
                     32);
    PC += 4;
  } else if (Mnemo == "mulhu") {
    GPRegs.write(Rd.to_ulong(),
                 ((unsigned long long)(unsigned int)GPRegs[Rs1.to_ulong()] *
                  (unsigned long long)(unsigned int)GPRegs[Rs2.to_ulong()]) >>
                     32);
    PC += 4;
  } else if (Mnemo == "div") {
    // FIXME: set DV zero register?
    RegVal Divisor = GPRegs[Rs2.to_ulong()], Dividend = GPRegs[Rs1.to_ulong()];
    if (Divisor == 0) {
      GPRegs.write(Rd.to_ulong(), -1);
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      GPRegs.write(Rd.to_ulong(), std::numeric_limits<std::int32_t>::min());
    } else {
      GPRegs.write(Rd.to_ulong(), Dividend / Divisor);
    }
    PC += 4;
  } else if (Mnemo == "divu") {
    RegVal Divisor = GPRegs[Rs2.to_ulong()], Dividend = GPRegs[Rs1.to_ulong()];
    if (Divisor == 0) {
      GPRegs.write(Rd.to_ulong(), std::numeric_limits<std::uint32_t>::max());
    } else {
      GPRegs.write(Rd.to_ulong(),
                   (unsigned int)Dividend / (unsigned int)Divisor);
    }
    PC += 4;
  } else if (Mnemo == "rem") {
    RegVal Divisor = GPRegs[Rs2.to_ulong()], Dividend = GPRegs[Rs1.to_ulong()];
    if (Divisor == 0) {
      GPRegs.write(Rd.to_ulong(), Dividend);
    } else if (Dividend == std::numeric_limits<std::int32_t>::min() &&
               Divisor == -1) {
      GPRegs.write(Rd.to_ulong(), 0);
    } else {
      GPRegs.write(Rd.to_ulong(),
                   GPRegs[Rs1.to_ulong()] % GPRegs[Rs2.to_ulong()]);
    }
    PC += 4;
  } else if (Mnemo == "remu") {
    RegVal Divisor = GPRegs[Rs2.to_ulong()], Dividend = GPRegs[Rs1.to_ulong()];
    if (Divisor == 0) {
      GPRegs.write(Rd.to_ulong(), Dividend);
    } else {
      GPRegs.write(Rd.to_ulong(), (unsigned int)GPRegs[Rs1.to_ulong()] %
                                      (unsigned int)GPRegs[Rs2.to_ulong()]);
    }
    PC += 4;
  } else
    assert(false && "unimplemented! or not exist");
  return std::nullopt;
}

std::optional<Exception> UInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = UT.getMnemo();
  int ImmI = signExtend(Imm);

  if (Mnemo == "lui") {
    GPRegs.write(Rd.to_ulong(), ImmI << 12);
    PC += 4;
  } else if (Mnemo == "auipc") {
    GPRegs.write(Rd.to_ulong(), PC + (ImmI << 12));
    PC += 4;
  } else
    assert(false && "not exist");
  return std::nullopt;
}

std::optional<Exception> SInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = ST.getMnemo();

  int ImmI = signExtend(Imm);

  Address Ad = GPRegs[Rs1.to_ulong()] + ImmI;
  if ((unsigned)Ad == 0x10000000) { // picorv32 Dhrystone MMIO
    std::cerr << (char)GPRegs[Rs2.to_ulong()];
    PC += 4;
    return std::nullopt;
  }
  if (Mode == ModeKind::Epilogue) {
    std::cerr << "Epilogue:" << GPRegs[Rs2.to_ulong()] << "is written to " << Ad
              << '\n';
    PC += 4;
    return std::nullopt;
  }
  if (Mnemo == "sb") {
    // FIXME: wrap add?
    Address Ad = GPRegs[Rs1.to_ulong()] + ImmI;
    Mem.writeByte(Ad, GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "sh") {
    // FIXME: wrap add?
    Address Ad = GPRegs[Rs1.to_ulong()] + ImmI;
    Mem.writeHalfWord(Ad, GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "sw") {
    // FIXME: wrap add?
    Address Ad = GPRegs[Rs1.to_ulong()] + ImmI;
    Mem.writeWord(Ad, GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else
    assert(false && "unimplemented! or not exist");
  return std::nullopt;
}

std::optional<Exception> BInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = BT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "beq") {
    if (GPRegs[Rs1.to_ulong()] == GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else if (Mnemo == "bne") {
    if (GPRegs[Rs1.to_ulong()] != GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else if (Mnemo == "blt") {
    if (GPRegs[Rs1.to_ulong()] < GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else if (Mnemo == "bge") {
    if (GPRegs[Rs1.to_ulong()] >= GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else if (Mnemo == "bltu") {
    if ((unsigned)GPRegs[Rs1.to_ulong()] < (unsigned)GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else if (Mnemo == "bgeu") {
    if ((unsigned)GPRegs[Rs1.to_ulong()] >= (unsigned)GPRegs[Rs2.to_ulong()]) {
      PC += ImmI;
    } else {
      PC += 4;
    }
  } else
    assert(false && "unimplemented! or not exist");
  return std::nullopt;
}

std::optional<Exception> JInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = JT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "jal") {
    GPRegs.write(Rd.to_ulong(), PC + 4);
    PC += ImmI;
  } else
    assert(false && "unimplemented! or not exist");
  return std::nullopt;
}
