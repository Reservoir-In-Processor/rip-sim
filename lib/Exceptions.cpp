#include "Exceptions.h"
std::uint32_t trap_val(Exception E, std::uint32_t PC, std::uint32_t InstVal) {
  switch (E) {
  case InstructionAddressMisaligned:
  case InstructionAccessFault:
  case Breakpoint:
  case LoadAddressMisaligned:
  case LoadAccessFault:
  case StoreAMOAddressMisaligned:
  case StoreAMOAccessFault:
    return PC;
  case IllegalInstruction:
    return InstVal;
  case InstructionPageFault:
  case LoadPageFault:
  case StoreAMOPageFault:
    assert(false && "unimplemented");
    return InstVal;
  default:
    return 0;
  }
}