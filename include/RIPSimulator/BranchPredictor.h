#ifndef BRANCHPREDICTOR_H
#define BRANCHPREDICTOR_H

class BranchPredictor {
private:
  bool DEBranchPred;
  bool EXBranched;

public:
  BranchPredictor() : DEBranchPred(0), EXBranched(0) {}

  const bool &getDEBranchPred() { return DEBranchPred; }
  void setDEBranchPred(bool &V) { DEBranchPred = V; }

  const bool &getEXBranched() { return EXBranched; }
  void setEXBranched(bool B) { EXBranched = B; }
};

#endif