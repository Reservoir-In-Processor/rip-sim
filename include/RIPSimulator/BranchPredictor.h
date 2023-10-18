#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include "Debug.h"
#include "Memory.h"
#include <iostream>
#include <optional>

class BranchPredictor {
private:
  int HitNum;
  int MissNum;
  bool PrevPred;

public:
  BranchPredictor(const BranchPredictor &) = delete;
  BranchPredictor &operator=(const BranchPredictor &) = delete;

  BranchPredictor() : HitNum(0), MissNum(0), PrevPred(0){};

  virtual void Learn(bool &) = 0;
  virtual bool Predict() = 0;
  virtual void setBranchPredPC(const Address &) = 0;
  virtual const std::optional<Address> takeBranchPredPC() = 0;

  void setPrevPred(bool Pred) { PrevPred = Pred; }
  bool getPrevPred() { return PrevPred; }

  void StatsUpdate(bool Cond, bool Pred) {
    if (Cond ^ Pred) {
      MissNum++;
      DEBUG_ONLY(std::cerr << "Branch pred: miss "
                           << "\n");
    } else {
      HitNum++;
      DEBUG_ONLY(std::cerr << "Branch pred: hit "
                           << "\n");
    }
  }

  void printStat() {
    std::cerr << " BP accuracy: " << (double)HitNum / (HitNum + MissNum)
              << " (Hit :" << HitNum << ", Miss :" << MissNum << ")"
              << "\n";
  }

  virtual ~BranchPredictor() {}
};

class OneBitBranchPredictor : public BranchPredictor {
private:
  bool BranchHistory;
  std::optional<Address> BranchPredPC;

public:
  OneBitBranchPredictor() : BranchPredictor(), BranchHistory(0) {}

  void Learn(bool &cond) override { BranchHistory = cond; }
  bool Predict() override { return BranchHistory; }

  const std::optional<Address> takeBranchPredPC() override {
    if (BranchPredPC) {
      Address BPPC = *BranchPredPC;
      BranchPredPC = std::nullopt;
      return std::make_optional(BPPC);
    } else {
      return std::nullopt;
    }
  }
  void setBranchPredPC(const Address &BPPC) override { BranchPredPC = BPPC; }
};

#endif
