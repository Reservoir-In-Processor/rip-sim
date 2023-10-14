#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include <iostream>

class BranchPredictor {
private:
  int HitNum;
  int MissNum;

public:
  bool PrevPred;
  BranchPredictor(const BranchPredictor &) = delete;
  BranchPredictor &operator=(const BranchPredictor &) = delete;

  BranchPredictor() : HitNum(0), MissNum(0), PrevPred(0) {}

  virtual void Learn(bool &) = 0;
  virtual bool Predict() = 0;

  void StatsUpdate(bool cond) {
    if (cond ^ PrevPred) {
      MissNum++;
#ifdef DEBUG
      std::cerr << "Branch pred: miss "
                << "\n";
#endif
    } else {
      HitNum++;
#ifdef DEBUG
      std::cerr << "Branch pred: hit "
                << "\n";
#endif
    }
  }

  void printStat() {
    std::cerr << "BP accuracy: " << (double)HitNum / (HitNum + MissNum)
              << " (Hit :" << HitNum << ", Miss :" << MissNum << ")"
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
  bool Predict() override {
    PrevPred = NextPred;
    return NextPred;
  }
};

#endif
