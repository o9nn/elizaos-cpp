#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CreateDMChannelRequest {
    std: userId;
    std: agentId;
    std::optional<std:> channelId;
    std::optional<std:> title;
};

struct DMChannelMetadata {
    std: user1;
    std: user2;
    std: forAgent;
    std: createdAt;
    std::optional<std:> title;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
