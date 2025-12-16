#include "action.ts.hpp"
#include "evaluator.ts.hpp"
#include "provider/index.ts.hpp"
#include "service.ts.hpp"
#include "types.ts.hpp"
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
;
;
;

const translationPlugin: Plugin = {
  name: "translation",
  description: "A plugin for translating text between different languages",
  actions: [getTranslationAction],
  evaluators: [translationEvaluator],
  providers: [translationProvider],
  services: [translationService],
};

const initializeTranslationPlugin = (
  config: TranslationConfig,
): void => {
  if (config.provider) {
    initializeTranslationConfig(config.provider);
  }
};

* from "./types.ts";

} // namespace elizaos
