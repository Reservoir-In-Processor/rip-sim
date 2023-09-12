#ifndef CPU_H
#define CPU_H
class CPU {
private:
public:
  CPU(const CPU &) = delete;
  CPU &operator=(const CPU &) = delete;

  CPU() {}
};
#endif
