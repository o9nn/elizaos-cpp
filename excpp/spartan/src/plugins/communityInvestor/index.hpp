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
#include "elizaos/core.hpp"
#include "events.hpp"
#include "routes.hpp"
#include "service.hpp"
#include "tests.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// AgentPanel interface defined locally as before
struct AgentPanel {
    std::string name;
    std::string path;
    std::string component;
    std::optional<std::string> icon;
    std::optional<bool> public;
};

// Removed getLeaderboardHandler and local communityInvestorRoutes definition from here
// They are now correctly defined in routes.ts

/**
 * Plugin representing the Community Investor Plugin for Eliza.
 * Includes evaluators, actions, and services for community investment functionality.
 */

* from './types';

} // namespace elizaos
