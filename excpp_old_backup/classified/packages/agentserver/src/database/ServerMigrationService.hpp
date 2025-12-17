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



// Define the minimal database type we need
using DrizzleDatabase = {

/**
 * ServerMigrationService handles database migrations for server-specific tables
 * This is separate from plugin migrations to maintain clean separation of concerns
 */
class ServerMigrationService {
  constructor(private db: DrizzleDatabase) {}

  /**
   * Runs all server-specific migrations
   */

      // Create messageServers table

      // Create channels table

      // Create messages table

      // Create channel_participants table

      // Create server_agents table

      // Use CREATE TABLE IF NOT EXISTS to avoid errors

      // Create index on server_id for better query performance

      // Create indexes for better query performance

      // Create indexes for better query performance

      // Add foreign key constraint to agents table separately
      // This assumes the agents table exists (created by plugin-sql)
        // First check if the constraint already exists


} // namespace elizaos
