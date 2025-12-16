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
 * Database Test Helpers
 *
 * Utilities for testing database viewer functionality
 */

struct AuthTokenResponse {
    bool success;
    { data;
    std::string token;
    { user;
    std::string id;
    std::string username;
    std::vector<std::string> roles;
    std::vector<std::string> permissions;
};

struct DatabaseTable {
    std::string name;
    std::string schema;
    double rowCount;
    std::optional<std::string> createSql;
};

struct TableColumn {
    std::string name;
    std::string type;
    bool nullable;
    std::optional<std::string> defaultValue;
    std::optional<bool> primaryKey;
};

struct TableData {
    std::string table;
    std::vector<TableColumn> columns;
    std::vector<std::any> data;
    { pagination;
    double page;
    double limit;
    double total;
    double totalPages;
    bool hasNext;
    bool hasPrev;
    { filters;
    std::string search;
    std::string orderBy;
    'ASC' | 'DESC' orderDir;
};

class DatabaseTestHelper {
  private backendUrl: string;
  private authToken: string | null = null;

  constructor(backendUrl: string = 'http://localhost:7777') {
    this.backendUrl = backendUrl;
  }

  /**
   * Authenticate and get auth token
   */

  /**
   * Get list of database tables
   */

  /**
   * Get table data with optional parameters
   */

  /**
   * Get specific record by ID
   */

  /**
   * Create a new record
   */

  /**
   * Update an existing record
   */

  /**
   * Delete a record
   */

  /**
   * Create a test memory record
   */

  /**
   * Create a test agent record
   */

  /**
   * Cleanup test records by pattern
   */
          // Check if any field contains the search pattern

  /**
   * Wait for table to have specific row count
   */

        return checkRowCount();

    return checkRowCount();

  /**
   * Verify record exists in table
   */

  /**
   * Get auth headers for requests
   */

  /**
   * Generate test data for different table types
   */

// Cypress commands for database testing
    struct Chainable {
    std::string tableName;
    std::string recordId;
    std::unordered_map<std::string, std::any> updateData;
    std::string tableName;
    std::unordered_map<std::string, std::any> searchCriteria;
};

// Register Cypress commands


} // namespace elizaos
