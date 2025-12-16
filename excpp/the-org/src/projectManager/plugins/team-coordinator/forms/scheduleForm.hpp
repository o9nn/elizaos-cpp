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
 * Sends a check-in schedule setup form to Discord
 * @param callback - The callback function to handle form submission
 * @param channels - Optional array of Discord text channels to display in the form
 */
std::future<void> sendCheckInScheduleForm(HandlerCallback callback, std::optional<std::vector<{ id: string; name: string; type: string }>> channels);

} // namespace elizaos
