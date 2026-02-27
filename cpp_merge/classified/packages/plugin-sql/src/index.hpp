#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "./pglite/adapter.h"
#include "./pglite/manager.h"
#include "./pg/adapter.h"
#include "./pg/manager.h"
#include "./utils.h"
#include "./schema.h"

class GlobalSingletons;

extern GLOBAL_SINGLETONS GLOBAL_SINGLETONS;
class GlobalSingletons : public object, public std::enable_shared_from_this<GlobalSingletons> {
public:
    using std::enable_shared_from_this<GlobalSingletons>::shared_from_this;
    std::shared_ptr<PGliteClientManager> pgLiteClientManager;

    std::shared_ptr<PostgresConnectionManager> postgresConnectionManager;
};

extern std::shared_ptr<Record<any, std::shared_ptr<GlobalSingletons>>> globalSymbols;
extern any globalSingletons;
std::shared_ptr<IDatabaseAdapter> createDatabaseAdapter(object config, std::shared_ptr<UUID> agentId);

extern std::shared_ptr<Plugin> plugin;
#endif
