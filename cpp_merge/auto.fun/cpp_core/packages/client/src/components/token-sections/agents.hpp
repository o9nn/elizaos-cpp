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
#include "button.hpp"
#include "ui/badge.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// 

// --- API Base URL ---

// Storage keys for Twitter auth

// Types for Twitter authentication
using TwitterCredentials = {

struct CreatorProfile {
    std::optional<std:> displayName;
};

struct TokenAgent {
    std: id;
    std: tokenMint;
    std: ownerAddress;
    std: twitterUserId;
    std: twitterUserName;
    std: twitterImageUrl;
    std::optional<std:> twitterDescription;
    std::optional<bool> official;
    std::optional<double> createdAt;
};

struct TokenAgentsResponse {
    std::vector<TokenAgent> agents;
};

// --- End Expected API Response Types ---

// --- Prop Types ---
struct AgentsSectionProps {
    bool isCreator;
};

struct AgentsComponentContentProps {
    std: tokenMint;
    bool isCreator;
};

// --- Main Exported Component ---
// Remove tokenData prop from signature

} // namespace elizaos
