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



struct GetOrCreateDMChannelRequest {
    std: userId;
    std: agentId;
    std::optional<std:> sessionId;
    std::optional<std:> initialMessage;
};

struct DMChannelMetadata {
    std: user1;
    std: user2;
    std: forAgent;
    std: createdAt;
    std::optional<std:> sessionId;
    std::optional<std:> initialMessage;
};

std::future<void> POST(NextRequest request);

} // namespace elizaos
