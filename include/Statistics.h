
#ifndef STATISTICS_H
#define STATISTICS_H
#include <Simulator/Memory.h>
#include <map>

class Statistics {
private:
  /// branch instruction distances' frequency on runtime.
  std::map<unsigned, unsigned> BDists;
  /// The each number of executed instructions on runtime.
  std::map<std::string, unsigned> InstCounts;
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