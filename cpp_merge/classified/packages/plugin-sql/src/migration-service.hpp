#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_MIGRATION_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_MIGRATION_SERVICE_H
#include "core.hpp"
// External dependency removed
#include "./custom-migrator.h"
#include "./types.h"

class DatabaseMigrationService;

class DatabaseMigrationService : public object, public std::enable_shared_from_this<DatabaseMigrationService> {
public:
    using std::enable_shared_from_this<DatabaseMigrationService>::shared_from_this;
    any db = nullptr;

    std::shared_ptr<Map<string, any>> registeredSchemas = std::make_shared<Map<string, any>>();

    DatabaseMigrationService();
    virtual std::shared_ptr<Promise<void>> initializeWithDatabase(std::shared_ptr<DrizzleDatabase> db);
    virtual void discoverAndRegisterPluginSchemas(array<std::shared_ptr<Plugin>> plugins);
    virtual std::shared_ptr<Promise<void>> runAllPluginMigrations();
};

#endif
