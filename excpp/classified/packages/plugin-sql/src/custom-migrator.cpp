#include "custom-migrator.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractErrorMessage(unknown error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error && 'cause' in error && error.cause) {
        return (error.cause).message;
        } else if (true /* instanceof check */) {
            return error.message;
        }
        return 'Unknown error';

}

void extractErrorDetails(unknown error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    message: string; stack?: string
}

std::string camelToSnakeCase(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "_" + std::to_string(letter.toLowerCase());

}

std::vector<std::string> topologicalSort(Map<string tables, auto TableDefinition>) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> sorted = [];
    const auto visited = new Set<string>();
    const auto visiting = new Set<string>();

    function visit(tableName: string) {
        if (visiting.has(tableName)) {
            std::cout << "Circular dependency detected involving table: " + std::to_string(tableName) << std::endl;
            return;
        }

        if (visited.has(tableName)) {
            return;
        }

        visiting.add(tableName);

        const auto table = tables.get(tableName);
        if (table) {
            // Visit dependencies first
            for (const auto& dep : table.dependencies)
                if (tables.has(dep)) {
                    visit(dep);
                }
            }
        }

        visiting.delete(tableName);
        visited.add(tableName);
        sorted.push(tableName);
    }

    // Visit all tables
    for (const auto& tableName : tables.keys()
        visit(tableName);
    }

    return sorted;

}

std::future<void> runPluginMigrations(DrizzleDB db, const std::string& pluginName, const std::any& schema) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "[CUSTOM MIGRATOR] Starting migration for plugin: " + std::to_string(pluginName) << std::endl;
        std::cout << "[CUSTOM MIGRATOR] Schema type:" << typeof schema << std::endl;
        console.log(
        "[CUSTOM MIGRATOR] Schema keys:"
        schema ? Object.keys(schema) : 'schema is nullptr/std::nullopt'
        );
        logger.debug(`[CUSTOM MIGRATOR] Starting migration for plugin: ${pluginName}`);

        // Test database connection first
        try {
            db.execute(sql.raw('SELECT 1'));
            std::cout << '[CUSTOM MIGRATOR] Database connection verified' << std::endl;
            logger.debug('[CUSTOM MIGRATOR] Database connection verified');
            } catch (error) {
                const auto errorDetails = extractErrorDetails(error);
                std::cerr << "[CUSTOM MIGRATOR] Database connection failed: " + std::to_string(errorDetails.message) << std::endl;
                if (errorDetails.stack) {
                    std::cerr << "[CUSTOM MIGRATOR] Stack trace: " + std::to_string(errorDetails.stack) << std::endl;
                }
                throw std::runtime_error(`Database connection failed: ${errorDetails.message}`);
            }

            const auto namespaceManager = new PluginNamespaceManager(db);
            const auto introspector = new DrizzleSchemaIntrospector();
            const auto extensionManager = new ExtensionManager(db);

            extensionManager.installRequiredExtensions(['vector', 'fuzzystrmatch']);
            const auto schemaName = namespaceManager.getPluginSchema(pluginName);
            std::cout << "[CUSTOM MIGRATOR] Using schema: " + std::to_string(schemaName) << std::endl;
            namespaceManager.ensureNamespace(schemaName);
            const auto existingTables = namespaceManager.introspectExistingTables(schemaName);

            std::cout << "[CUSTOM MIGRATOR] Schema name: " + std::to_string(schemaName) << std::endl;
            std::cout << "[CUSTOM MIGRATOR] Existing tables:" << existingTables << std::endl;

            // logger.debug(`[CUSTOM MIGRATOR] Schema name: ${schemaName}`);
            // logger.debug(`[CUSTOM MIGRATOR] Existing tables:`, existingTables);

            // Discover all tables
            const auto tableEntries = Object.entries(schema).filter(([key, v]) => {;
                const auto isDrizzleTable =;
                v &&;
                (((v)._ && typeof (v)._.name == 'string') ||;
                (typeof v == 'object' &&;
                v != nullptr &&;
                ('tableName' in v || 'dbName' in v || key.toLowerCase().includes('table'))));
                return isDrizzleTable;
                });

                std::cout << "[CUSTOM MIGRATOR] Found " + std::to_string(tableEntries.length) + " table entries in schema" << std::endl;
                console.log(
                "[CUSTOM MIGRATOR] Table keys:"
                tableEntries.map(([key]) => key);
                );
                // logger.debug(
                //   `[CUSTOM MIGRATOR] Found ${tableEntries.length} tables to process:`,
                //   tableEntries.map(([key]) => key)
                // );

                // Parse all table definitions
                const auto tableDefinitions = new Map<string, TableDefinition>();
                for (const int [exportKey, table] of tableEntries) {
                    const auto tableDef = introspector.parseTableDefinition(table, exportKey);
                    tableDefinitions.set(tableDef.name, tableDef);
                }

                // Sort tables by dependencies (topological sort)
                const auto sortedTableNames = topologicalSort(tableDefinitions);
                // logger.debug(`[CUSTOM MIGRATOR] Table creation order:`, sortedTableNames);

                // logger.info(
                //   `Migrating ${tableDefinitions.size} tables for ${pluginName} to schema ${schemaName}`
                // );

                try {
                    // Phase 1: Create all tables without foreign key constraints
                    std::cout << "[CUSTOM MIGRATOR] Phase 1: Creating tables..." << std::endl;
                    logger.debug(`[CUSTOM MIGRATOR] Phase 1: Creating tables...`);
                    for (const auto& tableName : sortedTableNames)
                        const auto tableDef = tableDefinitions.get(tableName);
                        if (!tableDef) continue;

                        const auto tableExists = existingTables.includes(tableDef.name);
                        logger.debug(`[CUSTOM MIGRATOR] Table ${tableDef.name} exists: ${tableExists}`);

                        if (!tableExists) {
                            // Special handling for tables that require extensions
                            if (tableDef.name == 'embeddings') {
                                // Check if pgvector extension is available
                                try {
                                    db.execute(sql.raw("SELECT 1 FROM pg_extension WHERE extname = 'vector'"));
                                    std::cout << `[CUSTOM MIGRATOR] pgvector extension found << creating embeddings table` << std::endl;
                                    } catch (extensionError) {
                                        console.warn(
                                        "[CUSTOM MIGRATOR] Skipping embeddings table - pgvector extension not available";
                                        );
                                        logger.warn(
                                        "[CUSTOM MIGRATOR] Skipping embeddings table - pgvector extension not available";
                                        );
                                        continue;
                                    }
                                }

                                std::cout << "[CUSTOM MIGRATOR] Creating table: " + std::to_string(tableDef.name) << std::endl;
                                logger.debug(`[CUSTOM MIGRATOR] Creating table: ${tableDef.name}`);
                                try {
                                    namespaceManager.createTable(tableDef, schemaName);
                                    std::cout << "[CUSTOM MIGRATOR] Successfully created table: " + std::to_string(tableDef.name) << std::endl;
                                    } catch (error) {
                                        const auto errorDetails = extractErrorDetails(error);
                                        console.error(
                                        "[CUSTOM MIGRATOR] Failed to create table " + std::to_string(tableDef.name) + ":"
                                        errorDetails.message;
                                        );
                                        logger.error(
                                        "[CUSTOM MIGRATOR] Failed to create table " + std::to_string(tableDef.name) + ": " + std::to_string(errorDetails.message)
                                        );
                                        if (errorDetails.stack) {
                                            std::cerr << "[CUSTOM MIGRATOR] Table creation stack trace: " + std::to_string(errorDetails.stack) << std::endl;
                                        }
                                        throw std::runtime_error(`Failed to create table ${tableDef.name}: ${errorDetails.message}`);
                                    }
                                    } else {
                                        std::cout << "[CUSTOM MIGRATOR] Table " + std::to_string(tableDef.name) << skipping` << std::endl;
                                        logger.debug(`[CUSTOM MIGRATOR] Table ${tableDef.name} already exists, skipping creation`);
                                    }
                                }

                                // Phase 2: Add constraints (foreign keys, check constraints, etc.)
                                logger.debug(`[CUSTOM MIGRATOR] Phase 2: Adding constraints...`);
                                for (const auto& tableName : sortedTableNames)
                                    const auto tableDef = tableDefinitions.get(tableName);
                                    if (!tableDef) continue;

                                    // Add constraints if table has foreign keys OR check constraints
                                    if (tableDef.foreignKeys.length > 0 || tableDef.checkConstraints.length > 0) {
                                        logger.debug(`[CUSTOM MIGRATOR] Adding constraints for table: ${tableDef.name}`, {
                                            foreignKeys: tableDef.foreignKeys.length,
                                            checkConstraints: tableDef.checkConstraints.length,
                                            });
                                            namespaceManager.addConstraints(tableDef, schemaName);
                                        }
                                    }

                                    logger.debug(`[CUSTOM MIGRATOR] Completed migration for plugin: ${pluginName}`);
                                    } catch (error) {
                                        const auto errorDetails = extractErrorDetails(error);
                                        logger.error(
                                        "[CUSTOM MIGRATOR] Migration failed for plugin " + std::to_string(pluginName) + ": " + std::to_string(errorDetails.message)
                                        );
                                        if (errorDetails.stack) {
                                            std::cerr << "[CUSTOM MIGRATOR] Migration stack trace: " + std::to_string(errorDetails.stack) << std::endl;
                                        }
                                        throw std::runtime_error(`Migration failed for plugin ${pluginName}: ${errorDetails.message}`);
                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
