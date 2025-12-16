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

// Export the plugin as default
default clankerPlugin;

// Export the plugin by name
{ clankerPlugin };

// Export services for direct use if needed
{ ClankerService } from "./services";

// Export actions for direct use if needed
{ tokenDeployAction } from "./actions";

// Export types
* from "./types";

// Export utilities if needed
* from "./utils/errors";
* from "./utils/format";
* from "./utils/transactions";

} // namespace elizaos
