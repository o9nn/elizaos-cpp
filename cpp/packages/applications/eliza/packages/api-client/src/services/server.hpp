#include ".lib/base-client.hpp"
#include ".types/server.hpp"
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



class ServerService extends BaseApiClient {
  /**
   * Health check
   */
  async checkHealth(): Promise<ServerHealth> {
    return this.get<ServerHealth>('/api/server/health');
  }

  /**
   * Simple ping
   */

  /**
   * Hello endpoint
   */

  /**
   * Get server status
   */

  /**
   * Stop the server
   */

  /**
   * Get runtime debug info
   */

  /**
   * Submit logs
   */

  /**
   * Clear logs
   */

} // namespace elizaos
