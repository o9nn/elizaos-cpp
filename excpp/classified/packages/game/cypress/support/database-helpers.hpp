#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
    std::string token;
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
    double page;
    double limit;
    double total;
    double totalPages;
    bool hasNext;
    bool hasPrev;
    std::string search;
    std::string orderBy;
    std::string orderDir;
};

class DatabaseTestHelper {
public:
    DatabaseTestHelper(std::string backendUrl = "http://localhost:7777");
    Cypress::Chainable<DatabaseTable[]> getTables();
    Cypress::Chainable<TableData> getTableData(const std::string& tableName, std::any options = {});
    std::unordered_map<std::string, std::string> getAuthHeaders();
    std::unordered_map<std::string, std::any> generateTestData(const std::string& tableName);

// Cypress commands for database testing
    struct Chainable {
};

// Register Cypress commands


} // namespace elizaos
