#include "Assembler/BinaryEmitter.h"
#include "InstructionTypes.h"
#include "Instructions.h"
#include "Registers.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

#ifdef DEBUG
#include "Debug.h"
#endif

void BinaryEmitter::emitBinary(std::ostream &os) {
  while (AP.parseLine()) {
    auto &Toks = AP.getTokens();
    auto &Mnemo = Toks[0];

    std::unique_ptr<Instruction> InstT;
    if (auto IT = ITypeKinds.find(Mnemo); IT != ITypeKinds.end()) {
      assert((Toks.size() == 4 || Toks.size() == 3) &&
             "Wrong token number I-Type Inst.");
      auto UI = std::make_unique<IInstruction>(IT->second, Toks);
      InstT = std::move(UI);

    } else if (auto IT = RTypeKinds.find(Mnemo); IT != RTypeKinds.end()) {
      assert(Toks.size() == 4 && "Wrong token number R-Type Inst.");
      auto UI = std::make_unique<RInstruction>(IT->second, Toks);
      InstT = std::move(UI);

    } else if (auto IT = UTypeKinds.find(Mnemo); IT != UTypeKinds.end()) {
      assert(Toks.size() == 3 && "Wrong token number for U-Type Inst.");
      auto UI = std::make_unique<UInstruction>(IT->second, Toks);
      InstT = std::move(UI);

    } else if (auto IT = JTypeKinds.find(Mnemo); IT != JTypeKinds.end()) {
      assert(Toks.size() == 3 && "Wrong token number for J-Type Inst.");
      auto UI = std::make_unique<JInstruction>(IT->second, Toks);
      InstT = std::move(UI);
    } else if (auto IT = STypeKinds.find(Mnemo); IT != STypeKinds.end()) {
      assert(Toks.size() == 3 && "Wrong token number for S-Type Inst.");
      auto UI = std::make_unique<SInstruction>(IT->second, Toks);
      InstT = std::move(UI);
    } else if (auto IT = BTypeKinds.find(Mnemo); IT != BTypeKinds.end()) {
      assert(Toks.size() == 4 && "Wrong token number for B-Type Inst.");
      auto UI = std::make_unique<BInstruction>(IT->second, Toks);
      InstT = std::move(UI);

    } else {
      std::cerr << Mnemo << "\n";
      assert(false && "unimplemented!");
    }

    // TODO: create DEBUGEXPR MACRO to shorten this
#ifdef DEBUG
    debugInstOnAsm(Toks, InstT->getVal());
#endif
    InstT->emitBinary(os);
    InstT.reset();
  }
}
