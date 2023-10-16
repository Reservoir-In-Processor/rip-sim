
#include "Simulator/Simulator.h"
#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

Simulator::Simulator(std::istream &is)
    : Mem(), PC(DRAM_BASE), Mode(ModeKind::Machine) {
  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = DRAM_BASE;
  while (is.read(Buff, 4)) {
    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    // Currently, Instruction level simulator doesn't use raw inst code, only
    // use cached instruction class values.
    Mem.writeWord(P, InstVal);
    P += 4;
    CodeSize += 4;
  }
}

void Simulator::execFromDRAMBASE() {
  PC = DRAM_BASE;
  while (auto I = Dec.decode(Mem.readWord(PC))) {
#ifdef DEBUG
    std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
    I->pprint(std::cerr);
#endif
    // TODO: non-machine mode
    if (auto E = I->exec(PC, GPRegs, Mem, States, Mode)) {
      // FIXME: if ecall happens, next address is written, is this correct?
      Address ExceptionPC = PC;
      ModeKind PrevMode = Mode;
      unsigned Cause = *E;
      // FIXME: temporary exit with break
      if (E == Exception::Breakpoint) {
        std::cerr << "breaked\n";
        break;
      }
      if (Mode == ModeKind::Machine) {
        PC = States.read(MTVEC) & (~1);

        States.write(MEPC, ExceptionPC & (~1));

        States.write(MCAUSE, Cause);

        // Machine Trap Value Register
        States.write(MTVAL, trap_val(*E, ExceptionPC, I->getVal()));

        // set MPIE to MIE;
        // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of
        // MSTATUS MPIE: Previous Interrupt-Enable bit for machine mode. 7-th
        // bit of MSTATUS
        CSRVal MSTATUSVal = States.read(MSTATUS);
        bool MIE = (bool)((MSTATUSVal >> 3) & 1);
        States.write(MSTATUS, (MSTATUSVal & 0xffffff7f) | (MIE << 7));

        // Set MIE to 0
        States.write(MSTATUS, MSTATUSVal & 0xfffffff7);

        // set MPP to prev mode
        States.write(MSTATUS, (MSTATUSVal & 0xffffe7ff) | (PrevMode << 11));

      } else {
        assert(false && "Non-Machine mode is unimplemented!");
        return;
      }
    }
#ifdef DEBUG
    std::cerr << "Regs after:\n";
    dumpGPRegs();
#endif
  }
#ifdef DEBUG
  std::cerr << "finish with:\n";
  dumpGPRegs();
#endif
  std::cerr << "stop on no instraction address="
            << "0x" << std::hex << PC << "\n";
}
