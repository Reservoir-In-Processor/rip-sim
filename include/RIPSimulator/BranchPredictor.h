#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H
#include "CommonTypes.h"
#include "Debug.h"
#include "Interactive.h"
#include <cmath>
#include <iostream>
#include <map>
#include <optional>

unsigned int getLowerNBits(unsigned int value, unsigned int N) {
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
    DEBUG_ONLY(std::cerr << "StatsUpdate: " << Cond << " " << Pred << "\n");
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
  unsigned int BHTIndexWidth = 5; // FIXME: should be argument

public:
  OneBitBranchPredictor() : BranchPredictor() {}

  void Learn(bool &cond, const Address &PC) override {
    BranchHistoryTable[getLowerNBits(PC >> 2, BHTIndexWidth)] = cond;
  }

  bool Predict(const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth);
    if (BranchHistoryTable.count(BHTIndex)) {

      DEBUG_ONLY(std::cerr << "BHT[INDEX] " << BranchHistoryTable[BHTIndex]
                           << "\n");
      DEBUG_ONLY(std::cerr << "INDEX: " << BHTIndex << "\n");
      return BranchHistoryTable[BHTIndex];
    } else {
      DEBUG_ONLY(std::cerr << "None"
                           << "\n");
      return false;
    }
  }
};

class TwoBitBranchPredictor : public BranchPredictor {
private:
  std::map<Address, int> BranchHistoryTable;
  unsigned int BHTIndexWidth = 5; // FIXME: should be argument

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
      BranchHistoryTable[BHTIndex] = 0; // FIXME: check init
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

class GshareBranchPredictor : public BranchPredictor {
private:
  std::map<Address, int> BranchHistoryTable;
  unsigned int BHTIndexWidth = 10; // FIXME: should be argument
  unsigned BranchHistory = 0;

public:
  GshareBranchPredictor() : BranchPredictor() {}

  int getBranchHistory() { return BranchHistory; }
  int getBHTIndex(Address PC) {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth) ^ BranchHistory;
    return BHTIndex;
  }

  void Learn(bool &cond, const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth) ^ BranchHistory;

    if (BranchHistoryTable.count(BHTIndex)) {
      if (cond) {
        BranchHistoryTable[BHTIndex]++;
      } else {
        BranchHistoryTable[BHTIndex]--;
      }
    } else {
      BranchHistoryTable[BHTIndex] = 0; // FIXME: check init
    }
    BranchHistoryTable[BHTIndex] =
        std::clamp(BranchHistoryTable[BHTIndex], 0, 3);
    DEBUG_ONLY(std::cerr << std::hex << "BHT [ " << BHTIndex
                         << "]: " << BranchHistoryTable[BHTIndex] << "\n");

    BranchHistory =
        (BranchHistory << 1) +
        cond; // update of Branch Hitory FIXME: should be in Predict?
    BranchHistory %= 1 << BHTIndexWidth;

    DEBUG_ONLY(std::cerr << std::hex << "Branch history: 0x" << BranchHistory
                         << "\n");
  }

  bool Predict(const Address &PC) override {
    unsigned BHTIndex = getLowerNBits(PC >> 2, BHTIndexWidth) ^ BranchHistory;
    return BranchHistoryTable.count(BHTIndex) &&
           BranchHistoryTable[BHTIndex] >= 2;
  }
};

class PerceptronBranchPredictor : public BranchPredictor {
private:
  std::map<Address, int> BranchHistoryTable;
  const int PerceptronIndexWidth =
      10;                       // DynamicBranchPredictionWithPerceptronsでのN
  const int HistoryLength = 10; // DynamicBranchPredictionWithPerceptronsでのn
  unsigned int Theta;
  unsigned int BranchHistory;
  signed int **WeightArray; // {PerceptronIndexWidth, HistoryLength}
  signed int y;

public:
  PerceptronBranchPredictor() : BranchPredictor() {
    // ここに初期化コードを追加します。
    Theta = 1.93 * HistoryLength + 14;
    BranchHistory = 0;
    // WeightArrayの動的な初期化
    WeightArray = new signed int *[2 << PerceptronIndexWidth];
    for (int i = 0; i < (2 << PerceptronIndexWidth); ++i) {
      WeightArray[i] = new signed int[HistoryLength];
    }

    // WeightArrayの初期化
    for (int i = 0; i < (2 << PerceptronIndexWidth); ++i) {
      for (int j = 0; j < HistoryLength; ++j) {
        WeightArray[i][j] = 0; // 初期値を設定する
      }
    }
  }

  ~PerceptronBranchPredictor() {
    for (int i = 0; i < PerceptronIndexWidth; ++i) {
      delete[] WeightArray[i];
    }
    delete[] WeightArray;
  }

  int getBranchHistory() { return BranchHistory; }
  int getBHTIndex(Address PC) {
    unsigned BHTIndex = getLowerNBits(PC >> 2, PerceptronIndexWidth);
    return BHTIndex;
  }

  void Learn(bool &cond, const Address &PC) override {
    unsigned PerceptronIndex = getLowerNBits(PC >> 2, PerceptronIndexWidth);
    int tmpBranchHistory = BranchHistory;

    if ((((y >= 0) ? 1 : -1) != cond) || (abs(y) <= Theta)) {
      // update bias
      signed int w = WeightArray[PerceptronIndex][0];
      w = w + cond;
      WeightArray[PerceptronIndex][0] = w;

      // update weights
      for (int i = 1; i < HistoryLength; i++) {
        signed int w = WeightArray[PerceptronIndex][i];
        w = w + cond * (tmpBranchHistory % 2);
        WeightArray[PerceptronIndex][i] = w;
        tmpBranchHistory = tmpBranchHistory >> 1;
      }
    }

    BranchHistory = (BranchHistory << 1) + cond; // update of Branch Hitory
    BranchHistory %= 1 << HistoryLength;

    DEBUG_ONLY(std::cerr << std::hex << "Branch history: 0x" << BranchHistory
                         << "\n");
  }

  bool Predict(const Address &PC) override {
    unsigned PerceptronIndex = getLowerNBits(PC >> 2, PerceptronIndexWidth);
    // TODO: ADD Perceptron prediction
    int tmpBranchHistory = BranchHistory;

    y = WeightArray[PerceptronIndex][0];
    DEBUG_ONLY(std::cerr << std::dec << "FOR DEBUG (BIAS) ====" << 1 << " " << y
                         << " " << std::bitset<10>(tmpBranchHistory)
                         << "====\n");

    for (int i = 1; i < HistoryLength; i++) {
      signed int w = WeightArray[PerceptronIndex][i];

      y = y + w * (tmpBranchHistory % 2);

      DEBUG_ONLY(std::cerr << std::dec << "FOR DEBUG ====" << w << " " << y
                           << " " << std::bitset<10>(tmpBranchHistory)
                           << "====\n");

      tmpBranchHistory = tmpBranchHistory >> 1;
    }

    if (y <= 0) {
      return 0;
    } else {
      return 1;
    }
  }
};

// FIXME: add test
class InteractiveBranchPredictor : public BranchPredictor {
private:
  std::ostream &os;
  std::istream &is;

public:
  InteractiveBranchPredictor(std::ostream &_os, std::istream &_is)
      : BranchPredictor(), os(_os), is(_is) {}

  void Learn(bool &cond, const Address &PC) override {

    nlohmann::json JLearn;
    JLearn["Kind"] = JSONKindToString(JSONKind::Learn);
    JLearn["Cond"] = cond;
    JLearn["PC"] = PC;
    os << JLearn.dump() << std::endl;
  }

  bool Predict(const Address &PC) override {
    nlohmann::json JPred, JPredRes;

    JPred["Kind"] = JSONKindToString(JSONKind::Predict);
    JPred["PrevPred"] = getPrevPred();
    os << JPred.dump() << std::endl;
    std::string ResStr;
    // FIXME: remove whatever things
    while (is >> ResStr && ResStr.find("PredRes") == std::string::npos) {
    }
    JPredRes = nlohmann::json::parse(ResStr);

    if (JPredRes.is_object() && JPredRes.size() == 1) {
      std::string key = JPredRes.begin().key();

      if (JPredRes[key].is_boolean()) {
        return JPredRes[key].get<bool>();
      }
    }
    std::cerr << "Interactive Predict result should be {\"PredRes\": bool}"
              << std::endl;
    assert(false && "Interactive Predict result should be {\"PredRes\": bool}");
    return false;
  }
};
#endif
