
#ifndef STATISTICS_H
#define STATISTICS_H
#include <iomanip>
#include <iostream>
#include <map>

class Statistics {
private:
  /// branch instruction distances' frequency on runtime.
  unsigned BDist;
  std::map<unsigned, unsigned> BDists;
  /// The each number of executed instructions on runtime.
  std::map<std::string, unsigned> InstCounts;

public:
  Statistics(const Statistics &) = delete;
  Statistics &operator=(const Statistics &) = delete;

  Statistics() : BDist(0) {}

  void incrementBDist() { BDist++; }
  void addInst(std::string &Mnemo) {
    if (auto IT = InstCounts.find(Mnemo); IT != InstCounts.end()) {
      IT->second++;
    } else {
      InstCounts.insert({Mnemo, 1});
    }
  }

  void addBDistAndReset() {
    if (auto IT = BDists.find(BDist); IT != BDists.end()) {
      IT->second++;
    } else {
      BDists.insert({BDist, 1});
    }
    BDist = 0;
  }

  void printBDists(std::ostream &os) {
    os << "Branches Distances: \n";
    unsigned LFCnt = 0;
    for (const auto &[Dist, Cnt] : BDists) {
      os << std::setfill(' ') << std::setw(3) << Dist << " | ";
      os << Cnt << ", ";
      if ((++LFCnt) % 4 == 0)
        os << "\n";
    }
    os << "\n";
  }

  void printInstCounts(std::ostream &os) {
    os << "Instruction Counts: \n";
    unsigned LFCnt = 0;
    for (const auto &[Mnemo, Cnt] : InstCounts) {
      os << std::setfill(' ') << std::setw(6) << Mnemo << " | ";
      os << Cnt << ", ";
      if ((++LFCnt) % 4 == 0)
        os << "\n";
    }
    os << "\n";
  }

  void printAllStatistics(std::ostream &os) {
    printInstCounts(os);
    printBDists(os);
  }
};
#endif