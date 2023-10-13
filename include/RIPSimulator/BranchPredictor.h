#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include <iostream>

class BranchPredictor {
private:
  bool DEBranchTaken;
  bool EXBranched;
  int HitNum;
  int MissNum;

public:
  BranchPredictor() : DEBranchTaken(0), EXBranched(0), HitNum(0), MissNum(0) {}

  const bool &getDEBranchTaken() {
    return DEBranchTaken;
  } // TODO: other predictors
  void setDEBranchTaken(bool &V) { DEBranchTaken = V; }

  const bool &getEXBranched() { return EXBranched; }
  void setEXBranched(bool B) { EXBranched = B; }

  void BranchHit() {
    HitNum++;
#ifdef DEBUG
    std::cerr << "Branch pred: hit "
              << "\n";
#endif
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