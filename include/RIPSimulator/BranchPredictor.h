#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include "CommonTypes.h"
#include "Debug.h"
#include <iostream>
#include <map>
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

  virtual void Learn(bool &, Address) = 0;
  virtual bool Predict(Address) = 0;
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
  std::optional<Address> BranchPredPC;
  std::map<Address, bool> BranchHistoryTable;

public:
  OneBitBranchPredictor() : BranchPredictor() {}

  void Learn(bool &cond, Address PC) override { BranchHistoryTable[PC] = cond; }

  bool Predict(Address PC) override {
    if (BranchHistoryTable.count(PC)) {
      return BranchHistoryTable[PC];
    } else {
      return false;
    }
  }

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
