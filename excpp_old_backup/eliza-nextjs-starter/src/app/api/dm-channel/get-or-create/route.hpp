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



struct GetOrCreateDMChannelRequest {
    std::string userId;
    std::string agentId;
    std::optional<string; // Optional session ID for deterministic channel creation> sessionId;
    std::optional<string; // Optional initial message for new sessions> initialMessage;
};

struct DMChannelMetadata {
    std::string user1;
    std::string user2;
    std::string forAgent;
    std::string createdAt;
    std::optional<std::string> sessionId;
    std::optional<std::string> initialMessage;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
