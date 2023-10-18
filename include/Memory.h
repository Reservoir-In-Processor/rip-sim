#ifndef MEMORY_H
#define MEMORY_H
#include "CommonTypes.h"
#include <cstdint>
#include <vector>

// to avoid sanitizer overhead hell.
#ifdef DEBUG
// 1 MiB
const std::uint64_t DRAM_SIZE = 1LL << 20;
// 1 KiB
// const std::uint64_t DRAM_SIZE = 1 << 10;
// 4GB memory
// const std::uint64_t DRAM_SIZE = 1LL << 32;
#else
// 1 GiB
const std::uint64_t DRAM_SIZE = 1 << 30;
#endif

const Address DRAM_BASE = 0x8000;
// For Dhrystone
// const Address DRAM_BASE = 0x0000;

class Memory {
private:
  std::vector<Byte> DRAM;

public:
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;

  Memory() : DRAM(DRAM_SIZE, 0) {}

  void writeByte(Address Ad, Byte Val);
  Byte readByte(Address Ad);
  void writeHalfWord(Address Ad, HalfWord Val);
  HalfWord readHalfWord(Address Ad);
  void writeWord(Address Ad, Word Val);
  Word readWord(Address Ad);
};
#endif
