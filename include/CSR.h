#ifndef CSR_H
#define CSR_H

#include <cstdint>

enum ModeKind {
  User,
  Supervisor,
  Hypervisor,
  Machine,
};

const unsigned CSR_SIZE = 4096;
// FIXME: is this ok to be 32 ?
using CSRVal = std::int32_t;
using CSRAddress = std::uint16_t;

// Machine Status register
const CSRAddress MSTATUS = 0x300;
// Machine Exception Delegation register
const CSRAddress MEDELEG = 0x302;
// Machine Interrupt Delegation register
const CSRAddress MIDELEG = 0x303;
// Machine Interrupt Enable register
const CSRAddress MIE = 0x304;
// Machine trap-handler base address.
const CSRAddress MTVEC = 0x305;
// Machine Exception Program Counter
const CSRAddress MEPC = 0x341;
// Machine Trap Cause
const CSRAddress MCAUSE = 0x342;

class CSRs {
private:
  CSRVal States[CSR_SIZE];

public:
  CSRs(const CSRs &) = delete;
  CSRs &operator=(const CSRs &) = delete;

  CSRs() : States{0} {}

  void write(CSRAddress Ad, CSRVal Val);
  CSRVal read(CSRAddress Ad);
};

#endif