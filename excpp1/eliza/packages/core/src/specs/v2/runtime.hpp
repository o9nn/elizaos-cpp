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
#include "..runtime.hpp"
#include "..types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Import types with the 'type' keyword

class Semaphore {
public:
    Semaphore(double count);
    std::future<void> acquire();
    void release();
};

/**
 * Represents the runtime environment for an agent.
 * @class
 * @implements { IAgentRuntime }
 * @property { number } #conversationLength - The maximum length of a conversation.
 * @property { UUID } agentId - The unique identifier for the agent.
 * @property { Character } character - The character associated with the agent.
 * @property { IDatabaseAdapter } adapter - The adapter for interacting with the database.
 * @property {Action[]} actions - The list of actions available to the agent.
 * @property {Evaluator[]} evaluators - The list of evaluators for decision making.
 * @property {Provider[]} providers - The list of providers for external services.
 * @property {Plugin[]} plugins - The list of plugins to extend functionality.
 */
class AgentRuntime {
public:
    std::unordered_map<ServiceTypeName, Service> services() const;
    std::function<void()> events() const;
    std::vector<Route> routes() const;
    UUID agentId() const;
    Character character() const;
    std::vector<Provider> providers() const;
    std::vector<Action> actions() const;
    std::vector<Evaluator> evaluators() const;
    std::vector<Plugin> plugins() const;
    IDatabaseAdapter adapter() const;
    std::future<void> registerPlugin(Plugin plugin);
    std::unordered_map<ServiceTypeName, Service> getAllServices();
    void stop();
    void initialize();
    std::future<std::any> getConnection();
    void setSetting(const std::string& key, const std::variant<std::string, bool, std::any>& value, auto secret = false);
    std::variant<std::string, bool, std::any> getSetting(const std::string& key);
    void getConversationLength();
    void registerDatabaseAdapter(IDatabaseAdapter adapter);
    void registerProvider(Provider provider);
    void registerAction(Action action);
    void registerEvaluator(Evaluator evaluator);
    std::future<void> processActions(Memory message, const std::vector<Memory>& responses, std::optional<State> state, std::optional<HandlerCallback> callback);
    void evaluate(Memory message, State state, std::optional<bool> didRespond, std::optional<HandlerCallback> callback, std::optional<std::vector<Memory>> responses);
    void ensureConnection(auto {
    entityId, auto roomId, auto userName, auto name, auto source, auto type, auto channelId, auto serverId, auto worldId, auto userId, std::optional<std::any> });
    void ensureParticipantInRoom(UUID entityId, UUID roomId);
    std::future<bool> removeParticipant(UUID entityId, UUID roomId);
    std::future<bool> addParticipant(UUID entityId, UUID roomId);
    std::future<bool> addParticipantsRoom(const std::vector<UUID>& entityIds, UUID roomId);
    void ensureWorldExists(auto { id, auto name, auto serverId, auto metadata, World agentId });
    void ensureRoomExists(auto { id, auto name, auto source, auto type, auto channelId, auto serverId, auto worldId, Room metadata });
    std::future<State> composeState(Memory message, const std::variant<std::vector<std::string>, null = null>& includeList, auto onlyInclude = false, auto skipCache = false);
    std::future<void> registerService(typeof Service service);
    void emitEvent(const std::variant<std::string, std::vector<std::string>>& event, const std::any& params);
    void ensureEmbeddingDimension();
    void registerTaskWorker(TaskWorker taskHandler);
    TaskWorker getTaskWorker(const std::string& name);
    std::any db() const;
    std::future<void> init();
    std::future<void> close();
    std::future<bool> createAgent(const std::optional<Agent>& agent);
    std::future<bool> updateAgent(UUID agentId, const std::optional<Agent>& agent);
    std::future<bool> deleteAgent(UUID agentId);
    std::future<Agent> ensureAgentExists(const std::optional<Agent>& agent);
    std::future<bool> createEntity(Entity entity);
    std::future<bool> createEntities(const std::vector<Entity>& entities);
    std::future<void> updateEntity(Entity entity);
    std::future<bool> createComponent(Component component);
    std::future<void> updateComponent(Component component);
    std::future<void> deleteComponent(UUID componentId);
    std::future<Memory> addEmbeddingToMemory(Memory memory);
    std::future<void> clearAllAgentMemories();
    std::future<void> log(const std::any& params);
    std::future<UUID> createMemory(Memory memory, const std::string& tableName, std::optional<bool> unique);
    std::future<bool> updateMemory(std::optional<std::optional<Memory>> memory);
    std::future<void> deleteMemory(UUID memoryId);
    std::future<void> deleteManyMemories(const std::vector<UUID>& memoryIds);
    std::future<void> deleteAllMemories(UUID roomId, const std::string& tableName);
    std::future<double> countMemories(UUID roomId, std::optional<bool> unique, std::optional<std::string> tableName);
    std::future<void> deleteLog(UUID logId);
    std::future<UUID> createWorld(World world);
    std::future<void> removeWorld(UUID worldId);
    std::future<void> updateWorld(World world);
    std::future<UUID> createRoom(auto { id, auto name, auto source, auto type, auto channelId, auto serverId, Room worldId });
    std::future<void> deleteRoom(UUID roomId);
    std::future<void> deleteRoomsByWorldId(UUID worldId);
    std::future<void> updateRoom(Room room);
    std::future<void> setParticipantUserState(UUID roomId, UUID entityId, const std::variant<'FOLLOWED', 'MUTED'>& state);
    std::future<bool> createRelationship(std::optional<std::any> params);
    std::future<void> updateRelationship(Relationship relationship);
    std::future<bool> deleteCache(const std::string& key);
    std::future<UUID> createTask(Task task);
    std::future<void> updateTask(UUID id, const std::optional<Task>& task);
    std::future<void> deleteTask(UUID id);
    void emit(const std::string& event, const std::any& data);
    std::future<void> sendControlMessage(std::optional<std::any> params);
    void registerSendHandler(const std::string& source, SendHandlerFunction handler);
    std::future<void> sendMessageToTarget(TargetInfo target, Content content);
    UUID createRunId();
    UUID startRun();
    void endRun();
    UUID getCurrentRunId();


} // namespace elizaos
