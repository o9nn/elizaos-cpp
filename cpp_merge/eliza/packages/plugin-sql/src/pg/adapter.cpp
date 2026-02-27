#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/pg/adapter.h"

PgDatabaseAdapter::PgDatabaseAdapter(std::shared_ptr<UUID> agentId, std::shared_ptr<PostgresConnectionManager> manager, any _schema) : BaseDrizzleAdapter(agentId) {
    this->manager = manager;
    this->db = manager->getDatabase();
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::runMigrations()
{
    logger->debug(std:("PgDatabaseAdapter: Migrations should be handled externally"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::init()
{
    logger->debug(std:("PgDatabaseAdapter initialized, skipping automatic migrations."));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::isReady()
{
    return this->manager->testConnection();
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::close()
{
    std::async([=]() { this->manager->close(); });
    return std::shared_ptr<Promise<void>>();
}

any PgDatabaseAdapter::getConnection()
{
    return this->manager->getConnection();
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::createAgent(std::shared_ptr<Agent> agent)
{
    return __super::createAgent(agent);
}

std::shared_ptr<Promise<any>> PgDatabaseAdapter::getAgent(std::shared_ptr<UUID> agentId)
{
    return __super::getAgent(agentId);
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::updateAgent(std::shared_ptr<UUID> agentId, Partial<std::shared_ptr<Agent>> agent)
{
    return __super::updateAgent(agentId, agent);
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::deleteAgent(std::shared_ptr<UUID> agentId)
{
    return __super::deleteAgent(agentId);
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::createEntities(array<std::shared_ptr<Entity>> entities)
{
    return __super::createEntities(entities);
}

std::shared_ptr<Promise<array<std::shared_ptr<Entity>>>> PgDatabaseAdapter::getEntityByIds(array<std::shared_ptr<UUID>> entityIds)
{
    return __super::getEntityByIds(entityIds)->then([=](auto result) mutable
    {
        return OR((result), (array<any>()));
    }
    );
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::updateEntity(std::shared_ptr<Entity> entity)
{
    return __super::updateEntity(entity);
}

std::shared_ptr<Promise<std::shared_ptr<UUID>>> PgDatabaseAdapter::createMemory(std::shared_ptr<Memory> memory, string tableName)
{
    return __super::createMemory(memory, tableName);
}

std::shared_ptr<Promise<any>> PgDatabaseAdapter::getMemoryById(std::shared_ptr<UUID> memoryId)
{
    return __super::getMemoryById(memoryId);
}

std::shared_ptr<Promise<array<any>>> PgDatabaseAdapter::searchMemories(any params)
{
    return __super::searchMemories(params);
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::updateMemory(any memory)
{
    return __super::updateMemory(memory);
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::deleteMemory(std::shared_ptr<UUID> memoryId)
{
    return __super::deleteMemory(memoryId);
}

std::shared_ptr<Promise<boolean>> PgDatabaseAdapter::createComponent(std::shared_ptr<Component> component)
{
    return __super::createComponent(component);
}

std::shared_ptr<Promise<any>> PgDatabaseAdapter::getComponent(std::shared_ptr<UUID> entityId, string type, std::shared_ptr<UUID> worldId, std::shared_ptr<UUID> sourceEntityId)
{
    return __super::getComponent(entityId, type, worldId, sourceEntityId);
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::updateComponent(std::shared_ptr<Component> component)
{
    return __super::updateComponent(component);
}

std::shared_ptr<Promise<void>> PgDatabaseAdapter::deleteComponent(std::shared_ptr<UUID> componentId)
{
    return __super::deleteComponent(componentId);
}

PgDatabaseAdapter::PgDatabaseAdapter(std::shared_ptr<UUID> agentId) : BaseDrizzleAdapter(agentId) {
}

