#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "messages.ts.hpp"
#include "types.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AgentRuntime {
public:
    AgentRuntime(std::optional<{
    token: string; // JWT token> opts);
    UUID agentId() const;
    std::string serverUrl() const;
    IDatabaseAdapter databaseAdapter() const;
    std::string token() const;
    Character character() const;
    std::vector<Action> actions() const;
    std::vector<Evaluator> evaluators() const;
    std::vector<Provider> providers() const;
    std::vector<Plugin> plugins() const;
    void modelProvider() const;
    void imageModelProvider() const;
    void imageVisionModelProvider() const;
    void messageManager() const;
    void routes() const;
    void services() const;
    void events() const;
    void descriptionManager() const;
    void documentsManager() const;
    void knowledgeManager() const;
    void ragKnowledgeManager() const;
    void loreManager() const;
    void cacheManager() const;
    void clients() const;
    void registerMemoryManager(IMemoryManager manager);
    std::any getMemoryManager(const std::string& tableName);
    std::future<void> registerService(Service service);
    void initializeDatabase();
    void initialize();
    void stop();
    void getSetting(const std::string& key);
    void getConversationLength();
    void registerAction(Action action);
    void registerEvaluator(Evaluator evaluator);
    void registerContextProvider(Provider provider);
    void registerAdapter(Adapter adapter);
    std::future<void> processActions(Memory message, const std::vector<Memory>& responses, std::optional<State> state, std::optional<HandlerCallback> callback);
    void evaluate(Memory message, State state, std::optional<bool> didRespond, std::optional<HandlerCallback> callback);
    void ensureParticipantExists(UUID userId, UUID roomId);
    void ensureUserExists(UUID userId, const std::string& userName, const std::string& name, std::optional<std::string> email, std::optional<std::string> source);
    void ensureParticipantInRoom(UUID userId, UUID roomId);
    void ensureConnection(UUID userId, UUID roomId, std::optional<std::string> userName, std::optional<std::string> userScreenName, std::optional<std::string> source);
    void ensureRoomExists(UUID roomId);
    void composeState(Memory message, std::any additionalKeys = {});
    std::future<State> updateRecentMessageState(State state);
};


} // namespace elizaos
