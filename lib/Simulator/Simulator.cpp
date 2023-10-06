
#include "Simulator/Simulator.h"
#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

Simulator::Simulator(std::istream &is)
    : PC(DRAM_BASE), Mode(ModeKind::Machine) {
  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = DRAM_BASE;
  while (is.read(Buff, 4)) {
    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    // Currently, Instruction level simulator doesn't use raw inst code, only
    // use cached instruction class values.
    Mem.writeWord(P, InstVal);
    PCInstMap.insert({P, Dec.decode(InstVal)});
    P += 4;
    CodeSize += 4;
  }
}

void Simulator::execFromDRAMBASE() {
  PC = DRAM_BASE;
  while (auto &I = PCInstMap[PC]) {
    if (auto E = I->exec(PC, GPRegs, Mem, States, Mode)) {
      // FIXME:
      std::cerr << "Exception!\n";
      break;
    }
#ifdef DEBUG
    std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
    I->pprint(std::cerr);
    std::cerr << "Regs after:\n";
    dumpGPRegs();
#endif
  }
  std::cerr << "stop on no instraction address\n";
}
