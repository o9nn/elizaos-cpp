#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/migration-service.h"

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
    console->log(std::string("[MIGRATION SERVICE] discoverAndRegisterPluginSchemas called with ") + plugins->get_length() + std::string(" plugins"));
    for (auto& plugin : plugins)
    {
        console->log(std::string("[MIGRATION SERVICE] Checking plugin: ") + plugin->name + std::string(", has schema: ") + !!plugin->schema + string_empty);
        if (plugin->schema) {
            console->log(std::string("[MIGRATION SERVICE] Schema keys for ") + plugin->name + std::string(":"), Object->keys(plugin->schema));
            this->registeredSchemas->set(plugin->name, plugin->schema);
            logger->info(std::string("Registered schema for plugin: ") + plugin->name + string_empty);
        }
    }
    console->log(std::string("[MIGRATION SERVICE] Total registered schemas: ") + this->registeredSchemas->size + string_empty);
    logger->info(std::string("Discovered ") + this->registeredSchemas->size + std::string(" plugin schemas out of ") + plugins->get_length() + std::string(" plugins"));
}

std::shared_ptr<Promise<void>> DatabaseMigrationService::runAllPluginMigrations()
{
    if (!this->db) {
        throw any(std::make_shared<Error>(std::string("Database not initialized in DatabaseMigrationService")));
    }
    console->log(std::string("[MIGRATION SERVICE] Running migrations for ") + this->registeredSchemas->size + std::string(" plugins..."));
    logger->info(std::string("Running migrations for ") + this->registeredSchemas->size + std::string(" plugins..."));
    for (auto& [pluginName, schema] : this->registeredSchemas)
    {
        console->log(std::string("[MIGRATION SERVICE] Starting migration for plugin: ") + pluginName + string_empty);
        logger->info(std::string("Starting migration for plugin: ") + pluginName + string_empty);
        std::async([=]() { runPluginMigrations(this->db, pluginName, schema); });
        console->log(std::string("[MIGRATION SERVICE] Completed migration for plugin: ") + pluginName + string_empty);
    }
    console->log(std::string("[MIGRATION SERVICE] All plugin migrations completed."));
    logger->info(std::string("All plugin migrations completed."));
    return std::shared_ptr<Promise<void>>();
}

