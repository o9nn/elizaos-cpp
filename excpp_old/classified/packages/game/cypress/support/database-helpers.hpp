#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
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
    std::variant<'ASC', 'DESC'> orderDir;
};

class DatabaseTestHelper {
public:
    DatabaseTestHelper(string = 'http://localhost:7777' backendUrl);
    Cypress::Chainable<string> authenticate(string = 'admin' username, string = 'password123' password);
    Cypress::Chainable<DatabaseTable[]> getTables();
    Cypress::Chainable<TableData> getTableData(const std::string& tableName, std::optional<std::any> options);
    Cypress::Chainable<any> getRecord(const std::string& tableName, const std::string& recordId);
    Cypress::Chainable<any> createRecord(const std::string& tableName, Record<string recordData, auto any>);
    Cypress::Chainable<any> updateRecord(const std::string& tableName, const std::string& recordId, Record<string updateData, auto any>);
    Cypress::Chainable<any> deleteRecord(const std::string& tableName, const std::string& recordId);
    Cypress::Chainable<any> createTestMemory(const std::optional<std::any>& customData);
    Cypress::Chainable<any> createTestAgent(const std::optional<std::any>& customData);
    Cypress::Chainable<void> cleanupTestRecords(const std::string& tableName, string = 'test-' searchPattern);
    Cypress::Chainable<void> waitForTableRowCount(const std::string& tableName, double expectedCount, number = 10000 timeout);
    Cypress::Chainable<boolean> verifyRecordExists(const std::string& tableName, Record<string searchCriteria, auto any>);
    std::unordered_map<std::string, std::string> getAuthHeaders();
    std::unordered_map<std::string, std::any> generateTestData(const std::string& tableName);

private:
    std::string backendUrl_;
};

// Cypress commands for database testing
    struct Chainable {
};

// Register Cypress commands


} // namespace elizaos
