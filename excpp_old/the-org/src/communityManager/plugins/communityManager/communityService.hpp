#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class CommunityManagerService {
public:
    CommunityManagerService(IAgentRuntime protected runtime);
    std::future<CommunityManagerService> start(IAgentRuntime runtime);
    std::future<void> stop(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<std::optional<std::string>> getDiscordGreetChannelId(World world, auto guild);
    void getGreetingMessage(IAgentRuntime runtime, const std::string& userName, std::optional<std::string> greetingMessage);
    void onDiscordUserJoined(auto params);
    void onTelegramUserJoined(auto params);
    void addEventListener(IAgentRuntime runtime);
};


} // namespace elizaos
