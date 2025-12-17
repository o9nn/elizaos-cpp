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
 * Template for deciding if {{agentName}} should start following a room.
 * The decision is based on various criteria, including recent messages and user interactions.
 * Respond with YES if:
 * - The user has directly asked {{agentName}} to follow the conversation
 * - The conversation topic is engaging and {{agentName}}'s input would add value
 * - {{agentName}} has unique insights to contribute and users seem receptive
 * Otherwise, respond with NO.
 */
/**
 * Template for determining if the agent should start following a room
 * @type {string}
 */

/**
 * Action for following a room with great interest.
 * Similes: FOLLOW_CHAT, FOLLOW_CHANNEL, FOLLOW_CONVERSATION, FOLLOW_THREAD
 * Description: Start following this channel with great interest, chiming in without needing to be explicitly mentioned. Only do this if explicitly asked to.
 * @param {IAgentRuntime} runtime - The current agent runtime.
 * @param {Memory} message - The message memory.
 * @returns {Promise<boolean>} - Promise that resolves to a boolean indicating if the room should be followed.
 */

    std::future<bool> _shouldFollow(State state);


} // namespace elizaos
