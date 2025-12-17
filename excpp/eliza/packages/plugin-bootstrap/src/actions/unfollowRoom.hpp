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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Template for deciding if an agent should stop closely following a previously followed room
 *
 * @type {string}
 */
/**
 * Template for determining if an agent should stop closely following a room and only respond when mentioned.
 * @param {string} agentName - The name of the agent to be referenced in the template.
 * @param {string} recentMessages - The recent messages in the conversation to be included in the template.
 * @param {string} booleanFooter - The footer for the template indicating the possible responses.
 * @returns {string} The template with placeholders for agent name, recent messages, and response.
 */

/**
 * Action for unfollowing a room.
 *
 * - Name: UNFOLLOW_ROOM
 * - Similes: ["UNFOLLOW_CHAT", "UNFOLLOW_CONVERSATION", "UNFOLLOW_ROOM", "UNFOLLOW_THREAD"]
 * - Description: Stop following this channel. You can still respond if explicitly mentioned, but you won't automatically chime in anymore. Unfollow if you're annoying people or have been asked to.
 * - Validate function checks if the room state is "FOLLOWED".
 * - Handler function handles the unfollowing logic based on user input.
 * - Examples provide sample interactions for unfollowing a room.
 */
    std::future<bool> _shouldUnfollow(State state);


} // namespace elizaos
