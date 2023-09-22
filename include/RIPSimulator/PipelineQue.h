#ifndef PIPELINEQUE_H
#define PIPELINEQUE_H
#include "Instructions.h"
#include <deque>
#include <memory>

enum class Pipeline {
  IF,
  DE,
  EX,
  MEM,
  WB,
};

class PipelineQue {
private:
  std::shared_ptr<Instruction> InstQue[5];

public:
  PipelineQue(const PipelineQue &) = delete;
  PipelineQue &operator=(const PipelineQue &) = delete;

  PipelineQue() {}
};
#endif
