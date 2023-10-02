
#include "Simulator/Simulator.h"
#ifdef DEBUG
#include "Debug.h"
#endif // DEBUG

Simulator::Simulator(std::istream &is) : PC(DRAM_BASE) {
  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = DRAM_BASE;
  while (is.read(Buff, 4)) {

    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    // Currently, Instruction level simulator doesn't use raw inst code, only
    // use cached instruction class values.
    M.writeWord(P, InstVal);
    PCInstMap.insert({P, Dec.decode(InstVal)});
    P += 4;
    CodeSize += 4;
  }
}