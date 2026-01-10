#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/migration-service.h"

DatabaseMigrationService::DatabaseMigrationService() {
}

std::shared_ptr<Promise<void>> DatabaseMigrationService::initializeWithDatabase(std::shared_ptr<DrizzleDatabase> db)
{
    this->db = db;
    logger->info(std::string("DatabaseMigrationService initialized with database"));
    return std::shared_ptr<Promise<void>>();
}

void DatabaseMigrationService::discoverAndRegisterPluginSchemas(array<std::shared_ptr<Plugin>> plugins)
{
    for (auto& plugin : plugins)
    {
        if (plugin->schema) {
            this->registeredSchemas->set(plugin->name, plugin->schema);
            logger->info(std::string("Registered schema for plugin: ") + plugin->name + string_empty);
        }
    }
    logger->info(std::string("Discovered ") + this->registeredSchemas->size + std::string(" plugin schemas out of ") + plugins->get_length() + std::string(" plugins"));
}

std::shared_ptr<Promise<void>> DatabaseMigrationService::runAllPluginMigrations()
{
    if (!this->db) {
        throw any(std::make_shared<Error>(std::string("Database not initialized in DatabaseMigrationService")));
    }
    logger->info(std::string("Running migrations for ") + this->registeredSchemas->size + std::string(" plugins..."));
    for (auto& [pluginName, schema] : this->registeredSchemas)
    {
        logger->info(std::string("Starting migration for plugin: ") + pluginName + string_empty);
        std::async([=]() { runPluginMigrations(this->db, pluginName, schema); });
    }
    logger->info(std::string("All plugin migrations completed."));
    return std::shared_ptr<Promise<void>>();
}

