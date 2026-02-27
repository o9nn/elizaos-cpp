#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC_PG_ADAPTER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_SQL_SRC_PG_ADAPTER_H
#include "core.hpp"
// External dependency removed
#include "drizzle-orm/node-postgres.h"
#include "../base.h"
#include "../schema/embedding.h"
#include "./manager.h"

class PgDatabaseAdapter;

class PgDatabaseAdapter : public BaseDrizzleAdapter, public std::enable_shared_from_this<PgDatabaseAdapter> {
public:
    using std::enable_shared_from_this<PgDatabaseAdapter>::shared_from_this;
    EmbeddingDimensionColumn embeddingDimension = const_(DIMENSION_MAP)[384];

    std::shared_ptr<PostgresConnectionManager> manager;

    PgDatabaseAdapter(std::shared_ptr<UUID> agentId, std::shared_ptr<PostgresConnectionManager> manager, any _schema = undefined);
    virtual std::shared_ptr<Promise<void>> runMigrations();
    template <typename T>
    std::shared_ptr<Promise<T>> withDatabase(std::function<std::shared_ptr<Promise<T>>()> operation);
    virtual std::shared_ptr<Promise<void>> init();
    virtual std::shared_ptr<Promise<boolean>> isReady();
    virtual std::shared_ptr<Promise<void>> close();
    virtual any getConnection();
    virtual std::shared_ptr<Promise<boolean>> createAgent(std::shared_ptr<Agent> agent);
    virtual std::shared_ptr<Promise<any>> getAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<boolean>> updateAgent(std::shared_ptr<UUID> agentId, Partial<std::shared_ptr<Agent>> agent);
    virtual std::shared_ptr<Promise<boolean>> deleteAgent(std::shared_ptr<UUID> agentId);
    virtual std::shared_ptr<Promise<boolean>> createEntities(array<std::shared_ptr<Entity>> entities);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Entity>>>> getEntityByIds(array<std::shared_ptr<UUID>> entityIds);
    virtual std::shared_ptr<Promise<void>> updateEntity(std::shared_ptr<Entity> entity);
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createMemory(std::shared_ptr<Memory> memory, string tableName);
    virtual std::shared_ptr<Promise<any>> getMemoryById(std::shared_ptr<UUID> memoryId);
    virtual std::shared_ptr<Promise<array<any>>> searchMemories(any params);
    virtual std::shared_ptr<Promise<boolean>> updateMemory(any memory);
    virtual std::shared_ptr<Promise<void>> deleteMemory(std::shared_ptr<UUID> memoryId);
    virtual std::shared_ptr<Promise<boolean>> createComponent(std::shared_ptr<Component> component);
    virtual std::shared_ptr<Promise<any>> getComponent(std::shared_ptr<UUID> entityId, string type, std::shared_ptr<UUID> worldId = undefined, std::shared_ptr<UUID> sourceEntityId = undefined);
    virtual std::shared_ptr<Promise<void>> updateComponent(std::shared_ptr<Component> component);
    virtual std::shared_ptr<Promise<void>> deleteComponent(std::shared_ptr<UUID> componentId);
    PgDatabaseAdapter(std::shared_ptr<UUID> agentId);
};

template <typename T>
std::shared_ptr<Promise<T>> PgDatabaseAdapter::withDatabase(std::function<std::shared_ptr<Promise<T>>()> operation)
{
    return std::async([=]() { this->withRetry([=]() mutable
    {
        auto client = std::async([=]() { this->manager->getClient(); });
        {
            utils::finally __finally1827_1863([&]() mutable
            {
                client->release();
            });
            try
            {
                auto db = drizzle(as<any>(client));
                this->db = db;
                return std::async([=]() { operation(); });
            }
            catch (...)
            {
                throw;
            }
        }
    }
    ); });
}

#endif
