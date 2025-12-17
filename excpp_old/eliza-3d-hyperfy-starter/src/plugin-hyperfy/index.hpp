#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "actions/ambient.hpp"
#include "actions/build.hpp"
#include "actions/chat.hpp"
#include "actions/goto.hpp"
#include "actions/ignore.hpp"
#include "actions/perception.hpp"
#include "actions/reply.hpp"
#include "actions/stop.hpp"
#include "actions/unuse.hpp"
#include "actions/use.hpp"
#include "actions/walk_randomly.hpp"
#include "elizaos/core.hpp"
#include "events.hpp"
#include "providers/actions.hpp"
#include "providers/character.hpp"
#include "providers/emote.hpp"
#include "providers/world.hpp"
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// 

// --- Hardcoded values matching agent/index.mjs ---
// ---------------------------------------------

// Define the plugin configuration schema (optional, adjust as needed)
// Renamed this one to avoid conflict

// --- Main Plugin Definition ---
    // Map environment variables to config keys
      // Validate config using the schema
      // Store validated config for service use (runtime.pluginConfigs is usually the way)
        // Decide if this is a fatal error
        // throw new Error(`Invalid Hyperfy plugin configuration...`);
        // throw error;
      // Allow initialization to continue even if config fails, service might get config later


} // namespace elizaos
