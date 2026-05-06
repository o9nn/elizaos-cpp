#include ".lib/base-client.hpp"
#include ".types/memory.hpp"
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



class MemoryService extends BaseApiClient {
  /**
   * Get agent memories
   */
  async getAgentMemories(agentId: UUID, params?: MemoryParams): Promise<{ memories: Memory[] }> {
    return this.get<{ memories: Memory[] }>(`/api/memory/${agentId}/memories`, { params });
  }

  /**
   * Get room-specific memories
   */

  /**
   * Update a memory
   */

  /**
   * Clear all agent memories
   */

  /**
   * Clear room memories
   */

  /**
   * List agent's rooms
   */

  /**
   * Get room details
   */

  /**
   * Create a room
   */

  /**
   * Create world from server
   */

  /**
   * Delete a world
   */

  /**
   * Clear world memories
   */

  /**
   * Delete a specific memory
   */

  /**
   * Get agent internal memories
   */

  /**
   * Delete agent internal memory
   */

  /**
   * Delete all agent internal memories
   */

  /**
   * Update agent internal memory
   */

  /**
   * Delete group memory (implemented via messaging channel message deletion)
   */

  /**
   * Clear group chat (implemented via messaging channel history clearing)
   */

} // namespace elizaos
