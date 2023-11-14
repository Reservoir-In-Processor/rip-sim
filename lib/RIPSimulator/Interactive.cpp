#include "RIPSimulator/Interactive.h"
std::string JSONKindToString(JSONKind kind) {
  switch (kind) {
  case JSONKind::PipelineStates:
    return "PipelineStates";
  case JSONKind::Learn:
    return "Learn";
  case JSONKind::Predict:
    return "Predict";
  default:
    return "Unknown";
  }
}