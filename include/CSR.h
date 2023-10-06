#ifndef CSR_H
#define CSR_H

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <utility>

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

// Supervisor address translation and protection
const CSRAddress SATP = 0x180;

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
// Machine Trap Value register. bad address or instructions
const CSRAddress MTVAL = 0x343;

/// Physical memory protection configuration.
const CSRAddress PMPCFG0 = 0x3a0;
/// Physical memory protection address register.
const CSRAddress PMPADDR0 = 0x3b0;

// Machine-level CSR addresses
// Machine information registers
// Hardware thread ID.
const CSRAddress MHARTID = 0xf14;

class CSRs {
private:
  CSRVal States[CSR_SIZE];

public:
  CSRs(const CSRs &) = delete;
  CSRs &operator=(const CSRs &) = delete;

  CSRs() : States{0} {}

  const CSRVal &operator[](CSRAddress index) const {
    assert(index < CSR_SIZE && "Index out of bounds");
    return States[index];
  }

  // TODO: it's better to be string : CSRVal
  CSRs(std::initializer_list<std::pair<CSRAddress, CSRVal>> init_list)
      : States{0} {
    for (const auto &p : init_list) {
      States[p.first] = p.second;
    }
  }

  void write(CSRAddress Ad, CSRVal Val);
  CSRVal read(CSRAddress Ad);
};

#endif