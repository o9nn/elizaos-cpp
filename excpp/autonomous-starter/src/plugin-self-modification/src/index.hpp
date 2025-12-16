#include "actions/modify-character-action.hpp"
#include "elizaos/core.hpp"
#include "evaluators/character-evolution-evaluator.hpp"
#include "providers/character-state-provider.hpp"
#include "services/character-modification-service.hpp"
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

// Export types for external use
* from "./types";

// Export individual components
{ CharacterModificationService };
{
  characterStateProvider,
  characterDiffProvider,
  modifyCharacterAction,
  viewCharacterHistoryAction,
  rollbackCharacterAction,
  characterEvolutionEvaluator,
};

const selfModificationPlugin: Plugin = {
  name: "self-modification",
  description:
    "Enables agents to modify their own character through reflection and self-learning",

  services: [CharacterModificationService],

  providers: [characterStateProvider, characterDiffProvider],

  actions: [
    modifyCharacterAction,
    viewCharacterHistoryAction,
    rollbackCharacterAction,
  ],

  evaluators: [characterEvolutionEvaluator],
};

default selfModificationPlugin;

} // namespace elizaos
