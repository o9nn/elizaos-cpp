#include "elizaos/core.hpp"
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



// Define interfaces for Discord component types
struct DiscordComponent {
    double type;
};

/**
 * Sends a check-in report form to Discord
 * @param callback - The callback function to handle form submission
 * @param channels - Optional array of Discord text channels to display in the form
 * @param serverInfo - Server information containing serverId and serverName
 */
std::future<void> sendCheckInReportForm(HandlerCallback callback, std::optional<std::vector<{ id: string; name: string; type: string }>> channels, std::optional<{ serverId: string; serverName: string }> serverInfo);

} // namespace elizaos
