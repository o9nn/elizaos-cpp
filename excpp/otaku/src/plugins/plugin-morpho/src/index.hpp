#include "plugin.hpp"
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

// Export the default plugin
default morphoPlugin;

// Named exports for convenience
{ morphoPlugin };

// Export services
{ MorphoService } from "./services";

// Export actions
{ marketInfoAction } from "./actions";

// Export types
* from "./types";

// Export utilities
{
  ErrorHandler,
  MorphoError,
  // GasOptimizer
} from "./utils";

} // namespace elizaos
