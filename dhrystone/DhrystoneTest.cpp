#include "Simulator/Simulator.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

// void Simulator::execDhrystone() {
//   PC = DRAM_BASE;
//   while (auto I = Dec.decode(Mem.readWord(PC))) {
// #ifdef DEBUG
//       // std::cerr << "Inst @ 0x" << std::hex << PC << std::dec << ":\n";
//       // I->pprint(std::cerr);
// #endif
//     // TODO: non-machine mode
//     if (auto E = I->exec(PC, GPRegs, Mem, States, Mode)) {
//       Address ExceptionPC = PC;
//       ModeKind PrevMode = Mode;
//       unsigned Cause = *E;
//       if (Mode == ModeKind::Machine) {
//         PC = States.read(MTVEC) & (~1);

//         States.write(MEPC, ExceptionPC & (~1));

//         States.write(MCAUSE, Cause);

//         // Machine Trap Value Register
//         States.write(MTVAL, trap_val(*E, ExceptionPC, I->getVal()));

//         // set MPIE to MIE;
//         // MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of
//         // MSTATUS MPIE: Previous Interrupt-Enable bit for machine mode. 7-th
//         // bit of MSTATUS
//         CSRVal MSTATUSVal = States.read(MSTATUS);
//         bool MIE = (bool)((MSTATUSVal >> 3) & 1);
//         States.write(MSTATUS, (MSTATUSVal & 0xffffff7f) | (MIE << 7));

//         // Set MIE to 0
//         States.write(MSTATUS, MSTATUSVal & 0xfffffff7);

//         // set MPP to prev mode
//         States.write(MSTATUS, (MSTATUSVal & 0xffffe7ff) | (PrevMode << 11));

//       } else {
//         assert(false && "Non-Machine mode is unimplemented!");
//         return;
//       }
//     }
// #ifdef DEBUG
//     // std::cerr << "Regs after:\n";
//     // dumpGPRegs();
// #endif
//   }
// #ifdef DEBUG
//   std::cerr << "finish with:\n";
//   dumpGPRegs();
// #endif
// }

// TEST(DhrystoneTest, DhryStone) {
//   std::string FileName = "../rip-tests/dhry.bin";
//   auto Files = std::ifstream(FileName);
//   Simulator Sim(Files);
//   Sim.execDhrystone();
//   // const GPRegisters &Res = Sim.getGPRegs();
//   Sim.dumpGPRegs();
// }