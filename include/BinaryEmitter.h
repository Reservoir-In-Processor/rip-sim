#ifndef BINARYEMITTER_H
#define BINARYEMITTER_H
#include "AsmParser.h"
#include <string>

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