
#ifndef STATS_H
#define STATS_H

class Stats {
private:
  ///
  unsigned BDists;
  unsigned InstCounts;
  unsigned ForwardNum;

public:
  Stats(const Stats &) = delete;
  Stats &operator=(const Stats &) = delete;

  Stats() {}
  void printStats();
  void addBDists(unsigned Dist &);
  void addInst(std::string Mnemo &);

  Stats() {}
};
#endif