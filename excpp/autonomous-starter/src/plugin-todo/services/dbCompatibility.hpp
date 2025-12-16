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
 * Database compatibility service to handle differences between SQLite and PostgreSQL
 */
class DatabaseCompatibilityService {
  private databaseType: "sqlite" | "postgres" | "unknown" = "unknown";

  constructor() {
    this.detectDatabaseType();
  }

  /**
   * Detect the current database type from environment or connection
   */
    // Check common environment variables
    
      // Default to sqlite for local development

  /**
   * Convert UUID for database storage
   */
      // PostgreSQL can handle UUID type directly
    // SQLite stores as TEXT

  /**
   * Convert boolean for database storage
   */
      // SQLite stores booleans as 0/1
    // PostgreSQL has native boolean

  /**
   * Parse boolean from database
   */
      // SQLite returns 0/1
    // PostgreSQL returns native boolean

  /**
   * Format date for database storage
   */
    
      // SQLite prefers ISO string format
    // PostgreSQL can handle both but prefers ISO format

  /**
   * Parse date from database
   */
    
        // SQLite might store as Unix timestamp

  /**
   * Format JSON for database storage
   */
      // SQLite stores JSON as TEXT
    // PostgreSQL JSONB can accept objects directly

  /**
   * Parse JSON from database
   */
    
    // PostgreSQL returns parsed object

  /**
   * Format array for database storage (for tags)
   */
      // SQLite stores arrays as JSON
    // PostgreSQL can use native arrays or JSONB

  /**
   * Parse array from database
   */
    
    // PostgreSQL returns native array

  /**
   * Build case-insensitive search condition
   */
      // PostgreSQL needs ILIKE for case-insensitive
    // SQLite LIKE is case-insensitive by default

  /**
   * Get current database type
   */

// Export singleton instance
} // namespace elizaos
