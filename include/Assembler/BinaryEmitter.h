#ifndef BINARYEMITTER_H
#define BINARYEMITTER_H
#include "Assembler/AsmParser.h"
#include <string>

#ifdef DEBUG
#include "Debug.h"
#endif
class BinaryEmitter {
private:
  AsmParser AP;

public:
  BinaryEmitter(const BinaryEmitter &) = delete;
  BinaryEmitter &operator=(const BinaryEmitter &) = delete;

  BinaryEmitter(std::istream &is) : AP(is) {}

  void emitBinary(std::ostream &os);
};

#endif
