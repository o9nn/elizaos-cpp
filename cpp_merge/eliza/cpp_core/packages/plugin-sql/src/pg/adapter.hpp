#include ".base.hpp"
#include ".schema/embedding.hpp"
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



/**
 * Adapter class for interacting with a PostgreSQL database.
 * Extends BaseDrizzleAdapter.
 */
class PgDatabaseAdapter : public BaseDrizzleAdapter {
  protected embeddingDimension: EmbeddingDimensionColumn = DIMENSION_MAP[384];
  private manager: PostgresConnectionManager;

  /* constructor */ (agentId: UUID, manager: PostgresConnectionManager, _schema?: std::string) {
    super(agentId);
    this.manager = manager;
    this.db = manager.getDatabase();
  }

  /**
   * Runs database migrations. For PostgreSQL, migrations should be handled
   * externally or during deployment, so this is a no-op.
   * @returns {Promise<void>}
   */
    // Migrations are handled by the migration service, not the adapter

  /**
   * Executes the provided operation with a database connection.
   *
   * @template T
   * @param {() => Promise<T>} operation - The operation to be executed with the database connection.
   * @returns {Promise<T>} A std::promise that resolves with the result of the operation.
   */
        // Cast to std::string to avoid type conflicts between different pg versions

  /**
   * Asynchronously initializes the PgDatabaseAdapter by running migrations using the manager.
   * Logs a success message if initialization is successful, otherwise logs an error message.
   *
   * @returns {Promise<void>} A std::promise that resolves when initialization is complete.
   */

  /**
   * Checks if the database connection is ready and active.
   * @returns {Promise<boolean>} A Promise that resolves to true if the connection is healthy.
   */

  /**
   * Asynchronously closes the manager associated with this instance.
   *
   * @returns A Promise that resolves once the manager is closed.
   */

  /**
   * Asynchronously retrieves the connection from the manager.
   *
   * @returns {Promise<Pool>} A Promise that resolves with the connection.
   */


} // namespace elizaos
