#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_DATABASE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CORE_SRC_TYPES_DATABASE_H
#include "core.hpp"
#include "./agent.h"
#include "./environment.h"
#include "./memory.h"
#include "./primitives.h"
#include "./task.h"

typedef any DbConnection;

class Log;
class IDatabaseAdapter;
class EmbeddingSearchResult;
class MemoryRetrievalOptions;
class MemorySearchOptions;
class MultiRoomMemoryOptions;
class UnifiedMemoryOptions;
class UnifiedSearchOptions;

class Log : public object, public std::enable_shared_from_this<Log> {
public:
    using std::enable_shared_from_this<Log>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<UUID> roomId;

    object body;

    string type;

    std::shared_ptr<Date> createdAt;
};

class IDatabaseAdapter : public object, public std::enable_shared_from_this<IDatabaseAdapter> {
public:
    using std::enable_shared_from_this<IDatabaseAdapter>::shared_from_this;
    any db;

    virtual std::shared_ptr<Promise<void>> initialize(any config = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> init() = 0;
    virtual std::shared_ptr<Promise<void>> runMigrations(any schema = undefined, string pluginName = undefined) = 0;
    virtual std::shared_ptr<Promise<boolean>> isReady() = 0;
    virtual std::shared_ptr<Promise<void>> close() = 0;
    virtual std::shared_ptr<Promise<any>> getConnection() = 0;
    virtual std::shared_ptr<Promise<any>> getAgent(std::shared_ptr<UUID> agentId) = 0;
    virtual std::shared_ptr<Promise<array<Partial<std::shared_ptr<Agent>>>>> getAgents() = 0;
    virtual std::shared_ptr<Promise<boolean>> createAgent(Partial<std::shared_ptr<Agent>> agent) = 0;
    virtual std::shared_ptr<Promise<boolean>> updateAgent(std::shared_ptr<UUID> agentId, Partial<std::shared_ptr<Agent>> agent) = 0;
    virtual std::shared_ptr<Promise<boolean>> deleteAgent(std::shared_ptr<UUID> agentId) = 0;
    virtual std::shared_ptr<Promise<void>> ensureEmbeddingDimension(double dimension) = 0;
    virtual std::shared_ptr<Promise<any>> getEntitiesByIds(array<std::shared_ptr<UUID>> entityIds) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Entity>>>> getEntitiesForRoom(std::shared_ptr<UUID> roomId, boolean includeComponents = undefined) = 0;
    virtual std::shared_ptr<Promise<boolean>> createEntities(array<std::shared_ptr<Entity>> entities) = 0;
    virtual std::shared_ptr<Promise<void>> updateEntity(std::shared_ptr<Entity> entity) = 0;
    virtual std::shared_ptr<Promise<any>> getComponent(std::shared_ptr<UUID> entityId, string type, std::shared_ptr<UUID> worldId = undefined, std::shared_ptr<UUID> sourceEntityId = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Component>>>> getComponents(std::shared_ptr<UUID> entityId, std::shared_ptr<UUID> worldId = undefined, std::shared_ptr<UUID> sourceEntityId = undefined) = 0;
    virtual std::shared_ptr<Promise<boolean>> createComponent(std::shared_ptr<Component> component) = 0;
    virtual std::shared_ptr<Promise<void>> updateComponent(std::shared_ptr<Component> component) = 0;
    virtual std::shared_ptr<Promise<void>> deleteComponent(std::shared_ptr<UUID> componentId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemories(object params) = 0;
    virtual std::shared_ptr<Promise<any>> getMemoryById(std::shared_ptr<UUID> id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByIds(array<std::shared_ptr<UUID>> ids, string tableName = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByRoomIds(object params) = 0;
    virtual std::shared_ptr<Promise<array<object>>> getCachedEmbeddings(object params) = 0;
    virtual std::shared_ptr<Promise<void>> log(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Log>>>> getLogs(object params) = 0;
    virtual std::shared_ptr<Promise<void>> deleteLog(std::shared_ptr<UUID> logId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> searchMemories(object params) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createMemory(std::shared_ptr<Memory> memory, string tableName, boolean unique = undefined) = 0;
    virtual std::shared_ptr<Promise<boolean>> updateMemory(any memory) = 0;
    virtual std::shared_ptr<Promise<void>> deleteMemory(std::shared_ptr<UUID> memoryId) = 0;
    virtual std::shared_ptr<Promise<void>> deleteManyMemories(array<std::shared_ptr<UUID>> memoryIds) = 0;
    virtual std::shared_ptr<Promise<void>> deleteAllMemories(std::shared_ptr<UUID> roomId, string tableName) = 0;
    virtual std::shared_ptr<Promise<double>> countMemories(std::shared_ptr<UUID> roomId, boolean unique = undefined, string tableName = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createWorld(World world) = 0;
    virtual std::shared_ptr<Promise<any>> getWorld(std::shared_ptr<UUID> id) = 0;
    virtual std::shared_ptr<Promise<void>> removeWorld(std::shared_ptr<UUID> id) = 0;
    virtual std::shared_ptr<Promise<array<World>>> getAllWorlds() = 0;
    virtual std::shared_ptr<Promise<void>> updateWorld(World world) = 0;
    virtual std::shared_ptr<Promise<any>> getRoomsByIds(array<std::shared_ptr<UUID>> roomIds) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<UUID>>>> createRooms(array<Room> rooms) = 0;
    virtual std::shared_ptr<Promise<void>> deleteRoom(std::shared_ptr<UUID> roomId) = 0;
    virtual std::shared_ptr<Promise<void>> deleteRoomsByWorldId(std::shared_ptr<UUID> worldId) = 0;
    virtual std::shared_ptr<Promise<void>> updateRoom(Room room) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<UUID>>>> getRoomsForParticipant(std::shared_ptr<UUID> entityId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<UUID>>>> getRoomsForParticipants(array<std::shared_ptr<UUID>> userIds) = 0;
    virtual std::shared_ptr<Promise<array<Room>>> getRoomsByWorld(std::shared_ptr<UUID> worldId) = 0;
    virtual std::shared_ptr<Promise<boolean>> removeParticipant(std::shared_ptr<UUID> entityId, std::shared_ptr<UUID> roomId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Participant>>>> getParticipantsForEntity(std::shared_ptr<UUID> entityId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<UUID>>>> getParticipantsForRoom(std::shared_ptr<UUID> roomId) = 0;
    virtual std::shared_ptr<Promise<boolean>> addParticipantsRoom(array<std::shared_ptr<UUID>> entityIds, std::shared_ptr<UUID> roomId) = 0;
    template <typename RET>
    virtual RET getParticipantUserState(std::shared_ptr<UUID> roomId, std::shared_ptr<UUID> entityId) = 0;
    virtual std::shared_ptr<Promise<void>> setParticipantUserState(std::shared_ptr<UUID> roomId, std::shared_ptr<UUID> entityId, P2 state) = 0;
    virtual std::shared_ptr<Promise<boolean>> createRelationship(object params) = 0;
    virtual std::shared_ptr<Promise<void>> updateRelationship(std::shared_ptr<Relationship> relationship) = 0;
    virtual std::shared_ptr<Promise<any>> getRelationship(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Relationship>>>> getRelationships(object params) = 0;
    template <typename T>
    std::shared_ptr<Promise<any>> getCache(string key) = 0;
    template <typename T>
    std::shared_ptr<Promise<boolean>> setCache(string key, T value) = 0;
    virtual std::shared_ptr<Promise<boolean>> deleteCache(string key) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UUID>>> createTask(std::shared_ptr<Task> task) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> getTasks(object params) = 0;
    virtual std::shared_ptr<Promise<any>> getTask(std::shared_ptr<UUID> id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Task>>>> getTasksByName(string name) = 0;
    virtual std::shared_ptr<Promise<void>> updateTask(std::shared_ptr<UUID> id, Partial<std::shared_ptr<Task>> task) = 0;
    virtual std::shared_ptr<Promise<void>> deleteTask(std::shared_ptr<UUID> id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByWorldId(object params) = 0;
};

class EmbeddingSearchResult : public object, public std::enable_shared_from_this<EmbeddingSearchResult> {
public:
    using std::enable_shared_from_this<EmbeddingSearchResult>::shared_from_this;
    array<double> embedding;

    double levenshtein_score;
};

class MemoryRetrievalOptions : public object, public std::enable_shared_from_this<MemoryRetrievalOptions> {
public:
    using std::enable_shared_from_this<MemoryRetrievalOptions>::shared_from_this;
    std::shared_ptr<UUID> roomId;

    double count;

    boolean unique;

    double start;

    double end;

    std::shared_ptr<UUID> agentId;
};

class MemorySearchOptions : public object, public std::enable_shared_from_this<MemorySearchOptions> {
public:
    using std::enable_shared_from_this<MemorySearchOptions>::shared_from_this;
    array<double> embedding;

    double match_threshold;

    double count;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> agentId;

    boolean unique;

    Partial<MemoryMetadata> metadata;
};

class MultiRoomMemoryOptions : public object, public std::enable_shared_from_this<MultiRoomMemoryOptions> {
public:
    using std::enable_shared_from_this<MultiRoomMemoryOptions>::shared_from_this;
    array<std::shared_ptr<UUID>> roomIds;

    double limit;

    std::shared_ptr<UUID> agentId;
};

class UnifiedMemoryOptions : public object, public std::enable_shared_from_this<UnifiedMemoryOptions> {
public:
    using std::enable_shared_from_this<UnifiedMemoryOptions>::shared_from_this;
    std::shared_ptr<UUID> roomId;

    double limit;

    std::shared_ptr<UUID> agentId;

    boolean unique;

    double start;

    double end;
};

class UnifiedSearchOptions : public UnifiedMemoryOptions, public std::enable_shared_from_this<UnifiedSearchOptions> {
public:
    using std::enable_shared_from_this<UnifiedSearchOptions>::shared_from_this;
    array<double> embedding;

    double similarity;
};

extern object VECTOR_DIMS;
#endif
