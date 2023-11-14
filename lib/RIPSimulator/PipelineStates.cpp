#include "RIPSimulator/PipelineStates.h"

#include <nlohmann/json.hpp>
#include <sstream>

void PipelineStates::dump() {
  // TODO: dump stall,
  for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
    std::cerr << StageNames[(STAGES)Stage] << ": ";
    if (isStall((STAGES)Stage))
      std::cerr << std::hex << "(Stalled) ";

    if (Insts[Stage] != nullptr) {
      std::cerr << std::hex << "PC=0x" << PCs[Stage] << " ";
      Insts[Stage]->mprint(std::cerr);
      std::cerr << ", ";
    } else
      std::cerr << "Bubble, ";

    // TODO: dump stage specific info.
    switch (Stage) {
    case STAGES::DE:
      std::cerr << "Rs1Val=" << DERs1Val << ", ";
      std::cerr << "Rs2Val=" << DERs2Val << ", ";
      std::cerr << "ImmVal=" << DEImmVal << ", ";
      std::cerr << "CSRVal=" << DECSRVal << "\n";
      break;

    case STAGES::EX:
      std::cerr << "CSRVal=" << EXCSRVal << ", ";
      std::cerr << "RdVal=" << EXRdVal << "\n";
      break;

    case STAGES::MA:
      std::cerr << "CSRVal=" << MACSRVal << ", ";
      std::cerr << "RdVal=" << MARdVal << "\n";

      break;

    default:
      std::cerr << "\n";
      break;
    }
  }
}

void PipelineStates::printJSON(std::ostream &os) {

  nlohmann::json JTotal;
  for (int Stage = STAGES::IF; Stage <= STAGES::WB; ++Stage) {
    nlohmann::json JStage;
    if (!Insts[Stage]) {
      JStage["isBubble"] = true;
      continue;
    }
    JStage["isBubble"] = false;

    JStage["isStall"] = isStall((STAGES)Stage);
    // TODO: decode correctly.
    // ;
    std::stringstream ss;
    Insts[Stage]->mprint(ss);
    JStage["InstStr"] = ss.str();

    JStage["mnemo"] = Insts[Stage]->getMnemo();
    JStage["PC"] = PCs[Stage];

    // TODO: dump stage specific info.
    switch (Stage) {
    case STAGES::DE:
      JStage["Rs1Val"] = DERs1Val;
      // TODO: other field.
      // std::cerr << "Rs2Val=" << DERs2Val << ", ";
      // std::cerr << "ImmVal=" << DEImmVal << ", ";
      // std::cerr << "CSRVal=" << DECSRVal << "\n";
      break;

    case STAGES::EX:

      JStage["CSRVal"] = EXCSRVal;
      // TODO: other field.
      // std::cerr << "RdVal=" << EXRdVal << "\n";
      break;

    case STAGES::MA:
      JStage["CSRVal"] = MACSRVal;
      // TODO: other field.
      // std::cerr << "RdVal=" << MARdVal << "\n";
      break;
    default:
      break;
    }
    JTotal[StageNames[(STAGES)Stage]] = JStage;
  }
  os << JTotal.dump() << std::endl;
}