#include ".lib/base-client.hpp"
#include ".types/system.hpp"
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



class SystemService extends BaseApiClient {
  /**
   * Retrieve the local environment variables from the ElizaOS server.
   *
   * Server route (packages/server/src/api/system):
   *   GET /api/system/env/local  ->  { success: true, data: Record<string,string> }
   */
  async getEnvironment(): Promise<Record<string, string>> {
    return this.get<Record<string, string>>('/api/system/env/local');
  }

  /**
   * Update (overwrite or merge) the local .env file on the ElizaOS server.
   *
   * Server route (packages/server/src/api/system):
   *   POST /api/system/env/local  ->  { success: true, message: string }
   *   Body: { content: Record<string,string> }
   *
   * For developer-ergonomics we accept several shapes:
   *   1. { variables: Record<string,string>; merge?: boolean }
   *   2. { content:   Record<string,string> }      (server-native)
   *   3. Record<string,string>                      (shorthand)
   */

      // eslint-disable-next-line @typescript-eslint/consistent-type-assertions
      // Treat params itself as record of env vars

  /**
   * Global logs functionality - implementing via system endpoints
   */
    // Special handling for logs endpoint that returns data directly without wrapper

    // The logs endpoint returns data directly, not wrapped in { success, data }
    // Map the response to expected format

    // Note: Individual log deletion is not supported by the server
    // The server only supports bulk deletion via deleteGlobalLogs()

} // namespace elizaos
