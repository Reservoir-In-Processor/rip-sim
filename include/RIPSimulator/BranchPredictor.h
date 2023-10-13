#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include <iostream>

class BranchPredictor {
private:
  bool DEBranchPred;
  bool EXBranched;
  int HitNum;
  int MissNum;

public:
  BranchPredictor() : DEBranchPred(0), EXBranched(0), HitNum(0), MissNum(0) {}

  const bool &getDEBranchPred() { return DEBranchPred; }
  void setDEBranchPred(bool &V) { DEBranchPred = V; }

  const bool &getEXBranched() { return EXBranched; }
  void setEXBranched(bool B) { EXBranched = B; }

  void BranchHit() {
    HitNum++;
    std::cerr << "Branch pred: hit "
              << "\n";
  }
  void BranchMiss() {
    MissNum++;
    std::cerr << "Branch pred: miss"
              << "\n";
  }

  void printStat() {
    std::cerr << "BP accuracy: " << (double)HitNum / (HitNum + MissNum)
              << " (Hit :" << HitNum << ", Miss :" << MissNum << ")"
              << "\n";
  }
};

#endif