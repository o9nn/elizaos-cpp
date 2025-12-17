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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Type assertion for gamification service (will be available after API client rebuild)

struct LeaderboardPageProps {
    UUID agentId;
    std::optional<UUID> userId;
};


} // namespace elizaos
