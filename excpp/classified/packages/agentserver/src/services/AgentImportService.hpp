#include ".schema.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
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



struct ImportOptions {
    std::optional<bool> overwrite;
    std::optional<bool> validateManifest;
    std::optional<number; // in bytes> maxFileSize;
};

struct ImportResult {
    bool success;
    std::string agentId;
    std::string agentName;
    std::vector<std::string> tablesImported;
    double filesImported;
    std::optional<std::vector<std::string>> errors;
};

class AgentImportService {
  private db: any; // Type as any to avoid drizzle version conflicts
  private pool: Pool;
  private readonly MAX_ZIP_SIZE = 500 * 1024 * 1024; // 500MB default

  constructor(
    private targetAgentId: UUID,
    private serverInstance: AgentServer
  ) {
    // Get database connection from server instance
    // The server has a 'db' property that contains the raw database connection
    this.db = (this.serverInstance as any).db;

    // Create a new pool for cleanup purposes
    // Extract connection details from environment or use defaults
    const postgresUrl =
      process.env.POSTGRES_URL ||
      process.env.DATABASE_URL ||
      'postgresql://eliza:eliza_secure_pass@localhost:5432/eliza';

    this.pool = new Pool({
      connectionString: postgresUrl,
    });
  }

  /**
   * Import agent data from a ZIP archive
   */

    // Validate ZIP size

      // Parse ZIP file

      // Validate and read manifest

      // Begin database transaction
        // Delete existing agent data if overwrite is enabled

        // Import database data in dependency order

      // Import uploaded files (outside transaction)

  /**
   * Validate ZIP structure and read manifest
   */

      // Validate manifest version

      // Validate required fields

      // Validate all expected tables are present

        // Check if corresponding file exists

  /**
   * Delete existing agent data (cascade will handle related tables)
   */

    // Delete from server_agents first (no cascade from agents table)

    // Delete agent (cascade will handle all other tables)

  /**
   * Import agent data
   */

    // Replace the agent ID with our target ID

  /**
   * Import entities
   */

    // Update agent IDs

  /**
   * Import worlds
   */

  /**
   * Import rooms
   */

  /**
   * Import participants
   */

  /**
   * Import memories (convert array embeddings back to vectors)
   */

    // Process memories in batches to avoid overwhelming the database

        // Convert array embeddings back to pgvector format

  /**
   * Import relationships
   */

  /**
   * Import tasks
   */

  /**
   * Import server_agents associations
   */

  /**
   * Import uploaded files
   */

    // Clear existing upload directory

    // Extract files

        // Ensure directory exists

        // Extract file

  /**
   * Clean up resources
   */

} // namespace elizaos
