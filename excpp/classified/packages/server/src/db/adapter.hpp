#include "connection.hpp"
#include "elizaos/core.hpp"
#include "schema.hpp"
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



class MessageDatabaseAdapter {
  // Provide access to the underlying database for raw queries
  get db() {
    return getDatabase();
  }

  // Server methods

  // Channel methods

    // Add participants if provided

    // Update participants if provided
      // Remove all existing participants

      // Add new participants

    // Find existing DM channel between these two users

    // Create new DM channel

  // Message methods

  // Server-Agent association methods

  // Additional helper methods

  // Close database connection
    await closeDatabase();

} // namespace elizaos
