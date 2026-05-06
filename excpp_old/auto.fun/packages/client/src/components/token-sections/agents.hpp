#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".button.hpp"
#include ".ui/badge.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// 

// --- API Base URL ---

// Storage keys for Twitter auth

// Types for Twitter authentication
using TwitterCredentials = {

struct CreatorProfile {
    std::optional<std::string> displayName;
};

struct TokenAgent {
    std::string id;
    std::string tokenMint;
    std::string ownerAddress;
    std::string twitterUserId;
    std::string twitterUserName;
    std::string twitterImageUrl;
    std::optional<std::string> twitterDescription;
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
    std::string tokenMint;
    bool isCreator;
};

// --- Main Exported Component ---
// Remove tokenData prop from signature

} // namespace elizaos
