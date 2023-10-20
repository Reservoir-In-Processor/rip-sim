#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include "Debug.h"
#include "Simulator/Memory.h"
#include <iostream>
#include <map>
#include <optional>

unsigned int getLowerNBits(unsigned int value, int N) {
  return value & ((1 << N) - 1);
}

class BranchPredictor {
private:
  int HitNum;
  int MissNum;
  bool PrevPred;
  std::optional<Address> BranchPredPC;

public:
  BranchPredictor(const BranchPredictor &) = delete;
  BranchPredictor &operator=(const BranchPredictor &) = delete;

  BranchPredictor() : HitNum(0), MissNum(0), PrevPred(0){};

  virtual void Learn(bool &, const Address &) = 0;
  virtual bool Predict(const Address &) = 0;

  void setBranchPredPC(const Address &BPPC) { BranchPredPC = BPPC; }

  const std::optional<Address> takeBranchPredPC() {
    if (BranchPredPC) {
      Address BPPC = *BranchPredPC;
      BranchPredPC = std::nullopt;
      return std::make_optional(BPPC);
    } else {
      return std::nullopt;
    }
  }

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
  std::map<Address, bool> BranchHistoryTable;
  int BHTIndexWidth = 5; // FIXME: should be argument

public:
  OneBitBranchPredictor() : BranchPredictor() {}

  void Learn(bool &cond, const Address &PC) override {
    BranchHistoryTable[getLowerNBits(PC >> 2, BHTIndexWidth)] = cond;
  }

  bool Predict(const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth);
    if (BranchHistoryTable.count(BHTIndex)) {
      return BranchHistoryTable[BHTIndex];
    } else {
      return false;
    }
  }
};

class TwoBitBranchPredictor : public BranchPredictor {
private:
  std::map<Address, int> BranchHistoryTable;
  int BHTIndexWidth = 5; // FIXME: should be argument

public:
  TwoBitBranchPredictor() : BranchPredictor() {}

  void Learn(bool &cond, const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth);

    if (BranchHistoryTable.count(BHTIndex)) {
      if (cond) {
        BranchHistoryTable[BHTIndex]++;
      } else {
        BranchHistoryTable[BHTIndex]--;
      }
    } else {
      BranchHistoryTable[BHTIndex] = 0;
    }
    BranchHistoryTable[BHTIndex] =
        std::clamp(BranchHistoryTable[BHTIndex], 0, 3);
    DEBUG_ONLY(std::cerr << std::hex << "BHT [ " << BHTIndex
                         << "]: " << BranchHistoryTable[BHTIndex] << "\n";);
  }

  bool Predict(const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth);
    return BranchHistoryTable.count(BHTIndex) &&
           BranchHistoryTable[BHTIndex] >= 2;
  }
};

#endif
