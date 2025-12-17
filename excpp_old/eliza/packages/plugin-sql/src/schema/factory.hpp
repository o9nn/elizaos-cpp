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



using DatabaseType = std::variant<'postgres', 'pglite'>;

// Type helpers for cross-database compatibility
// Since Pglite uses PostgreSQL dialect, we use the same types for both
using TableFn = PgTableFn;
using UuidColumn = ReturnType<typeof pgUuid>;
using TextColumn = ReturnType<typeof pgText>;
using JsonColumn = ReturnType<typeof pgJsonb>;
using BooleanColumn = ReturnType<typeof pgBoolean>;
using TimestampColumn = ReturnType<typeof pgTimestamp>;
using IntegerColumn = ReturnType<typeof pgInteger>;

/**
 * Schema factory to create database-specific column types
 * Since Pglite is PostgreSQL-compatible, we use the same constructs for both
 */
class SchemaFactory {
public:
    SchemaFactory(DatabaseType public dbType);
    TableFn table() const;
    void uuid(const std::string& name);
    void text(const std::string& name);
    void json(const std::string& name);
    void boolean(const std::string& name);
    void timestamp(const std::string& name, std::optional<std::any> options);
    void integer(const std::string& name);
    void vector(const std::string& name, double dimensions);
    void textArray(const std::string& name);
    void check(const std::string& name, const std::any& sql);
    void index(std::optional<std::string> name);
    void foreignKey(const std::any& config);
    void defaultTimestamp();
    void defaultRandomUuid();
};

// Global factory instance - will be set based on database type

void setDatabaseType(DatabaseType dbType);

SchemaFactory getSchemaFactory();

} // namespace elizaos
