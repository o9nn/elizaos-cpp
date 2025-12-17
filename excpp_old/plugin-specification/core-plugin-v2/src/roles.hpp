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



/**
 * Interface representing the ownership state of servers.
 * @property {Object.<string, World>} servers - The servers and their corresponding worlds, where the key is the server ID and the value is the World object.
 */
struct ServerOwnershipState {
};

/**
 * Retrieve the server role of a specified user entity within a given server.
 *
 * @param {IAgentRuntime} runtime - The runtime object containing necessary configurations and services.
 * @param {string} entityId - The unique identifier of the user entity.
 * @param {string} serverId - The unique identifier of the server.
 * @returns {Promise<Role>} The role of the user entity within the server, resolved as a Promise.
 */
std::future<Role> getUserServerRole(IAgentRuntime runtime, const std::string& entityId, const std::string& serverId);

/**
 * Finds a server where the given user is the owner
 */

} // namespace elizaos
