#include ".base.hpp"
#include ".schema.hpp"
#include ".schema/embedding.hpp"
#include "elizaos/core.hpp"
#include "manager.hpp"
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
 * PostgreSQL adapter implementation for database operations.
 * Extends the BaseDrizzleAdapter to provide PostgreSQL-specific functionality.
 */
class PgDatabaseAdapter extends BaseDrizzleAdapter {
  protected embeddingDimension: EmbeddingDimensionColumn = DIMENSION_MAP[768];
  private manager: PostgresConnectionManager;

  constructor(agentId: UUID, manager: PostgresConnectionManager, _schema?: any) {
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
   * @returns {Promise<T>} A promise that resolves with the result of the operation.
   */
        // Cast to any to avoid type conflicts between different pg versions

  /**
   * Asynchronously initializes the PgDatabaseAdapter by running migrations using the manager.
   * Logs a success message if initialization is successful, otherwise logs an error message.
   *
   * @returns {Promise<void>} A promise that resolves when initialization is complete.
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
