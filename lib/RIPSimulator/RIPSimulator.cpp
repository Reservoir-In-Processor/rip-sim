#include "RIPSimulator/RIPSimulator.h"
#include <set>
#include <cmath>

namespace {
  // BitWidth < 32
 int signExtend(const unsigned Imm, unsigned BitWidth) {
  if ((Imm >> (BitWidth - 1)) & 1) {
    return (~0 ^ ((int)(pow(2, BitWidth) - 1)) ) | (signed)Imm;
  }else {
    return Imm;
  }
}

} // namespace

void PipelineStates::dump(){
    // TODO: dump stall,
    for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
      std::cerr << StageNames[(STAGES)Stage] << ": ";
      if (Insts[Stage] != nullptr) {
        Insts[Stage]->mprint(std::cerr);
        std::cerr << ", ";
      } else
        std::cerr << "Bubble, ";

      // TODO: dump stage specific info.
      switch (Stage) {
      case STAGES::DE:
        std::cerr << "DERs1Val=" << DERs1Val << ", ";
        std::cerr << "DERs2Val=" << DERs2Val << ", ";
        std::cerr << "DEImmVal=" << DEImmVal << "\n";
        break;
      case STAGES::EX:
        std::cerr << "EXRdVal=" << EXRdVal << "\n";
        break;
      case STAGES::MA:
        std::cerr << "MARdVal=" << MARdVal << "\n";
        break;
      default:
        std::cerr << "\n";
        break;
      }
    }
  }

RIPSimulator::RIPSimulator(std::istream &is) :  PC(DRAM_BASE), CycleNum(0) {
  // TODO: parse per 2 bytes for compressed instructions
  char Buff[4];
  // starts from DRAM_BASE
  Address P = DRAM_BASE;
  // Load binary into memory
  while (is.read(Buff, 4)) {
    unsigned InstVal = *(reinterpret_cast<unsigned *>(Buff));
    Mem.writeWord(P, InstVal);
    P += 4;
    CodeSize += 4;
  }
}

void RIPSimulator::writeback(GPRegisters &, PipelineStates &) {
  const auto &Inst = PS[STAGES::WB];
  GPRegs.write(Inst->getRd(), PS.getMARdVal());
}
void RIPSimulator::memoryaccess(Memory &, PipelineStates &) {
  // const auto &Inst = PS[STAGES::MA];
  const RegVal MARdVal = PS.getEXRdVal();

  PS.setMARdVal(MARdVal);
}

const std::set<std::string> INVALID_EX = {"lbu",  "lhu",   "beq",   "blt",
                                          "bge",  "bltu",  "bgeu",  "jal",
                                          "jalr", "ecall", "ebreak"};

void RIPSimulator::exec(PipelineStates &) {
  const auto &Inst = PS[STAGES::EX];
  // TODO: calc
  RegVal Res = 0;
  std::string Mnemo = Inst->getMnemo();
  if (Mnemo == "add") {
    Res = PS.getDERs1Val() + PS.getDERs2Val();
  } else if (Mnemo == "addi") {
    Res = PS.getDERs1Val() + PS.getDEImmVal();
  }else {
    assert(false && "unimplemented!");
  }

  if (INVALID_EX.find(Inst->getMnemo()) != INVALID_EX.end()) {
    // FIXME: invalide itself, is this right?
    PS.setInvalid(EX);
    PS.setInvalid(DE);
    PS.setInvalid(IF);
    // TODO: reset PC
  }
  // TODO: implemnet Branched PC
  if (false)
    PS.setBranchPC(PC + 4);

  PS.setEXRdVal(Res);
}

// register access shuold be done in this phase, exec shuoldn't access GPRegs
// directly.
void RIPSimulator::decode(GPRegisters &, PipelineStates &) {
  // FIXME: PS indexing seems not consistent(it's not only instructions)
  const auto &Inst = PS[STAGES::DE];

  // TODO:
  // FIXME: this is incorrect, because U-type and J-type instr's Rs1 or Rs2 is a
  // part of immediate. Check if the inst is one of those.

  // Register access on Rs1
  if (PS[STAGES::MA] && Inst->getRs1() == PS[STAGES::MA]->getRd()) {
    // MA forward
    PS.setDERs1Val(PS.getMARdVal());
    std::cerr << "Forwarding from MA\n";
  } else if (PS[STAGES::EX] && Inst->getRs1() == PS[STAGES::EX]->getRd()) {
    // EX forward
    PS.setDERs1Val(PS.getEXRdVal());
    std::cerr << "Forwarding from EX\n";
  } else {
    PS.setDERs1Val(GPRegs[Inst->getRs1()]);
  }

  // Register access on Rs2
  if (PS[STAGES::MA] && Inst->getRs2() == PS[STAGES::MA]->getRd()) {
    PS.setDERs2Val(PS.getMARdVal());
    std::cerr << "Forwarding from MA\n";
  } else if (PS[STAGES::EX] && Inst->getRs2() == PS[STAGES::EX]->getRd()) {
    PS.setDERs2Val(PS.getEXRdVal());
    std::cerr << "Forwarding from EX\n";
  } else {
    PS.setDERs2Val(GPRegs[Inst->getRs2()]);
  }

  // TODO: more variants
  if (ITypeKinds.find(Inst->getMnemo()) != ITypeKinds.end())
    PS.setDEImmVal(signExtend(Inst->getImm(), 12));
  else 
    assert(false && "unimplemented!");
  // TODO: stall 1 cycle if the inst is load;
}
void RIPSimulator::fetch(Memory &, PipelineStates &) {
  // const auto &Inst = PS[STAGES::IF];
  // FIXME: how and when can I change PC?
}

void RIPSimulator::runFromDRAMBASE() {
  PC = DRAM_BASE;

  while (true) {
    auto InstPtr = Dec.decode(Mem.readWord(PC));
    if (InstPtr) {
      PC += 4;
    }
    PS.push(std::move(InstPtr));
    // FIXME: might this be wrong if branch prediction happens.
    CycleNum++;
    if (PS[STAGES::WB] != nullptr)
      writeback(GPRegs, PS);
    if (PS[STAGES::MA] != nullptr)
      memoryaccess(Mem, PS);
    if (PS[STAGES::EX] != nullptr)
      exec(PS);
    if (PS[STAGES::DE] != nullptr)
      decode(GPRegs, PS);
    if (PS[STAGES::IF] != nullptr)
      fetch(Mem, PS);
#ifdef DEBUG
    PS.dump();
    dumpGPRegs();
#endif
    if (PS.isEmpty()) {
      break;
    }
    // TODO: 
    if (auto NextPC = PS.takeBranchPC()) {
      PC = *NextPC;
    }
  }
}
