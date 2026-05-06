#include "dbCompatibility.hpp"
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
 * Component service that provides database-compatible component operations
 */
class ComponentService {
  constructor(private runtime: IAgentRuntime) {}

  /**
   * Get a component with database-compatible data parsing
   */

  /**
   * Create a component with database-compatible data
   */
      // Pass params directly to runtime - it handles database-specific formatting internally

  /**
   * Update a component with database-compatible data
   */
      // Pass params directly to runtime - it handles database-specific formatting internally

  /**
   * Normalize a component from the database
   */
    // Parse the data field

    // Normalize any boolean fields in data

    // Normalize UUIDs to ensure consistent format

  /**
   * Recursively normalize boolean values in an object
   */

          // Likely a boolean stored as 0/1

/**
 * Create a component service instance for the runtime
 */
ComponentService createComponentService(IAgentRuntime runtime); 
} // namespace elizaos
