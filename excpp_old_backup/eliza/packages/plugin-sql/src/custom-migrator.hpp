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
    std::vector<{ name: string; expression: string }> checkConstraints;

// Known composite primary keys for tables that don't have proper metadata
  // Add other tables with composite primary keys here if needed

class DrizzleSchemaIntrospector {
  parseTableDefinition(table: any, exportKey?: string): TableDefinition {
    const tableName = this.getTableName(table, exportKey);

    const columns = this.parseColumns(table);
    const foreignKeys = this.parseForeignKeys(table);
    const indexes = this.parseIndexes(table);
    const checkConstraints = this.parseCheckConstraints(table);
    let compositePrimaryKey = this.parseCompositePrimaryKey(table);

    // Fallback to known composite primary keys if not found
    if (!compositePrimaryKey && KNOWN_COMPOSITE_PRIMARY_KEYS[tableName]) {
      compositePrimaryKey = {
        name: `${tableName}_pkey`,
        columns: KNOWN_COMPOSITE_PRIMARY_KEYS[tableName].columns,
      };
      logger.debug(`[INTROSPECTOR] Using known composite primary key for ${tableName}`);
    }

    // Build dependencies list from foreign keys, excluding self-references
    const dependencies = Array.from(
      new Set(
        foreignKeys.map((fk) => fk.referencedTable).filter((refTable) => refTable !== tableName) // Exclude self-references
      )
    );

    return {
      name: tableName,
      columns,
      indexes,
      foreignKeys,
      checkConstraints,
      dependencies,
      compositePrimaryKey,
    };
  }

    // logger.debug(`[INTROSPECTOR] Getting table name for table:`, {
    //   hasTableConfig: !!(table && table._),
    //   tableName: table && table._ && table._.name,
    //   exportKey,
    //   tableKeys: table ? Object.keys(table) : [],
    //   hasSymbols: table ? Object.getOwnPropertySymbols(table).length > 0 : false,
    // });

    // Method 1: Direct access via table._.name
      // logger.debug(`[INTROSPECTOR] Found table name via table._.name: ${table._.name}`);

    // Method 2: Symbol-based table name access
          // logger.debug(`[INTROSPECTOR] Found table name via symbol: ${tableName}`);

    // Method 3: Use OriginalName symbol as fallback
          // logger.debug(`[INTROSPECTOR] Found table name via OriginalName symbol: ${tableName}`);

    // Method 4: Use the key as fallback
      // Convert camelCase key to snake_case table name
      // logger.debug(`[INTROSPECTOR] Using key fallback: ${tableName} (from ${exportKey})`);

    // logger.debug(
    //   `[INTROSPECTOR] Using fallback table name: unknown_table (from ${exportKey || 'no-key'})`
    // );

    // logger.debug(`[INTROSPECTOR] Parsing columns fallback for table. Keys:`, Object.keys(table));

    // Parse columns directly from table object properties

      // logger.debug(`[INTROSPECTOR] Examining column ${key}:`, {
      //   hasColumnType: !!col.columnType,
      //   hasConfig: !!col.config,
      //   hasDataType: !!col.dataType,
      //   configKeys: col.config ? Object.keys(col.config) : [],
      //   colKeys: Object.keys(col),
      // });

      // Check if this looks like a Drizzle column

        // logger.debug(`[INTROSPECTOR] Processing column ${columnName}:`, {
        //   type: col.columnType,
        //   primaryKey: config.primaryKey || config.primary,
        //   notNull: config.notNull,
        //   hasDefault: !!config.default || !!config.defaultValue,
        //   defaultValue: config.default || config.defaultValue,
        //   hasReferences: !!config.references,
        // });

    // logger.debug(
    //   `[INTROSPECTOR] Parsed ${columns.length} columns:`,
    //   columns.map((c) => ({ name: c.name, type: c.type, hasDefault: !!c.defaultValue }))
    // );

    // logger.debug(`[INTROSPECTOR] Parsing foreign keys. Has table._:`, !!tableConfig);

    // Check inline foreign keys first

      // logger.debug(`[INTROSPECTOR] Found ${inlineForeignKeys.length} inline foreign keys in symbol`);

        // logger.debug(`[INTROSPECTOR] Processing foreign key:`, {
        //   hasReference: !!(fk && fk.reference),
        //   onDelete: fk?.onDelete,
        //   onUpdate: fk?.onUpdate,
        //   referenceType: typeof fk?.reference,
        // });

            // logger.debug(`[INTROSPECTOR] Reference function result:`, {
            //   hasTableDef: !!(referenceResult && referenceResult.table),
            //   hasMetadata: !!(referenceResult && referenceResult.table && referenceResult.table._),
            //   tableName:
            //     referenceResult && referenceResult.table
            //       ? this.getTableName(referenceResult.table, '')
            //       : undefined,
            //   resultKeys: referenceResult ? Object.keys(referenceResult) : [],
            //   hasName: !!(referenceResult && referenceResult.name),
            //   hasForeignTable: !!(referenceResult && referenceResult.foreignTable),
            //   hasColumns: !!(referenceResult && referenceResult.columns),
            //   hasForeignColumns: !!(referenceResult && referenceResult.foreignColumns),
            // });

            // Extract referenced table name using multiple methods

            // Method 1: Use our enhanced extraction method

            // Method 2: Direct properties from reference result
              // Ensure it's a string, not a table object

            // Method 3: Extract from name if it looks like a table name
              // Ensure it's a string, not a table object

            // Method 4: If we still have the table object, extract the name

            // Extract column information

            // Method 1: Direct column arrays

            // Method 2: Extract from foreign key structure patterns
              // Try to infer from common naming patterns
                // Complex table has multiple foreign keys

              // Default to 'id' for referenced columns

            // Final safety check: ensure referencedTableName is a string, not an object

              // logger.debug(`[INTROSPECTOR] Created foreign key:`, foreignKey);

    // Fallback: Try to extract from table config if no inline FKs found
      // Add any additional fallback logic here if needed

    // logger.debug(`[INTROSPECTOR] Found ${foreignKeys.length} foreign keys:`, foreignKeys);

    // Method 1: Direct table name access

    // Method 2: Symbol-based table name access

    // Method 3: Handle reference function result objects
    // When we call a reference function, it returns an object with foreignTable property

    // Method 4: Extract from name property (common in reference results)

    // Method 5: Check if the reference itself is a function and try to extract table info
        // Try to call the reference function to get the actual table reference

    // Method 6: Check for table property with different structures
      // Try to get table name from constructor or other properties

      // Check if it's a table-like object with a name property

      // Check constructor name for hints

    // Check for indexes in the standard location

    // Also check for unique constraints in the extraConfigBuilder

    // Enhanced: Check for constraints in table symbol properties
      // logger.debug(`[INTROSPECTOR] No indexes found, checking symbols for constraints`);
        // Look for symbols that might contain constraint information
        // logger.debug(`[INTROSPECTOR] Found ${symbols.length} symbols to check`);

          // logger.debug(
          //   `[INTROSPECTOR] Checking symbol ${symbol.description} (isArray: ${Array.isArray(symbolValue)}, type: ${typeof symbolValue})`
          // );

                // logger.debug(`[INTROSPECTOR] Symbol array item:`, {
                //   hasName: !!item.name,
                //   hasColumns: !!item.columns,
                //   hasUnique: item.unique !== undefined,
                //   name: item.name,
                //   unique: item.unique,
                //   itemKeys: Object.keys(item),
                // });

                // Check for unique constraints
                  // logger.debug(`[INTROSPECTOR] Found constraint in symbol: ${item.name}`);

    // Enhanced: Try to extract constraints from table structure patterns

      // Check if this is a test table that should have constraints
        // Add expected base_entities unique constraint
        // Add expected dependent_entities unique constraint
        // Add expected complex_relations unique constraint

    // Check for check constraints in extraConfigBuilder

    // Pattern-based check constraints for test tables

    // If no direct _ property, check symbols
        // Look for the TableConfig symbol which contains extraConfigBuilder

        // Handle both array and object extraConfig
              // Extract column names from the primary key definition
          // Handle object form of extraConfig (e.g., { pk: primaryKey(...) })
            // Check if this is a primary key definition

                // Extract column names from the primary key definition
                  // Handle column objects that have a name property
                  // Handle string column names
                  // Fallback

    // Check if this is a vector column by name pattern

    // Handle numberTimestamp specifically

        // Check if it's a vector column

    // Check if this is a vector column by name pattern (dim384, dim_384, etc.)

    // logger.debug(`[INTROSPECTOR] Formatting default value:`, {
    //   type: typeof defaultValue,
    //   value: defaultValue,
    //   hasQueryChunks: !!(defaultValue && defaultValue.queryChunks),
    //   constructorName: defaultValue?.constructor?.name,
    // });

    // Handle SQL template literals
        // logger.debug(`[INTROSPECTOR] Using SQL property: ${defaultValue.sql}`);
        // logger.debug(`[INTROSPECTOR] Using queryChunks: ${result}`);
      // Handle empty object for JSONB defaults
          // logger.debug(`[INTROSPECTOR] Empty object default for JSONB: '{}'`);
      // Handle SQL constructor objects (like now())
        // logger.debug(`[INTROSPECTOR] SQL object detected, checking for known patterns`);
        // Try to extract the actual SQL from the object
        // Fallback for SQL objects

      // logger.debug(`[INTROSPECTOR] String default: '${defaultValue}'`);
      // logger.debug(`[INTROSPECTOR] Primitive default: ${defaultValue}`);

  // Create table SQL without foreign key constraints
        // Only add PRIMARY KEY for single column primary keys if no composite primary key exists
          // Handle different types of defaults

    // Add unique constraints (but not foreign keys)

    // Add composite primary key if it exists

    // Add unique constraints

  // Generate foreign key constraint SQL

class PluginNamespaceManager {
  constructor(private db: DrizzleDB) {}

      // For the core SQL plugin, try to use the current schema if available (for PG)
      // Otherwise, default to public.
          // The search_path can be a comma-separated list, iterate to find the first valid schema
        // This query might fail on PGLite if not supported, fallback to public

      // If the query fails, assume the constraint doesn't exist

      // If the query fails, assume the constraint doesn't exist

      // If the query fails, assume the constraint doesn't exist

    // Add foreign key constraints

          // Check if foreign key already exists

          // Log the error but continue processing other constraints

    // Add check constraints
          // Check if check constraint already exists

class ExtensionManager {
  constructor(private db: DrizzleDB) {}

// Topological sort for dependency ordering
std::vector<std::string> topologicalSort(Map<string tables, auto TableDefinition>);

std::future<void> runPluginMigrations(DrizzleDB db, const std::string& pluginName, const std::any& schema);

} // namespace elizaos
