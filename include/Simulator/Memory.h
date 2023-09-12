#ifndef MEMORY_H
#define MEMORY_H
class Memory {
private:
public:
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;

  Memory() {}
};
#endif
