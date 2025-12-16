#include "elizaos/core.hpp"
#include "evaluators/experienceEvaluator.hpp"
#include "providers/experienceRAG.hpp"
#include "providers/recentExperiences.hpp"
#include "service.hpp"
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

import type { Plugin } from "@elizaos/core";
;
;
;
;
;
;
import "./types"; // Ensure module augmentation is loaded

const experiencePlugin: Plugin = {
  name: "experience",
  description:
    "Self-learning experience system that records experiences and learns from agent interactions",

  services: [ExperienceService],

  providers: [experienceRAGProvider, recentExperiencesProvider],

  evaluators: [experienceEvaluator],

  init: async (config: Record<string, any>, runtime: IAgentRuntime) => {
    logger.info(
      "[ExperiencePlugin] Initializing self-learning experience system",
    );

    const maxExperiences = config.maxExperiences || 10000;
    const autoRecordThreshold = config.autoRecordThreshold || 0.7;

    logger.info(`[ExperiencePlugin] Configuration read:
    - Max experiences: ${maxExperiences}
    - Auto-record threshold: ${autoRecordThreshold}`);
  },
};

// Export individual components for testing
{ ExperienceService } from "./service";
* from "./types";

default experiencePlugin;

} // namespace elizaos
