#include "elizaos/core.hpp"
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

// Re-the self-modification plugin
// This wrapper allows us to include the plugin without TypeScript complaining about rootDir
{ selfModificationPlugin } from "./plugin-self-modification/src/index";

// Re-types for convenience
type {
  CharacterModification,
  CharacterSnapshot,
  CharacterDiff,
  ModificationOperation,
  ModificationOptions,
  ValidationResult,
} from "./plugin-self-modification/src/types";

} // namespace elizaos
