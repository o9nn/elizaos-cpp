#include "elizaos/core.hpp"
#include "use-query-hooks.hpp"
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



// Create ElizaClient instance

/**
 * Hook to get or create a DM channel between current user and target user (agent)
 * This is the original behavior, usually resulting in a single canonical DM channel.
 */
void useGetOrCreateDmChannel();

/**
 * Hook to fetch all DM conversations (channels marked as DMs) for a specific agent.
 */
void useDmChannelsForAgent(UUID | undefined agentId, UUID = '00000000-0000-0000-0000-000000000000' as UUID serverId);

/**
 * Hook to create a new, distinct DM channel (conversation) with an agent.
 */
void useCreateDmChannel();

} // namespace elizaos
