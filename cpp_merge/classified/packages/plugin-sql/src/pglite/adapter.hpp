#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_PGLITE_ADAPTER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SQL_SRC_PGLITE_ADAPTER_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "drizzle-orm/pglite.h"
#include "../base.h"
#include "../schema.h"
#include "../schema/embedding.h"
#include "./manager.h"

class PgliteDatabaseAdapter;

class PgliteDatabaseAdapter : public BaseDrizzleAdapter, public std::enable_shared_from_this<PgliteDatabaseAdapter> {
public:
    using std::enable_shared_from_this<PgliteDatabaseAdapter>::shared_from_this;
    EmbeddingDimensionColumn embeddingDimension = const_(DIMENSION_MAP)[768];

    std::shared_ptr<PGliteClientManager> manager;

    PgliteDatabaseAdapter(std::shared_ptr<UUID> agentId, std::shared_ptr<PGliteClientManager> manager);
    virtual std::shared_ptr<Promise<void>> runMigrations();
    template <typename T>
    std::shared_ptr<Promise<T>> withDatabase(std::function<std::shared_ptr<Promise<T>>()> operation);
    virtual std::shared_ptr<Promise<void>> init();
    virtual std::shared_ptr<Promise<boolean>> isReady();
    virtual void close();
    virtual any getConnection();
    PgliteDatabaseAdapter(std::shared_ptr<UUID> agentId);
};

template <typename T>
std::shared_ptr<Promise<T>> PgliteDatabaseAdapter::withDatabase(std::function<std::shared_ptr<Promise<T>>()> operation)
{
    if (this->manager->isShuttingDown()) {
        logger->warn(std::string("Database is shutting down"));
        return as<T>(as<any>(nullptr));
    }
    return operation();
}

#endif
