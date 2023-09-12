#ifdef DEBUG

#include <iomanip>
#include <iostream>
#include <vector>
void debugInst(const std::vector<std::string> Toks, unsigned Val) {

  bool first = true;
  for (auto &s : Toks) {
    if (!first)
      std::cerr << ", ";
    std::cerr << s;
    if (first)
      first = false;
  }
  std::cerr << "\n";

  std::bitset<32> binaryValue(Val);
  std::cerr << "Binary(BE): ";
  for (int i = 0; i < 32; ++i) {
    if (i == 7 || i == 12 || i == 17 || i == 20 || i == 25)
      std::cerr << ' ';
    std::cerr << ((binaryValue >> (31 - i)).to_ulong() & 1);
  }

  std::cerr << "\n";
  std::cerr << "Hex(LE): ";
  for (int i = 0; i < sizeof(Val); ++i) {
    unsigned char byte = (Val >> (i * 8)) & 0xFF;
    std::cerr << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(byte) << ' ';
  }
  std::cerr << "\n\n";
}

#endif
