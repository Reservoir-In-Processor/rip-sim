#include "Memory.h"
#include <cassert>
#include <iostream>

void Memory::writeByte(Address Ad, Byte Val){
  // FIXME: raise access fault for address like x < DRAMSize ?
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  DRAM[DRAMAd] = Val & 0xff;
}

Byte Memory::readByte(Address Ad){
  // FIXME: raise access fault for address like x < DRAMSize ?
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  Byte Val = DRAM[DRAMAd];
  return Val;
}

void Memory::writeHalfWord(Address Ad, HalfWord Val){
  // FIXME: raise access fault for address like x < DRAMSize ?
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  DRAM[DRAMAd] = Val & 0xff;
  DRAM[DRAMAd + 1] = (Val >> 8) & 0xff;
}

HalfWord Memory::readHalfWord(Address Ad){
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  HalfWord Val = DRAM[DRAMAd] + (DRAM[DRAMAd + 1] << 8);
  return Val;
}

void Memory::writeWord(Address Ad, Word Val) {
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  DRAM[DRAMAd] = Val & 0xff;
  DRAM[DRAMAd + 1] = (Val >> 8) & 0xff;
  DRAM[DRAMAd + 2] = (Val >> 16) & 0xff;
  DRAM[DRAMAd + 3] = (Val >> 24) & 0xff;
}

Word Memory::readWord(Address Ad) {
  if (Ad < DRAMBase)
    assert(false && "invalid access to smaller address than DRAMBase");
  Address DRAMAd = Ad - DRAMBase;
  if (DRAMSize < DRAMAd)
    assert(false && "invalid access to bigger address than DRAMSize");
  Word Val = DRAM[DRAMAd] + (DRAM[DRAMAd + 1] << 8) + (DRAM[DRAMAd + 2] << 16) +
             (DRAM[DRAMAd + 3] << 24);
  return Val;
}
