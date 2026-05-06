#include "entities.hpp"
#include "logger.hpp"
#include "types.hpp"
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

// File: /swarm/shared/ownership/core.ts
// Updated to use world metadata instead of cache

/**
 * Represents the state of server ownership, including a mapping of server IDs to their respective World objects.
 */
/**
 * Interface representing the ownership state of servers.
 * @property {Object.<string, World>} servers - The servers and their corresponding worlds, where the key is the server ID and the value is the World object.
 */
struct ServerOwnershipState {
    { servers;
};

/**
 * Gets a user's role from world metadata
 */
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
std::future<World[] | null> findWorldsForOwner(IAgentRuntime runtime, const std::string& entityId);

} // namespace elizaos
