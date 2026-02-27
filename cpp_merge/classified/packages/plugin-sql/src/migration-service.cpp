#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/migration-service.h"

DatabaseMigrationService::DatabaseMigrationService() {
}

std::shared_ptr<Promise<void>> DatabaseMigrationService::initializeWithDatabase(std::shared_ptr<DrizzleDatabase> db)
{
    this->db = db;
    logger->info(std:("DatabaseMigrationService initialized with database"));
    return std::shared_ptr<Promise<void>>();
}

void DatabaseMigrationService::discoverAndRegisterPluginSchemas(array<std::shared_ptr<Plugin>> plugins)
{
    console->log(std:("[MIGRATION SERVICE] discoverAndRegisterPluginSchemas called with ") + plugins->get_length() + std:(" plugins"));
    for (auto& plugin : plugins)
    {
        console->log(std:("[MIGRATION SERVICE] Checking plugin: ") + plugin->name + std:(", has schema: ") + !!plugin->schema + string_empty);
        if (plugin->schema) {
            console->log(std:("[MIGRATION SERVICE] Schema keys for ") + plugin->name + std:(":"), Object->keys(plugin->schema));
            this->registeredSchemas->set(plugin->name, plugin->schema);
            logger->info(std:("Registered schema for plugin: ") + plugin->name + string_empty);
        }
    }
    console->log(std:("[MIGRATION SERVICE] Total registered schemas: ") + this->registeredSchemas->size + string_empty);
    logger->info(std:("Discovered ") + this->registeredSchemas->size + std:(" plugin schemas out of ") + plugins->get_length() + std:(" plugins"));
}

std::shared_ptr<Promise<void>> DatabaseMigrationService::runAllPluginMigrations()
{
    if (!this->db) {
        throw any(std::make_shared<Error>(std:("Database not initialized in DatabaseMigrationService")));
    }
    console->log(std:("[MIGRATION SERVICE] Running migrations for ") + this->registeredSchemas->size + std:(" plugins..."));
    logger->info(std:("Running migrations for ") + this->registeredSchemas->size + std:(" plugins..."));
    for (auto& [pluginName, schema] : this->registeredSchemas)
    {
        console->log(std:("[MIGRATION SERVICE] Starting migration for plugin: ") + pluginName + string_empty);
        logger->info(std:("Starting migration for plugin: ") + pluginName + string_empty);
        std::async([=]() { runPluginMigrations(this->db, pluginName, schema); });
        console->log(std:("[MIGRATION SERVICE] Completed migration for plugin: ") + pluginName + string_empty);
    }
    console->log(std:("[MIGRATION SERVICE] All plugin migrations completed."));
    logger->info(std:("All plugin migrations completed."));
    return std::shared_ptr<Promise<void>>();
}

