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
 * Template for determining if an agent should unmute a previously muted room.
 * * @type { string }
 */
/**
 * Template for deciding if {{agentName}} should unmute a previously muted room.
 *
 * @type {string}
 */

/**
 * Action to unmute a room, allowing the agent to consider responding to messages again.
 *
 * @name UNMUTE_ROOM
 * @similes ["UNMUTE_CHAT", "UNMUTE_CONVERSATION", "UNMUTE_ROOM", "UNMUTE_THREAD"]
 * @description Unmutes a room, allowing the agent to consider responding to messages again.
 *
 * @param {IAgentRuntime} runtime - The agent runtime to access runtime functionalities.
 * @param {Memory} message - The message containing information about the room.
 * @returns {Promise<boolean>} A boolean value indicating if the room was successfully unmuted.
 */
    std::future<bool> _shouldUnmute(State state);


} // namespace elizaos
