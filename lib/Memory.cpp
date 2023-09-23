#include "Simulator/Memory.h"
#include <cassert>
#include <iostream>

void Memory::writeByte(Address Ad, Byte Val){
  Address DRAM_Ad = Ad - DRAM_BASE;
  DRAM[DRAM_Ad] = Val & 0xff;
}

Byte Memory::readByte(Address Ad){
  Address DRAM_Ad = Ad - DRAM_BASE;
  Byte Val = DRAM[DRAM_Ad];
  return Val;
}

void Memory::writeHalfWord(Address Ad, HalfWord Val){
  Address DRAM_Ad = Ad - DRAM_BASE;
  DRAM[DRAM_Ad] = Val & 0xff;
  DRAM[DRAM_Ad + 1] = (Val >> 8) & 0xff;
}

HalfWord Memory::readHalfWord(Address Ad){
  Address DRAM_Ad = Ad - DRAM_BASE;
  HalfWord Val = DRAM[DRAM_Ad] + (DRAM[DRAM_Ad + 1] << 8);
  return Val;
}

void Memory::writeWord(Address Ad, Word Val) {
  Address DRAM_Ad = Ad - DRAM_BASE;
  DRAM[DRAM_Ad] = Val & 0xff;
  DRAM[DRAM_Ad + 1] = (Val >> 8) & 0xff;
  DRAM[DRAM_Ad + 2] = (Val >> 16) & 0xff;
  DRAM[DRAM_Ad + 3] = (Val >> 24) & 0xff;
}

Word Memory::readWord(Address Ad) {
  Address DRAM_Ad = Ad - DRAM_BASE;
  Word Val = DRAM[DRAM_Ad] + (DRAM[DRAM_Ad + 1] << 8) +
             (DRAM[DRAM_Ad + 2] << 16) + (DRAM[DRAM_Ad + 3] << 24);
  return Val;
}
