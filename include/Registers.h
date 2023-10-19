#ifndef REGISTERS_H
#define REGISTERS_H

#include "CommonTypes.h"
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
const std::map<std::string, std::bitset<5>> GPRegMap = {
    {"x0", 0},   {"zero", 0}, {"x1", 1},   {"ra", 1},   {"x2", 2},  {"sp", 2},
    {"x3", 3},   {"gp", 3},   {"x4", 4},   {"tp", 4},   {"x5", 5},  {"t0", 5},
    {"x6", 6},   {"t1", 6},   {"x7", 7},   {"t2", 7},   {"x8", 8},  {"s0", 8},
    {"fp", 8},   {"x9", 9},   {"s1", 9},   {"x10", 10}, {"a0", 10}, {"x11", 11},
    {"a1", 11},  {"x12", 12}, {"a2", 12},  {"x13", 13}, {"a3", 13}, {"x14", 14},
    {"a4", 14},  {"x15", 15}, {"a5", 15},  {"x16", 16}, {"a6", 16}, {"x17", 17},
    {"a7", 17},  {"x18", 18}, {"s2", 18},  {"x19", 19}, {"s3", 19}, {"x20", 20},
    {"s4", 20},  {"x21", 21}, {"s5", 21},  {"x22", 22}, {"s6", 22}, {"x23", 23},
    {"s7", 23},  {"x24", 24}, {"s8", 24},  {"x25", 25}, {"s9", 25}, {"x26", 26},
    {"s10", 26}, {"x27", 27}, {"s11", 27}, {"x28", 28}, {"t3", 28}, {"x29", 29},
    {"t4", 29},  {"x30", 30}, {"t5", 30},  {"x31", 31}, {"t6", 31}};

static const std::string ABI[32] = {
    "zero", " ra ", " sp ", " gp ", " tp ", " t0 ", " t1 ", " t2 ",
    " s0 ", " s1 ", " a0 ", " a1 ", " a2 ", " a3 ", " a4 ", " a5 ",
    " a6 ", " a7 ", " s2 ", " s3 ", " s4 ", " s5 ", " s6 ", " s7 ",
    " s8 ", " s9 ", " s10", " s11", " t3 ", " t4 ", " t5 ", " t6 ",
};

const unsigned RegNum = 32;
class GPRegisters {
private:
  RegVal Regs[RegNum];

public:
  GPRegisters(const GPRegisters &) = delete;
  GPRegisters &operator=(const GPRegisters &) = delete;

  GPRegisters(std::optional<Address> DRAMSize = 1 << 10,
              std::optional<Address> DRAMBase = 0x8000,
              std::optional<Address> SPIValue = std::nullopt)
      : Regs{0} {
    // The stack pointer is set in the default maximum mamory size + the start
    // address of dram.
    if (SPIValue)
      Regs[2] = *SPIValue;
    else
      Regs[2] = (RegVal)*DRAMSize + (RegVal)*DRAMBase;
  }
  GPRegisters(std::initializer_list<std::pair<unsigned, RegVal>> init_list,
              std::optional<Address> DRAMSize = 1 << 10,
              std::optional<Address> DRAMBase = 0x8000,
              std::optional<Address> SPIValue = std::nullopt)
      : Regs{0} {
    for (const auto &p : init_list) {
      assert(p.first < RegNum && "Register index out of bounds.");
      assert(p.first != 0 && "The zero register x0 = zero is set 0.");
      assert(p.first != 2 &&
             "The stack pointer sp = x2 is set for end of the dram.");
      Regs[p.first] = p.second;
    }
    // The stack pointer is set in the default maximum mamory size + the start
    // address of dram.
    // FIXME: it may be enough to clamp with big value like 1 << 30.
    // value)
    if (SPIValue)
      Regs[2] = *SPIValue;
    else
      Regs[2] = (RegVal)*DRAMSize + (RegVal)*DRAMBase;
  }

  void write(unsigned Id, RegVal Val) {
    assert(Id < RegNum && "Index out of bounds");
    if (Id == 0) {
      return;
    }
    Regs[Id] = Val;
  }

  const RegVal &operator[](unsigned index) const {
    assert(index < RegNum && "Index out of bounds");
    return Regs[index];
  }

  const RegVal &operator[](std::string name) const {
    auto IT = GPRegMap.find(name);
    assert(IT != GPRegMap.end() && "No such registers.");
    return Regs[(IT->second).to_ulong()];
  }

  const void dump() const {
    for (unsigned i = 0; i < 32; ++i) {
      char ValStr[11];
      std::snprintf(ValStr, sizeof(ValStr), "0x%08x", Regs[i]);
      std::cerr << 'x' << std::dec << std::left << std::setw(2)
                << std::setfill(' ') << i << "(" << ABI[i] << ")"
                << ":=" << std::right << std::setw(12) << std::setfill(' ')
                << ValStr << ", ";
      if (i % 4 == 3)
        std::cerr << '\n';
    }
    std::cerr << '\n';
  }
};

#endif
