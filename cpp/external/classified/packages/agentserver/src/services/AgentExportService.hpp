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



struct ExportManifest {
    std::string version;
    std::string exportedAt;
    std::string agentId;
    std::string agentName;
    std::vector<std::string> tables;
    double fileCount;
    std::string elizaVersion;
};

struct ExportOptions {
    std::optional<bool> includeFiles;
    std::optional<bool> compress;
};

class AgentExportService {
  private db: any; // Type as any to avoid drizzle version conflicts
  private pool: Pool;

  constructor(
    private agentId: UUID,
    private runtime: IAgentRuntime,
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
   * Export agent data to a ZIP archive
   */

      // Create ZIP archive

      // Handle archive errors

      // Export database data

      // Export uploaded files if requested

      // Add manifest to archive

      // Finalize archive

  /**
   * Export all database data for the agent
   */

    // Export agent data

    // Export entities

    // Export worlds

    // Export rooms

    // Export participants

    // Export memories (handle vector embeddings)

    // Convert vector embeddings to arrays for JSON serialization

    // Export relationships

    // Export tasks

    // Export server_agents associations

    // Create manifest

  /**
   * Export uploaded files for the agent
   */

      // Add the entire upload directory to the archive

      // Count files

  /**
   * Clean up resources
   */

} // namespace elizaos
