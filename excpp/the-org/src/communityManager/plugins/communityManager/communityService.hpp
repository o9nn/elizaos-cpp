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
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class CommunityManagerService {
public:
    CommunityManagerService();
    static std::future<CommunityManagerService> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    std::future<std::string> getDiscordGreetChannelId(World world, auto guild);
    void getGreetingMessage(IAgentRuntime runtime, const std::string& userName, std::optional<std::string> greetingMessage);
    void onDiscordUserJoined(auto params);
    void onTelegramUserJoined(auto params);
    void addEventListener(IAgentRuntime runtime);
};


} // namespace elizaos
