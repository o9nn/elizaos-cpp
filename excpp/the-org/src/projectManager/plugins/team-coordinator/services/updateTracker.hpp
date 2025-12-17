#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".actions/checkInList.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define interfaces for custom services

// Extended CheckInSchedule with lastUpdated property

class TeamUpdateTrackerService {
public:
    TeamUpdateTrackerService(IAgentRuntime runtime);
    std::future<void> start();
    void setupDiscordRetry();
    void setupTelegramRetry();
    Promise<Array< fetchUsersInChannel(const std::string& channelId);
    std::future<std::vector<std::string>> messageAllUsers(std::optional<Array<{
      id: string;
      username: string;
      displayName: string;
      channelName: string;
      updatesFormat: string[];
    }>> users, CheckInSchedule schedule, std::optional<std::string> serverName);
    std::future<void> checkInServiceJob();
    std::future<void> stop();

// Static start method required for service registration

} // namespace elizaos
