#ifndef CUSTOMREGISTERS_H
#define CUSTOMREGISTERS_H
class CustomRegisters {
private:
public:
  CustomRegisters(const CustomRegisters &) = delete;
  CustomRegisters &operator=(const CustomRegisters &) = delete;

  CustomRegisters() {}
  void dump() {}
};

#endif
