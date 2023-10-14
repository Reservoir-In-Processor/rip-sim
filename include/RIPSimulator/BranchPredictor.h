#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include <iostream>

class BranchPredictor {
private:
  int StatsHitNum;
  int StatsMissNum;

public:
  bool PrevPredict;
  BranchPredictor(const BranchPredictor &) = delete;
  BranchPredictor &operator=(const BranchPredictor &) = delete;

  BranchPredictor() : StatsHitNum(0), StatsMissNum(0) {}

  virtual void Learn(bool &) = 0;
  virtual bool Predict() = 0;

  void StatsUpdate(bool cond) {
    if (cond ^ PrevPredict) {
      StatsMissNum++;
#ifdef DEBUG
      std::cerr << "Branch pred: miss "
                << "\n";
#endif
    } else {
      StatsHitNum++;
#ifdef DEBUG
      std::cerr << "Branch pred: hit "
                << "\n";
#endif
    }
  }

  void printStat() {
    std::cerr << "BP accuracy: "
              << (double)StatsHitNum / (StatsHitNum + StatsMissNum)
              << " (Hit :" << StatsHitNum << ", Miss :" << StatsMissNum << ")"
              << "\n";
  }

  virtual ~BranchPredictor() {}
};

class OneBitBranchPredictor : public BranchPredictor {
private:
  /// Result of Prediction
  bool NextPred;

public:
  OneBitBranchPredictor() : BranchPredictor(), NextPred(0) {}

  void Learn(bool &cond) override { NextPred = cond; }
  bool Predict() override { return NextPred; }
};

#endif
