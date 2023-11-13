#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <nlohmann/json.hpp>
enum class JSONKind { PipelineStates, Learn, Predict };
std::string JSONKindToString(JSONKind kind);
#endif