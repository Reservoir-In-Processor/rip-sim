
#ifndef RIPSIMULATOR_H
#define RIPSIMULATOR_H
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Memory.h"
#include "Registers.h"
#include <memory>
#include <string>
#include <vector>

const unsigned STAGENUM = 5;
const enum STAGES {
  IF,
  DE,
  EX,
  MEM,
  WB,
};

class PipelineStates {
private:
  Reg ForwardVal;
  unsigned MemRPortNum;
  unsigned MemWPortNum;
  std::unique_ptr<Instruction> P[STAGENUM];

public:
  PipelineStates(const PipelineStates &) = delete;
  PipelineStates &operator=(const PipelineStates &) = delete;

  PipelineStates() {}
  void dump() { assert(false && "unimplemented!"); }
};

class RIPSimulator {
private:
  Memory M;
  unsigned CodeSize;
  Address PC;
  unsigned FetchedInst;
  // FIXME: byref?
  GPRegisters GPRegs;
  std::map<Address, std::unique_ptr<Instruction>> PCInstMap;

public:
  RIPSimulator(const RIPSimulator &) = delete;
  RIPSimulator &operator=(const RIPSimulator &) = delete;

  // move this def to .cpp
  RIPSimulator(std::istream &is) : PC(DRAM_BASE) {
    // TODO: parse per 2 bytes for compressed instructions
    char Buff[4];
    // starts from DRAM_BASE
    Address P = DRAM_BASE;
    // TODO: Write once
    while (is.read(Buff, 4)) {
      unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
      // Raw inst
      M.writeWord(P, InstVal);
    }
  }
  GPRegisters &getGPRegs() { return GPRegs; }
  void writeback() {}

  void writeback() {}
  void runFromDRAMBASE() {
    PC = DRAM_BASE;
    unsigned CycleNum = 0;
    // TODO: PCにあるものをPQに読む, 分岐するかどうかはCRegsを見る?
    // fetchが0だったら
    Instruction *I;
    // 、停止条件は、パイプラインが全部null?
    while (true) {
      // forwardingはどうやる？CRegsにいれておけばよい？
      writeback(GPRegs, PQ, CRegs);

      // TODO: write address info to CRegs?
      memoryaccess(M, PQ, CRegs);

      // exec
      exec(PQ, CRegs);

      // InstructionのgetRdを
      // CRegsを、介して、各フェーズはつながる. PQにはいろいろつめる
      // 構造ハザードはうしろが優先なので、うしろからやってもよいかも
      // TODO: バブルはnop? stall Instruction? nullptrでいいか
      // operandの値をｄ取るのはｄ
      // CRegs, string: long, longにして、それでええか？
      // decode
      decode(PQ, CRegs);

      // TODO: it's enough to show only what is fetched.
      fetch(PC, PQ, CRegs);
      // TODO: dump all queued instruction, and CRegs?
      CycleNum++;
    }
  }

  void dumpGPRegs() { GPRegs.dump(); }
  Address &getPC() { return PC; }
};

#endif
