#ifndef MEMORY_H
#define MEMORY_H
#include "CommonTypes.h"
#include <cstdint>
#include <optional>
#include <vector>

class Memory {
private:
  std::vector<Byte> DRAM;
  Address DRAMSize, DRAMBase;

public:
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;

  Memory(Address _DRAMSize = 1 << 10, Address _DRAMBase = 0x8000)
      : DRAM(_DRAMSize, 0), DRAMSize(_DRAMSize), DRAMBase(_DRAMBase) {}

  void writeByte(Address Ad, Byte Val);
  Byte readByte(Address Ad);
  void writeHalfWord(Address Ad, HalfWord Val);
  HalfWord readHalfWord(Address Ad);
  void writeWord(Address Ad, Word Val);
  Word readWord(Address Ad);
};
#endif
