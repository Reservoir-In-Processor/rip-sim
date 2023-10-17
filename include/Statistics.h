
#ifndef STATISTICS_H
#define STATISTICS_H
#include <Simulator/Memory.h>

class Statistics {
private:
  /// Tach executed branch instructions distances on runtime.
  unsigned BDists;
  /// The each number of executed instructions on runtime.
  unsigned InstCounts;
  /// The number of forwarding.
  unsigned ForwardNum;
  /// The maximum address which store/load accessed.
  Address MaxMemoryAddress;
  /// The minimum address which store/load accessed.
  Address MaxMemoryAddress;

public:
  Statistics(const Statistics &) = delete;
  Statistics &operator=(const Statistics &) = delete;

  Statistics() {}
  void printStatistics();
  void addBDists(unsigned Dist &);
  void addInst(std::string Mnemo &);

  Statistics() {}
};
#endif