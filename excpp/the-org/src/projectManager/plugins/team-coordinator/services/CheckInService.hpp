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
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extension of CheckInSchedule with additional fields

// Interface for storing report channel configuration
struct ReportChannelConfig {
    std::optional<std::string> serverId;
    std::string serverName;
    std::string channelId;
    std::string createdAt;
};

using BaseInteraction = std::variant<ButtonInteraction, StringSelectMenuInteraction, SelectMenuInteraction>;

// Define our custom interaction type
struct ExtendedInteraction {
    std::string customId;
    std::optional<User> user;
    std::optional<std::vector<std::string>> checkin_frequency;
    std::optional<std::vector<std::string>> checkin_time;
    std::optional<std::vector<std::string>> timezone;
    std::optional<std::vector<std::string>> checkin_days;
    std::optional<std::vector<std::string>> checkin_type;
    std::optional<std::vector<std::string>> checkin_channel;
    std::optional<std::vector<std::string>> report_channel;
    std::optional<std::vector<std::string>> server_info;
    std::optional<std::string> guildId;
};

class CheckInService {
public:
    CheckInService();
    std::future<void> start();
    std::future<void> stop();
    void initialize();
    std::future<DiscordService> ensureDiscordClient(IAgentRuntime runtime);
    void handleCheckInSubmission(ExtendedInteraction interaction);
    std::future<void> storeCheckInSchedule(const std::string& roomId, ExtendedCheckInSchedule schedule);
    void handleReportChannelSubmission(ExtendedInteraction interaction);
    std::future<void> storeReportChannelConfig(ReportChannelConfig config);
};

      // Ensure the room exists before trying to access it

  // Helper method to get report channel for a server

} // namespace elizaos
