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
#include "entities.hpp"
#include "index.hpp"
#include "logger.hpp"
#include "search.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Interface for working memory entries
struct WorkingMemoryEntry {
    std::string actionName;
    ActionResult result;
    double timestamp;
};

class Semaphore {
public:
    Semaphore(double count);
    std::future<void> acquire();
    void release();

private:
    double permits_;
};

class AgentRuntime {
public:
    UUID createRunId();
    UUID startRun();
    void endRun();
    UUID getCurrentRunId();
    std::future<void> registerPlugin(Plugin plugin);
    std::unordered_map<ServiceTypeName, std::vector<Service>> getAllServices();
    void stop();
    std::future<void> initialize();
    std::future<unknown> getConnection();
    void setSetting(const std::string& key, const std::variant<std::string, bool, std::any>& value, auto secret);
    std::variant<std::string, bool, std::any> getSetting(const std::string& key);
    void getConversationLength();
    void registerDatabaseAdapter(IDatabaseAdapter adapter);
    void registerProvider(Provider provider);
    void registerAction(Action action);
    void registerEvaluator(Evaluator evaluator);
    std::future<void> processActions(Memory message, const std::vector<Memory>& responses, std::optional<State> state, std::optional<HandlerCallback> callback);
    void evaluate(Memory message, State state, std::optional<bool> didRespond, std::optional<HandlerCallback> callback, std::optional<std::vector<Memory>> responses);
    std::future<void> ensureConnections(auto entities, auto rooms, auto source, auto world);
    void ensureConnection(auto roomId, auto worldId, auto worldName, auto userName, auto name, auto source, auto type, auto channelId, auto serverId, auto userId, auto metadata);
    void ensureParticipantInRoom(UUID entityId, UUID roomId);
    std::future<bool> removeParticipant(UUID entityId, UUID roomId);
    std::future<std::vector<Participant>> getParticipantsForEntity(UUID entityId);
    std::future<std::vector<UUID>> getParticipantsForRoom(UUID roomId);
    std::future<bool> addParticipant(UUID entityId, UUID roomId);
    std::future<bool> addParticipantsRoom(const std::vector<UUID>& entityIds, UUID roomId);
    void ensureWorldExists(auto name, auto serverId);
    void ensureRoomExists(auto name, auto source, auto type, auto channelId, auto serverId, auto worldId);
    std::future<State> composeState(Memory message, std::optional<std::vector<std::string>> includeList = std::nullopt, auto onlyInclude, auto skipCache);
    std::vector<ServiceTypeName> getRegisteredServiceTypes();
    bool hasService(const std::variant<ServiceTypeName, std::string>& serviceType);
    std::future<void> registerService(typeof Service serviceDef);
    std::function<std::future<std::any>(IAgentRuntime, std::any)> getModel(ModelTypeName modelType, std::optional<std::string> provider);
    null);
    const model = this::getModel(modelKey, provider);
    if (!model) stringify();
    std::function<std::vector<std::future<void>>(std::any)> getEvent(const std::string& event);
    void emitEvent(const std::variant<std::string, std::vector<std::string>>& event, const std::any& params);
    void ensureEmbeddingDimension();
    void registerTaskWorker(TaskWorker taskHandler);
    TaskWorker getTaskWorker(const std::string& name);
    std::any db() const;
    std::future<void> init();
    std::future<void> close();
    std::variant<Promise<Agent, null>> getAgent(UUID agentId);
    std::future<std::vector<std::optional<Agent>>> getAgents();
    std::future<bool> createAgent(const std::optional<Agent>& agent);
    std::future<bool> updateAgent(UUID agentId, const std::optional<Agent>& agent);
    std::future<bool> deleteAgent(UUID agentId);
    std::future<Agent> ensureAgentExists(const std::optional<Agent>& agent);
    std::variant<Promise<Entity, null>> getEntityById(UUID entityId);
    std::variant<std::vector<Promise<Entity>, null>> getEntitiesByIds(const std::vector<UUID>& entityIds);
    std::future<std::vector<Entity>> getEntitiesForRoom(UUID roomId, std::optional<bool> includeComponents);
    std::future<bool> createEntity(Entity entity);
    std::future<bool> createEntities(const std::vector<Entity>& entities);
    std::future<void> updateEntity(Entity entity);
    std::variant<Promise<Component, null>> getComponent(UUID entityId, const std::string& type, std::optional<UUID> worldId, std::optional<UUID> sourceEntityId);
    std::future<std::vector<Component>> getComponents(UUID entityId, std::optional<UUID> worldId, std::optional<UUID> sourceEntityId);
    std::future<bool> createComponent(Component component);
    std::future<void> updateComponent(Component component);
    std::future<void> deleteComponent(UUID componentId);
    std::future<Memory> addEmbeddingToMemory(Memory memory);
    std::future<std::vector<Memory>> getMemories(std::optional<std::any> params);
    std::future<std::vector<Memory>> getAllMemories();
    std::variant<Promise<Memory, null>> getMemoryById(UUID id);
    std::future<std::vector<Memory>> getMemoriesByIds(const std::vector<UUID>& ids, std::optional<std::string> tableName);
    std::future<std::vector<Memory>> getMemoriesByRoomIds(std::optional<std::any> params);
    Promise< getCachedEmbeddings(const std::any& params);
    std::future<void> log(const std::any& params);
    std::future<std::vector<Memory>> searchMemories(std::optional<std::any> params);
    std::future<std::vector<Memory>> rerankMemories(const std::string& query, const std::vector<Memory>& memories);
    std::future<UUID> createMemory(Memory memory, const std::string& tableName, std::optional<bool> unique);
    std::future<bool> updateMemory(std::optional<std::optional<Memory>> memory);
    std::future<void> deleteMemory(UUID memoryId);
    std::future<void> deleteManyMemories(const std::vector<UUID>& memoryIds);
    std::future<void> clearAllAgentMemories();
    std::future<void> deleteAllMemories(UUID roomId, const std::string& tableName);
    std::future<double> countMemories(UUID roomId, std::optional<bool> unique, std::optional<std::string> tableName);
    std::future<std::vector<Log>> getLogs(std::optional<std::any> params);
    std::future<void> deleteLog(UUID logId);
    std::future<UUID> createWorld(World world);
    std::variant<Promise<World, null>> getWorld(UUID id);
    std::future<void> removeWorld(UUID worldId);
    std::future<std::vector<World>> getAllWorlds();
    std::future<void> updateWorld(World world);
    std::variant<Promise<Room, null>> getRoom(UUID roomId);
    std::variant<std::vector<Promise<Room>, null>> getRoomsByIds(const std::vector<UUID>& roomIds);
    std::future<UUID> createRoom(auto name, auto source, auto type, auto channelId, auto serverId);
    std::future<std::vector<UUID>> createRooms(const std::vector<Room>& rooms);
    std::future<void> deleteRoom(UUID roomId);
    std::future<void> deleteRoomsByWorldId(UUID worldId);
    std::future<void> updateRoom(Room room);
    std::future<std::vector<UUID>> getRoomsForParticipant(UUID entityId);
    std::future<std::vector<UUID>> getRoomsForParticipants(const std::vector<UUID>& userIds);
    std::future<std::vector<Room>> getRooms(UUID worldId);
    std::future<std::vector<Room>> getRoomsByWorld(UUID worldId);
    std::future<void> setParticipantUserState(UUID roomId, UUID entityId, const std::string& state);
    std::future<bool> createRelationship(std::optional<std::any> params);
    std::future<void> updateRelationship(Relationship relationship);
    std::variant<Promise<Relationship, null>> getRelationship(const std::any& params);
    std::future<std::vector<Relationship>> getRelationships(std::optional<std::any> params);
    std::future<bool> deleteCache(const std::string& key);
    std::future<UUID> createTask(Task task);
    std::future<std::vector<Task>> getTasks(std::optional<std::any> params);
    std::variant<Promise<Task, null>> getTask(UUID id);
    std::future<std::vector<Task>> getTasksByName(const std::string& name);
    std::future<void> updateTask(UUID id, const std::optional<Task>& task);
    std::future<void> deleteTask(UUID id);
    void emit(const std::string& event, const std::any& data);
    std::future<void> sendControlMessage(std::optional<std::any> params);
    void registerSendHandler(const std::string& source, SendHandlerFunction handler);
    std::future<void> sendMessageToTarget(TargetInfo target, Content content);
    std::future<std::vector<Memory>> getMemoriesByWorldId(std::optional<std::any> params);
    std::future<void> runMigrations(std::optional<std::vector<std::string>> migrationsPaths);
    std::future<bool> isReady();
    std::future<UUID> getAgentEntityId();

private:
    std::string text_;
    RuntimeSettings settings_;
    std::string actionName_;
    UUID actionId_;
    std::string modelType_;
    std::string prompt_;
    double timestamp_;
};


} // namespace elizaos
