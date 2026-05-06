#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using DrizzleDB = std::variant<NodePgDatabase, PgliteDatabase>;

/**
 * Extract clean error message from Drizzle wrapped errors
 * Drizzle wraps PostgreSQL errors and only shows the SQL query in the error message,
 * hiding the actual error in the cause property.
 */
std::string extractErrorMessage(unknown error);

/**
 * Extract detailed error information including stack trace for logging
 * Returns both the clean message and stack trace for comprehensive debugging
 */
void extractErrorDetails(unknown error); {

struct ColumnDefinition {
    std::string name;
    std::string type;
    std::optional<bool> primaryKey;
    std::optional<bool> notNull;
    std::optional<std::string> defaultValue;
    std::optional<bool> unique;
};

struct IndexDefinition {
    std::string name;
    std::vector<std::string> columns;
    std::optional<bool> unique;
};

struct ForeignKeyDefinition {
    std::string name;
    std::vector<std::string> columns;
    std::string referencedTable;
    std::vector<std::string> referencedColumns;
    std::optional<std::string> onDelete;
};

struct TableDefinition {
    std::string name;
    std::vector<ColumnDefinition> columns;
    std::vector<IndexDefinition> indexes;
    std::vector<ForeignKeyDefinition> foreignKeys;
    std::vector<std::string> dependencies;
};

// Known composite primary keys for tables that don't have proper metadata
  // Add other tables with composite primary keys here if needed

// Helper function to convert camelCase to snake_case
std::string camelToSnakeCase(const std::string& str);

class DrizzleSchemaIntrospector {
public:
    TableDefinition parseTableDefinition(const std::any& table, std::optional<std::string> exportKey);
    std::string getTableName(const std::any& table, std::optional<std::string> exportKey);
    std::vector<ColumnDefinition> parseColumns(const std::any& table);
    std::vector<ColumnDefinition> parseColumnsFallback(const std::any& table);
    std::vector<ForeignKeyDefinition> parseForeignKeys(const std::any& table);
    std::optional<std::string> extractReferencedTableName(const std::any& reference);
    std::vector<IndexDefinition> parseIndexes(const std::any& table);
     parseCheckConstraints(const std::any& table);
    void if(auto tableConfig && tableConfig.extraConfigBuilder);
     parseCompositePrimaryKey(const std::any& table, std::optional<Map<string> propertyToColumnMap, auto string>);
    void if(auto !tableConfig);
    void if(auto tableConfig && tableConfig.extraConfigBuilder);
    std::string getSQLType(const std::any& column, const std::string& columnName);
    std::string mapDrizzleColumnType(const std::string& columnType, const std::any& config, const std::string& columnName);
    std::string getSQLTypeFromDataType(const std::string& dataType, const std::string& columnName);
    std::string formatDefaultValue(const std::any& defaultValue);
    std::string generateCreateTableSQL(TableDefinition tableDef, const std::string& schemaName);
    std::vector<std::string> generateForeignKeySQL(TableDefinition tableDef, const std::string& schemaName);
};

class PluginNamespaceManager {
public:
    PluginNamespaceManager(DrizzleDB private db);
    std::future<std::string> getPluginSchema(const std::string& pluginName);
    std::future<void> ensureNamespace(const std::string& schemaName);
    std::future<std::vector<std::string>> introspectExistingTables(const std::string& schemaName);
    std::future<bool> foreignKeyExists(const std::string& schemaName, const std::string& tableName, const std::string& constraintName);
    std::future<bool> checkConstraintExists(const std::string& schemaName, const std::string& tableName, const std::string& constraintName);
    std::future<bool> uniqueConstraintExists(const std::string& schemaName, const std::string& tableName, const std::string& constraintName);
    std::future<void> createTable(TableDefinition tableDef, const std::string& schemaName);
    std::future<void> addConstraints(TableDefinition tableDef, const std::string& schemaName);
};

class ExtensionManager {
public:
    ExtensionManager(DrizzleDB private db);
    std::future<void> installRequiredExtensions(const std::vector<std::string>& requiredExtensions);
};

// Topological sort for dependency ordering
std::vector<std::string> topologicalSort(Map<string tables, auto TableDefinition>);

std::future<void> runPluginMigrations(DrizzleDB db, const std::string& pluginName, const std::any& schema);

} // namespace elizaos
