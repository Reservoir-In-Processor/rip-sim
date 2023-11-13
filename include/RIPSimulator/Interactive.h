#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <nlohmann/json.hpp>
enum class JSONKind { PipelineStates, Learn, Predict };
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

#endif