#include "Instructions.h"
void IInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = IT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "addi") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] + ImmI;
    PC += 4;
  } else if (Mnemo == "slti") {
    GPRegs[Rd.to_ulong()] = (signed)GPRegs[Rs1.to_ulong()] < ImmI;
    PC += 4;
  } else if (Mnemo == "sltiu") {
    GPRegs[Rd.to_ulong()] = (unsigned)GPRegs[Rs1.to_ulong()] < (unsigned)ImmI;
    PC += 4;
  } else if (Mnemo == "xori") {
    GPRegs[Rd.to_ulong()] = (unsigned)GPRegs[Rs1.to_ulong()] ^ ImmI;
    PC += 4;
  } else if (Mnemo == "ori") {
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] | ImmI; // FIXME: sext?
    PC += 4;
  } else if (Mnemo == "andi") {
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] & ImmI; // FIXME: sext?
    PC += 4;
  } else if (Mnemo == "jalr") {
    // FIXME: should addresss calculation be wrapped?
    GPRegs[Rd.to_ulong()] = PC + 4;
    PC = (GPRegs[Rs1.to_ulong()] + ImmI) & ~1;
  } else if (Mnemo == "lb")
    assert(false && "unimplemented!");
  else if (Mnemo == "lh")
    assert(false && "unimplemented!");
  else if (Mnemo == "lw") {
    Word V = M.readWord(GPRegs[Rs1.to_ulong()] + ImmI);
    GPRegs[Rd.to_ulong()] = V;
    PC += 4;
  } else if (Mnemo == "lbu")
    assert(false && "unimplemented!");
  else if (Mnemo == "lhu")
    assert(false && "unimplemented!");
  // FIXME: shamt
  else if (Mnemo == "slli") {
    GPRegs[Rd.to_ulong()] = (unsigned)GPRegs[Rs1.to_ulong()]
                            << Imm.to_ulong(); // FIXME: sext?
    PC += 4;
  } else if (Mnemo == "srli") {
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong(); // FIXME: sext?
    PC += 4;
  } else if (Mnemo == "srai") {
    GPRegs[Rd.to_ulong()] =
        (signed)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong(); // FIXME: sext?
    PC += 4;
  } else
    assert(false && "unimplemented! or not exist");
}

void RInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = RT.getMnemo();
  if (Mnemo == "add") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] + GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "sub") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] - GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "sll") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()]
                            << (GPRegs[Rs2.to_ulong()] & 0b11111);
    PC += 4;
  } else if (Mnemo == "slt") {
    GPRegs[Rd.to_ulong()] =
        (signed)GPRegs[Rs1.to_ulong()] < (signed)GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "sltu") {
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] < (unsigned)GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "xor") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] ^ GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "srl") {
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] >> (GPRegs[Rs2.to_ulong()] & 0b11111);
    PC += 4;
  } else if (Mnemo == "sra") {
    GPRegs[Rd.to_ulong()] = (signed)GPRegs[Rs1.to_ulong()] >>
                            (signed)(GPRegs[Rs2.to_ulong()] & 0b11111);
    PC += 4;
  } else if (Mnemo == "or") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] | GPRegs[Rs2.to_ulong()];
    PC += 4;
  } else if (Mnemo == "and") {
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] & GPRegs[Rs2.to_ulong()];
    PC += 4;
  }

  else
    assert(false && "unimplemented! or not exist");
}

void UInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = UT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "auipc") {
    GPRegs[Rd.to_ulong()] = PC + (ImmI & 0xfffff000);
    PC += 4;
  } else
    assert(false && "not exist");
}

void SInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = ST.getMnemo();

  int ImmI = signExtend(Imm);
  if (Mnemo == "sw") {
    // FIXME: wrap?
    Address Ad = GPRegs[Rs1.to_ulong()] + ImmI;
    M.writeWord(Ad, GPRegs[Rs2.to_ulong()]);
    PC += 4;
  } else
    assert(false && "unimplemented! or not exist");
}

void BInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
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
}

void JInstruction::exec(Address &PC, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = JT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "jal") {
    GPRegs[Rd.to_ulong()] = PC + 4;
    PC += ImmI;
  } else
    assert(false && "unimplemented! or not exist");
}
