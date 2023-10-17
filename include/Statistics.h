
#ifndef STATISTICS_H
#define STATISTICS_H
#include <Simulator/Memory.h>
#include <iomanip>
#include <iostream>
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

  void printHistogram() {
    int max_value = 0;
    for (const auto &[key, value] : BDists) {
      if (value > max_value) {
        max_value = value;
      }
    }

    for (const auto &[key, value] : BDists) {
      std::cerr << std::setw(3) << key << " | ";
      for (int i = 0; i < value; ++i) {
        std::cerr << "*";
      }
      std::cerr << " " << value << "\n";
    }
  }
};
#endif