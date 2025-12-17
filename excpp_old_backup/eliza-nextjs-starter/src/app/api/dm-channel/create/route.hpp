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



struct CreateDMChannelRequest {
    std::string userId;
    std::string agentId;
    std::optional<string; // Optional - will be generated if not provided> channelId;
    std::optional<string; // Optional title for the channel> title;
};

struct DMChannelMetadata {
    std::string user1;
    std::string user2;
    std::string forAgent;
    std::string createdAt;
    std::optional<std::string> title;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
