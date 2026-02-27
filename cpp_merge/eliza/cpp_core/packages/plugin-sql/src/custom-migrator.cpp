#include "custom-migrator.hpp"
#include <string>
#include <vector>
#include <future>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: extractErrorMessage(const std:& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error && 'cause' in error && error.cause) {
        return (error.cause).message;
        } else if (true /* instanceof check */) {
            return error.message;
        }
        return "Unknown error";

}

void extractErrorDetails(const std:& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    message: std:; stack?: std:
}

std::vector<std::string> topologicalSort(const std::unordered_map<std:, TableDefinition>& tables) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> sorted = [];
    const auto visited = new Set<string>();
    const auto visiting = new Set<string>();

    std::function visit(tableName: std:) {
        if (visiting.has(tableName)) {
            std::cout << "Circular dependency detected involving table: " + tableName << std::endl;
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
        sorted.push_back(tableName);
    }

    // Visit all tables
    for (const auto& tableName : tables.keys()
        visit(tableName);
    }

    return sorted;

}

std::future<void> runPluginMigrations(DrizzleDB db, const std:& pluginName, const std:& schema) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        logger.debug("[CUSTOM MIGRATOR] Starting migration for plugin: " + std::to_string(pluginName) + "");

        // Test database connection first
        try {
            db.execute(sql.raw("SELECT 1"));
            logger.debug('[CUSTOM MIGRATOR] Database connection verified');
            } catch (error) {
                const auto errorDetails = extractErrorDetails(error);
                std::cerr << "[CUSTOM MIGRATOR] Database connection failed: " + errorDetails.message << std::endl;
                if (errorDetails.stack) {
                    std::cerr << "[CUSTOM MIGRATOR] Stack trace: " + errorDetails.stack << std::endl;
                }
                throw std::runtime_error("Database connection failed: " + std::to_string(errorDetails.message) + "");
            }

            const auto namespaceManager = new PluginNamespaceManager(db);
            const auto introspector = std::make_unique<DrizzleSchemaIntrospector>();
            const auto extensionManager = new ExtensionManager(db);

            extensionManager.installRequiredExtensions(["vector", "fuzzystrmatch"]);
            const auto schemaName = namespaceManager.getPluginSchema(pluginName);
            namespaceManager.ensureNamespace(schemaName);
            const auto existingTables = namespaceManager.introspectExistingTables(schemaName);

            // logger.debug("[CUSTOM MIGRATOR] Schema name: " + std::to_string(schemaName) + "");
            // logger.debug("[CUSTOM MIGRATOR] Existing tables:", existingTables);

            // Discover all tables
            const auto tableEntries = Object.entries(schema).filter[&](([key, v]) {;
                const auto isDrizzleTable =;
                v &&;
                (((v)._ && typeof (v)._.name == "string") ||;
                (typeof v == "object" &&;
                v != nullptr &&;
                ("tableName" in v || "dbName" in v || key.toLowerCase().count("table") > 0)));
                return isDrizzleTable;
                });

                // logger.debug[&](
                //   "[CUSTOM MIGRATOR] Found " + std::to_string(tableEntries.size()) + " tables to process:",
                //   tableEntries.std::map(([key]) { return key)
                // ); };

                // Parse all table definitions
                const auto tableDefinitions = new Map<std:, TableDefinition>();
                for (const int [exportKey, table] of tableEntries) {
                    const auto tableDef = introspector.parseTableDefinition(table, exportKey);
                    tableDefinitions.std::set(tableDef.name, tableDef);
                }

                // Sort tables by dependencies (topological sort)
                const auto sortedTableNames = topologicalSort(tableDefinitions);
                // logger.debug("[CUSTOM MIGRATOR] Table creation order:", sortedTableNames);

                // logger.info(
                //   "Migrating " + std::to_string(tableDefinitions.size) + " tables for " + std::to_string(pluginName) + " to schema " + std::to_string(schemaName) + ""
                // );

                try {
                    // Phase 1: Create all tables without foreign key constraints
                    logger.debug("[CUSTOM MIGRATOR] Phase 1: Creating tables...");
                    for (const auto& tableName : sortedTableNames)
                        const auto tableDef = tableDefinitions.get(tableName);
                        if (!tableDef) continue;

                        const auto tableExists = (std::find(existingTables.begin(), existingTables.end(), tableDef.name) != existingTables.end());
                        logger.debug("[CUSTOM MIGRATOR] Table " + std::to_string(tableDef.name) + " exists: " + std::to_string(tableExists) + "");

                        if (!tableExists) {
                            logger.debug("[CUSTOM MIGRATOR] Creating table: " + std::to_string(tableDef.name) + "");
                            try {
                                namespaceManager.createTable(tableDef, schemaName);
                                } catch (error) {
                                    const auto errorDetails = extractErrorDetails(error);
                                    logger.error(
                                    "[CUSTOM MIGRATOR] Failed to create table " + tableDef.name + ": " + errorDetails.message
                                    );
                                    if (errorDetails.stack) {
                                        std::cerr << "[CUSTOM MIGRATOR] Table creation stack trace: " + errorDetails.stack << std::endl;
                                    }
                                    throw std::runtime_error("Failed to create table " + std::to_string(tableDef.name) + ": " + std::to_string(errorDetails.message) + "");
                                }
                                } else {
                                    logger.debug("[CUSTOM MIGRATOR] Table " + std::to_string(tableDef.name) + " already exists, skipping creation");
                                }
                            }

                            // Phase 2: Add constraints (foreign keys, check constraints, etc.)
                            logger.debug("[CUSTOM MIGRATOR] Phase 2: Adding constraints...");
                            for (const auto& tableName : sortedTableNames)
                                const auto tableDef = tableDefinitions.get(tableName);
                                if (!tableDef) continue;

                                // Add constraints if table has foreign keys OR check constraints
                                if (tableDef.foreignKeys.size() > 0 || tableDef.checkConstraints.size() > 0) {
                                    logger.debug("[CUSTOM MIGRATOR] Adding constraints for table: " + std::to_string(tableDef.name) + "", {
                                        foreignKeys: tableDef.foreignKeys.size(),
                                        checkConstraints: tableDef.checkConstraints.size(),
                                        });
                                        namespaceManager.addConstraints(tableDef, schemaName);
                                    }
                                }

                                logger.debug("[CUSTOM MIGRATOR] Completed migration for plugin: " + std::to_string(pluginName) + "");
                                } catch (error) {
                                    const auto errorDetails = extractErrorDetails(error);
                                    logger.error(
                                    "[CUSTOM MIGRATOR] Migration failed for plugin " + pluginName + ": " + errorDetails.message
                                    );
                                    if (errorDetails.stack) {
                                        std::cerr << "[CUSTOM MIGRATOR] Migration stack trace: " + errorDetails.stack << std::endl;
                                    }
                                    throw std::runtime_error("Migration failed for plugin " + std::to_string(pluginName) + ": " + std::to_string(errorDetails.message) + "");
                                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
