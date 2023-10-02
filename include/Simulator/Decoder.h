
#ifndef DECODER_H
#define DECODER_H

#include "Debug.h"
#include "Instructions.h"
#include <memory>

class Decoder {
private:
public:
  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;

  Decoder() {}
  std::unique_ptr<Instruction> decode(unsigned InstVal);
};
#endif
