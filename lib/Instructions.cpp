#include "Instructions.h"

std::optional<Exception> IInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = IT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "addi") {
    GPRegs.write(Rd.to_ulong(), GPRegs[Rs1.to_ulong()] + ImmI);
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
    GPRegs.write(Rd.to_ulong(), PC + 4);
    PC = (GPRegs[Rs1.to_ulong()] + ImmI) & ~1;
  } else if (Mnemo == "lb") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    Byte V = Mem.readByte(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs.write(Rd.to_ulong(), (signed char)V);
    PC += 4;
  } else if (Mnemo == "lh") {
    // FIXME: unsigned to signed safe cast (not implementation defined way)
    HalfWord V = Mem.readHalfWord(GPRegs[Rs1.to_ulong()] + ImmI);
    std::cerr << std::dec << "lh: V= " << V << "\n";
    std::cerr << "lh: V= " << (signed short)V << "\n";
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
    CSRVal CV = States.read(Imm.to_ulong() & 0xfff);
    // let t = self.state.read(csr_addr);
    // self.state.write(csr_addr, self.xregs.read(rs1));
    // self.xregs.write(rd, t);
    PC += 4;
  } else if (Mnemo == "csrrs") {
    // let t = self.state.read(csr_addr);
    // self.state.write(csr_addr, t | self.xregs.read(rs1));
    // self.xregs.write(rd, t);
    PC += 4;
  } else if (Mnemo == "csrrc") {
    // let t = self.state.read(csr_addr);
    // self.state.write(csr_addr, t & (!self.xregs.read(rs1)));
    // self.xregs.write(rd, t);
    PC += 4;
  } else if (Mnemo == "csrrwi") {
    // let zimm = rs1;
    // self.xregs.write(rd, self.state.read(csr_addr));
    // self.state.write(csr_addr, zimm);
    PC += 4;
  } else if (Mnemo == "csrrsi") {
    // let zimm = rs1;
    // let t = self.state.read(csr_addr);
    // self.state.write(csr_addr, t | zimm);
    // self.xregs.write(rd, t);
    PC += 4;
  } else if (Mnemo == "csrrci") {
    // let zimm = rs1;
    // let t = self.state.read(csr_addr);
    // self.state.write(csr_addr, t & (!zimm));
    // self.xregs.write(rd, t);
    PC += 4;
  } else if (Mnemo == "ecall") {
    // FIXME: check current mode
    return Exception::EnvironmentCallFromMMode;
  } else
    assert(false && "unimplemented! or not exist");
  return std::nullopt;
}

std::optional<Exception> RInstruction::exec(Address &PC, GPRegisters &GPRegs,
                                            Memory &Mem, CSRs &States,
                                            ModeKind &Mode) {
  std::string Mnemo = RT.getMnemo();
  if (Mnemo == "add") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] + GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "sub") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] - GPRegs[Rs2.to_ulong()]);
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
    GPRegs.write(Rd.to_ulong(),
                 ((signed long long)GPRegs[Rs1.to_ulong()] * (signed long long)GPRegs[Rs2.to_ulong()]) & 0xffff);
    PC += 4;
  } else if (Mnemo == "mulh") {
    GPRegs.write(Rd.to_ulong(),
                 ((signed long long)GPRegs[Rs1.to_ulong()] * (signed long long)GPRegs[Rs2.to_ulong()]) >> 32);
    PC += 4;
  } else if (Mnemo == "mulhsu") {
    GPRegs.write(Rd.to_ulong(),
                 ((signed long long)GPRegs[Rs1.to_ulong()] * (unsigned long long)(unsigned int)GPRegs[Rs2.to_ulong()]) >> 32);
    PC += 4;
  } else if (Mnemo == "mulhu") {
    GPRegs.write(Rd.to_ulong(),
                 ((unsigned long long)(unsigned int)GPRegs[Rs1.to_ulong()] * (unsigned long long)(unsigned int)GPRegs[Rs2.to_ulong()]) >> 32);
    PC += 4;
  } else if (Mnemo == "div") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] / GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "divu") {
    GPRegs.write(Rd.to_ulong(),
                 (unsigned int)GPRegs[Rs1.to_ulong()] / (unsigned int)GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "rem") {
    GPRegs.write(Rd.to_ulong(),
                 GPRegs[Rs1.to_ulong()] % GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else if (Mnemo == "remu") {
    GPRegs.write(Rd.to_ulong(),
                 (unsigned int)GPRegs[Rs1.to_ulong()] % (unsigned int)GPRegs[Rs2.to_ulong()]);
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
    GPRegs.write(Rd.to_ulong(), ImmI);
    PC += 4;
  } else if (Mnemo == "auipc") {
    GPRegs.write(Rd.to_ulong(), PC + (ImmI & 0xfffff000));
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
