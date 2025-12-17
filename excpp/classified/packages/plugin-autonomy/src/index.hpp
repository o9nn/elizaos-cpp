#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "action.hpp"
#include "elizaos/core.hpp"
#include "provider.hpp"
#include "routes.hpp"
#include "service.hpp"
#include "status-provider.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Clean autonomy plugin with settings-based control:
 * 1. Service: Autonomous loop controlled via AUTONOMY_ENABLED setting
 * 2. Admin Chat Provider: Admin history (autonomous context only)
 * 3. Status Provider: Shows autonomy status (regular chat only)
 * 4. Action: Send message to admin (autonomous context only)
 * 5. Routes: API for enable/disable/status
 */

// Export components


} // namespace elizaos
