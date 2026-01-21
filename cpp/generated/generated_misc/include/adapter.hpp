#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC_PGLITE_ADAPTER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PLUGIN-SQL_SRC_PGLITE_ADAPTER_H
#include "core.h"
#include "@elizaos/core.h"
#include "drizzle-orm/pglite.h"
#include "../base.h"
#include "../schema/embedding.h"
#include "./manager.h"

class PgliteDatabaseAdapter;

class PgliteDatabaseAdapter : public BaseDrizzleAdapter, public std::enable_shared_from_this<PgliteDatabaseAdapter> {
public:
    using std::enable_shared_from_this<PgliteDatabaseAdapter>::shared_from_this;
    std::shared_ptr<PGliteClientManager> manager;

    EmbeddingDimensionColumn embeddingDimension = const_(DIMENSION_MAP)[384];

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
