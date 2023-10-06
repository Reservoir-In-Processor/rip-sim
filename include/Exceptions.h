#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <cassert>
#include <cstdint>
// FIXME: explicitly write the number?
enum Exception {
  InstructionAddressMisaligned,
  InstructionAccessFault,
  IllegalInstruction,
  Breakpoint,
  LoadAddressMisaligned,
  LoadAccessFault,
  StoreAMOAddressMisaligned,
  StoreAMOAccessFault,
  EnvironmentCallFromUMode,
  EnvironmentCallFromSMode,
  R0,
  EnvironmentCallFromMMode,
  InstructionPageFault,
  LoadPageFault,
  R1,
  StoreAMOPageFault,
};

extern std::uint32_t trap_val(Exception E, std::uint32_t PC,
                              std::uint32_t InstVal);
#endif
