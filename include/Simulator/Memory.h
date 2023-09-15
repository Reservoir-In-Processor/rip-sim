#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <vector>

// to avoid sanitizer overhead hell.
#ifdef DEBUG
// 1 MiB
// const std::uint64_t DRAM_SIZE = 1 << 20;
// 1 KiB
const std::uint64_t DRAM_SIZE = 1 << 10;
#else
// 1 GiB
const std::uint64_t DRAM_SIZE = 1 << 30;
#endif

using Address = std::uint64_t;
const Address DRAM_BASE = 0x8000;

using Byte = std::uint8_t;
using Word = std::uint32_t;

class Memory {
private:
  std::vector<Byte> DRAM;

public:
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;

  Memory() : DRAM(DRAM_SIZE, 0) {}

  void writeWord(Address Ad, Word Val);
  Word readWord(Address Ad);
};
#endif
