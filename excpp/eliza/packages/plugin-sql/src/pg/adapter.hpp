#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".base.hpp"
#include ".schema/embedding.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Adapter class for interacting with a PostgreSQL database.
 * Extends BaseDrizzleAdapter.
 */
class PgDatabaseAdapter {
public:
    PgDatabaseAdapter(UUID agentId, PostgresConnectionManager manager, std::optional<std::any> _schema);
    std::future<void> runMigrations();
    std::future<void> init();
    std::future<bool> isReady();
    std::future<void> close();
    void getConnection();
    std::future<bool> createAgent(Agent agent);
    std::variant<Promise<Agent, null>> getAgent(UUID agentId);
    std::future<bool> updateAgent(UUID agentId, const std::optional<Agent>& agent);
    std::future<bool> deleteAgent(UUID agentId);
    std::future<bool> createEntities(const std::vector<Entity>& entities);
    std::future<std::vector<Entity>> getEntityByIds(const std::vector<UUID>& entityIds);
    std::future<void> updateEntity(Entity entity);
    std::future<UUID> createMemory(Memory memory, const std::string& tableName);
    std::variant<Promise<Memory, null>> getMemoryById(UUID memoryId);
    std::future<std::vector<std::any>> searchMemories(const std::any& params);
    std::future<bool> updateMemory(const std::optional<Memory>& memory);
    std::future<void> deleteMemory(UUID memoryId);
    std::future<bool> createComponent(Component component);
    std::variant<Promise<Component, null>> getComponent(UUID entityId, const std::string& type, std::optional<UUID> worldId, std::optional<UUID> sourceEntityId);
    std::future<void> updateComponent(Component component);
    std::future<void> deleteComponent(UUID componentId);

private:
    PostgresConnectionManager manager_;
};


} // namespace elizaos
