
#ifndef DECODER_H
#define DECODER_H

#include "Instructions.h"
#include <memory>

#ifdef DEBUG
#include "Debug.h"
#endif

class Decoder {
private:
public:
  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;

  Decoder() {}
  std::unique_ptr<Instruction> decode(unsigned InstVal);
};
#endif
