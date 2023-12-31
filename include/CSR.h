#ifndef CSR_H
#define CSR_H

#include "CommonTypes.h"
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <utility>

enum ModeKind {
  User,
  Supervisor,
  Hypervisor,
  Machine,
};

const unsigned CSR_SIZE = 4096;

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

const std::map<std::string, CSRAddress> CSRMap = {
    {"satp", 0x180},    {"mstatus", 0x300}, {"medeleg", 0x302},
    {"mideleg", 0x303}, {"mie", 0x304},     {"mtvec", 0x305},
    {"mepc", 0x341},    {"mcause", 0x342},  {"mtval", 0x343},
    {"pmpcfg", 0x3a0},  {"pmpaddr", 0x3b0}, {"mhartid", 0xf14},
};

static std::map<CSRAddress, std::string> CSRNames = {
    {0x180, "satp"},    {0x300, "mstatus"}, {0x302, "medeleg"},
    {0x303, "mideleg"}, {0x304, "mie"},     {0x305, "mtvec"},
    {0x341, "mepc"},    {0x342, "mcause"},  {0x343, "mtval"},
    {0x3a0, "pmpcfg"},  {0x3b0, "pmpaddr"}, {0xf14, "mhartid"},
};

// FIXME: for aligning the output text.
static const std::string CSRABI[12] = {
    "  satp  ", " mstatus", " medeleg", " mideleg", "   mie  ", "  mtvec ",
    "  mepc  ", " mcause ", "  mtval ", " pmpcfg ", " pmpaddr", " mhartid",
};

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

  /// get MSTATUS MPREV bit
  /// MPREV: Modify privilege bit. 17-th bit of MSTATUS
  const inline bool getMPREV() { return (read(MSTATUS) & 0xfffdffff) >> 17; }

  /// set MSTATUS MPREV bit
  /// MPREV: Modify privilege bit. 17-th bit of MSTATUS
  void setMPREV(bool Val) {
    write(MSTATUS, (read(MSTATUS) & 0xfffdffff) | ((int)Val << 17));
  }

  /// get MSTATUS MIE bit
  /// MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of MSTATUS
  const inline bool getMIE() { return (read(MSTATUS) & 0xfffffff7) >> 3; }

  /// set MSTATUS MIE bit
  /// MIE: Global Interrupt-Enable bit for machine mode. 3-th bit of MSTATUS
  void setMIE(bool Val) {
    write(MSTATUS, (read(MSTATUS) & 0xfffffff7) | ((int)Val << 3));
  }

  /// get MSTATUS MPIE bit
  /// MPIE: Previous Interrupt-Enable bit for machine mode. 7-th bit of MSTATUS
  const inline bool getMPIE() { return (read(MSTATUS) & 0xffffff7f) >> 7; }

  /// set MSTATUS MPIE bit
  /// MPIE: Previous Interrupt-Enable bit for machine mode. 7-th bit of MSTATUS
  void setMPIE(bool Val) {
    write(MSTATUS, (read(MSTATUS) & 0xffffff7f) | ((int)Val << 7));
  }

  /// get MSTATUS MPP bit
  /// MPP: Machine Privious Priviledge. 12-11-th bits of MSTATUS
  const inline ModeKind getMPP() {
    return (ModeKind)((read(MSTATUS) & 0xffffe7ff) >> 11 & 0b11);
  }

  /// set MSTATUS MPP bit
  /// MPP: Machine Privious Priviledge. 12/11-th bits of MSTATUS
  void setMPP(ModeKind Val) {
    write(MSTATUS, (read(MSTATUS) & 0xffffe7ff) | ((int)Val << 11));
  }

  const void dump() const {
    int i = 0;
    for (auto iter = CSRNames.begin(); iter != CSRNames.end(); ++iter) {
      char ValStr[11];
      std::snprintf(ValStr, sizeof(ValStr), "0x%08x", States[iter->first]);
      std::cerr << 'x' << std::dec << std::left << std::setw(4)
                << std::setfill(' ') << iter->first << "(" << CSRABI[i] << ")"
                << ":=" << std::right << std::setw(12) << std::setfill(' ')
                << ValStr << ", ";

      i++;
      if (i % 4 == 0)
        std::cerr << '\n';
    }

    std::cerr << '\n';
  }
};

#endif