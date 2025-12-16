#include ".lib/base-client.hpp"
#include ".types/agents.hpp"
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



class AgentsService extends BaseApiClient {
  /**
   * List all agents with minimal details
   */
  async listAgents(): Promise<{ agents: Agent[] }> {
    return this.get<{ agents: Agent[] }>('/api/agents');
  }

  /**
   * Get specific agent details
   */

  /**
   * Create a new agent
   */

  /**
   * Update an existing agent
   */

  /**
   * Delete an agent
   */

  /**
   * Start an existing agent
   */

  /**
   * Stop a running agent
   */

  /**
   * Get all available worlds
   */

  /**
   * Add agent to a world
   */

  /**
   * Update agent's world settings
   */

  /**
   * Get agent's plugin panels
   */

  /**
   * Get agent logs
   */

  /**
   * Delete a specific log entry
   */

  /**
   * Get agents associated with a server
   */


} // namespace elizaos
