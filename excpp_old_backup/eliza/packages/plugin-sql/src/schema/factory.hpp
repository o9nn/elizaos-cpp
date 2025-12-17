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
  constructor(public dbType: DatabaseType) {}

    // Both postgres and pglite use pgTable

    // Both postgres and pglite support native UUID
    return pgUuid(name);

    return pgText(name);

    // Both postgres and pglite support JSONB
    return pgJsonb(name);

    return pgBoolean(name);

    return pgTimestamp(name, options);

    return pgInteger(name);

    // Pglite may not support pgvector extension yet
    // For compatibility, we'll store as JSONB for pglite
      return pgJsonb(name);
    return pgVector(name, { dimensions });

    // Both postgres and pglite support arrays

    // Both postgres and pglite support CHECK constraints
    return pgCheck(name, sql);

    return pgIndex(name);

    return pgForeignKey(config);

  // Helper for timestamp defaults
    // Both postgres and pglite support NOW()

  // Helper for random UUID generation
    // Pglite may not have gen_random_uuid() extension
      // Will use application-level UUID generation

// Global factory instance - will be set based on database type

void setDatabaseType(DatabaseType dbType);

SchemaFactory getSchemaFactory();

} // namespace elizaos
