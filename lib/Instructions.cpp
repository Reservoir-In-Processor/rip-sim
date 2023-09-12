#include "Instructions.h"
void IInstruction::exec(PC &P, GPRegisters &GPRegs, Memory &M,
                        CustomRegisters &) {
  std::string Mnemo = IT.getMnemo();
  int ImmI = signExtend(Imm);
  if (Mnemo == "addi")
    GPRegs[Rd.to_ulong()] = GPRegs[Rs1.to_ulong()] + ImmI;
  else if (Mnemo == "slti")
    GPRegs[Rd.to_ulong()] = (signed)GPRegs[Rs1.to_ulong()] < ImmI;
  else if (Mnemo == "sltiu")
    GPRegs[Rd.to_ulong()] = (unsigned)GPRegs[Rs1.to_ulong()] < ImmI;
  else if (Mnemo == "xori")
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] ^ ImmI; // FIXME: sext?
  else if (Mnemo == "ori")
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] | ImmI; // FIXME: sext?
  else if (Mnemo == "andi")
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] & ImmI; // FIXME: sext?
  else if (Mnemo == "jalr")
    assert(false && "unimplemented!");
  else if (Mnemo == "lb")
    assert(false && "unimplemented!");
  else if (Mnemo == "lh")
    assert(false && "unimplemented!");
  else if (Mnemo == "lw")
    assert(false && "unimplemented!");
  else if (Mnemo == "lbu")
    assert(false && "unimplemented!");
  else if (Mnemo == "lhu")
    assert(false && "unimplemented!");
  // FIXME: shamt
  else if (Mnemo == "slli")
    GPRegs[Rd.to_ulong()] = (unsigned)GPRegs[Rs1.to_ulong()]
                            << Imm.to_ulong(); // FIXME: sext?
  else if (Mnemo == "srli")
    GPRegs[Rd.to_ulong()] =
        (unsigned)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong(); // FIXME: sext?
  else if (Mnemo == "srai")
    GPRegs[Rd.to_ulong()] =
        (signed)GPRegs[Rs1.to_ulong()] >> Imm.to_ulong(); // FIXME: sext?
  else
    assert(false && "unimplemented! or not exist");
}
