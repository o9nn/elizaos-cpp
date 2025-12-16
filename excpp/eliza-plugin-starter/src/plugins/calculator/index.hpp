#include "action.ts.hpp"
#include "evaluator.ts.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

const calculatorPlugin: Plugin = {
  name: "calculator",
  description: "Basic arithmetic calculator plugin",
  actions: [calculateAction],
  evaluators: [calculateEvaluator],
  providers: [],
};

} // namespace elizaos
