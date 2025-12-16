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
#include ".v2.hpp"
#include "messages.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AgentRuntime {
public:
    AgentRuntime(std::optional<{
    conversationLength: number; // number of messages to hold in the recent message cache
    agentId: UUID; // ID of the agent
    character: Character; // The character to use for the agent
    token: string; // JWT token> opts, auto can be a JWT token if outside worker, std::optional<std::variant<string; // The URL of the worker
    actions: Action[]; // Optional custom actions
    evaluators: Evaluator[]; // Optional custom evaluators
    plugins: Plugin[];
    providers: Provider[];
    modelProvider: ModelProviderName;

    services: Service[]; // Map of service name to service instance
    managers: IMemoryManager[]; // Map of table name to memory manager
    databaseAdapter: IDatabaseAdapter; // The database adapter used for interacting with the database
    fetch: typeof fetch, unknown;
    speechModelPath: string;
    cacheManager: ICacheManager;
    logging: boolean;
    // verifiableInferenceAdapter: IVerifiableInferenceAdapter;
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
    void registerMemoryManager(IMemoryManager _manager);
    std::any getMemoryManager(const std::string& _tableName);
    std::future<void> registerService(Service service);
    void initializeDatabase();
    void initialize();
    void stop();
    void getSetting(const std::string& key);
    void getConversationLength();
    void registerAction(Action action);
    void registerEvaluator(Evaluator evaluator);
    void registerContextProvider(Provider provider);
    void registerAdapter(Adapter _adapter);
    std::future<void> processActions(Memory message, const std::vector<Memory>& responses, std::optional<State> state, std::optional<HandlerCallback> callback);
    void evaluate(Memory message, State state, std::optional<bool> didRespond, std::optional<HandlerCallback> callback);
    void ensureParticipantExists(UUID _userId, UUID _roomId);
    void ensureUserExists(UUID _userId, const std::optional<std::string>& _userName, const std::optional<std::string>& _name, std::optional<std::optional<std::string>> _email, std::optional<std::optional<std::string>> _source);
    void ensureParticipantInRoom(UUID userId, UUID roomId);
    void ensureConnection(UUID userId, UUID roomId, std::optional<std::string> userName, std::optional<std::string> _userScreenName, std::optional<std::string> source);
    void ensureRoomExists(UUID roomId);
    void composeState(Memory message, const std::any& _additionalKeys);
    std::future<State> updateRecentMessageState(State state);


} // namespace elizaos
