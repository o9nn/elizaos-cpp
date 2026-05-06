#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V1_TYPES_H
#include "core.h"
#include "stream.h"

typedef  UUID;

typedef object ModelSettings;

typedef object ImageModelSettings;

typedef object EmbeddingModelSettings;

typedef object Model;

typedef object Models;

typedef std::function<std::shared_ptr<Promise<any>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>, object, HandlerCallback)> Handler;

typedef std::function<std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>>(std::shared_ptr<Content>, any)> HandlerCallback;

typedef std::function<std::shared_ptr<Promise<boolean>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>)> Validator;

typedef object Media;

typedef object ClientInstance;

typedef object Client;

typedef object Adapter;

typedef object Plugin;

typedef object TelemetrySettings;

typedef any TemplateType;

typedef object Character;

typedef object CacheOptions;

typedef object KnowledgeItem;

class Content;
class ActionExample;
class ConversationExample;
class Actor;
class Objective;
enum struct GoalStatus;
class Goal;
enum struct ModelClass;
enum struct ModelProviderName;
class State;
class Memory;
class MessageExample;
class Action;
class EvaluationExample;
class Evaluator;
class Provider;
class Relationship;
class Account;
class Participant;
class Room;
class IAgentConfig;
class ModelConfiguration;
class TwitterSpaceDecisionOptions;
class IDatabaseAdapter;
class IDatabaseCacheAdapter;
class IMemoryManager;
class IRAGKnowledgeManager;
enum struct CacheStore;
class ICacheManager;
class Service;
class IAgentRuntime;
class IImageDescriptionService;
class ITranscriptionService;
class IVideoService;
class ITextGenerationService;
class IBrowserService;
class ISpeechService;
class IPdfService;
class IAwsS3Service;
class UploadIrysResult;
class DataIrysFetchedFromGQL;
class GraphQLTag;
enum struct IrysMessageType;
enum struct IrysDataType;
class IrysTimestamp;
class IIrysService;
class ITeeLogService;
enum struct ServiceType;
enum struct LoggingLevel;
class RAGKnowledgeItem;
class ActionResponse;
class ISlackService;
enum struct TokenizerType;
enum struct TranscriptionProvider;
enum struct ActionTimelineType;
enum struct KnowledgeScope;
enum struct CacheKeyPrefix;
class DirectoryItem;
class ChunkRow;

class Content : public object, public std::enable_shared_from_this<Content> {
public:
    using std::enable_shared_from_this<Content>::shared_from_this;
    string text;

    string action;

    string source;

    string url;

    UUID inReplyTo;

    array<Media> attachments;
};

class ActionExample : public object, public std::enable_shared_from_this<ActionExample> {
public:
    using std::enable_shared_from_this<ActionExample>::shared_from_this;
    string user;

    std::shared_ptr<Content> content;
};

class ConversationExample : public object, public std::enable_shared_from_this<ConversationExample> {
public:
    using std::enable_shared_from_this<ConversationExample>::shared_from_this;
    UUID userId;

    std::shared_ptr<Content> content;
};

class Actor : public object, public std::enable_shared_from_this<Actor> {
public:
    using std::enable_shared_from_this<Actor>::shared_from_this;
    string name;

    string username;

    object details;

    UUID id;
};

class Objective : public object, public std::enable_shared_from_this<Objective> {
public:
    using std::enable_shared_from_this<Objective>::shared_from_this;
    string id;

    string description;

    boolean completed;
};

enum struct GoalStatus {
    DONE = std::string("DONE"), FAILED = std::string("FAILED"), IN_PROGRESS = std::string("IN_PROGRESS")
};
class Goal : public object, public std::enable_shared_from_this<Goal> {
public:
    using std::enable_shared_from_this<Goal>::shared_from_this;
    UUID id;

    UUID roomId;

    UUID userId;

    string name;

    GoalStatus status;

    array<std::shared_ptr<Objective>> objectives;
};

enum struct ModelClass {
    SMALL = std::string("small"), MEDIUM = std::string("medium"), LARGE = std::string("large"), EMBEDDING = std::string("embedding"), IMAGE = std::string("image")
};
enum struct ModelProviderName {
    OPENAI = std::string("openai"), ETERNALAI = std::string("eternalai"), ANTHROPIC = std::string("anthropic"), GROK = std::string("grok"), GROQ = std::string("groq"), LLAMACLOUD = std::string("llama_cloud"), TOGETHER = std::string("together"), LLAMALOCAL = std::string("llama_local"), LMSTUDIO = std::string("lmstudio"), GOOGLE = std::string("google"), MISTRAL = std::string("mistral"), CLAUDE_VERTEX = std::string("claude_vertex"), REDPILL = std::string("redpill"), OPENROUTER = std::string("openrouter"), OLLAMA = std::string("ollama"), HEURIST = std::string("heurist"), GALADRIEL = std::string("galadriel"), FAL = std::string("falai"), GAIANET = std::string("gaianet"), ALI_BAILIAN = std::string("ali_bailian"), VOLENGINE = std::string("volengine"), NANOGPT = std::string("nanogpt"), HYPERBOLIC = std::string("hyperbolic"), VENICE = std::string("venice"), NVIDIA = std::string("nvidia"), NINETEEN_AI = std::string("nineteen_ai"), AKASH_CHAT_API = std::string("akash_chat_api"), LIVEPEER = std::string("livepeer"), LETZAI = std::string("letzai"), DEEPSEEK = std::string("deepseek"), INFERA = std::string("infera"), BEDROCK = std::string("bedrock"), ATOMA = std::string("atoma"), SECRETAI = std::string("secret_ai"), NEARAI = std::string("nearai")
};
class State : public object, public std::enable_shared_from_this<State> {
public:
    using std::enable_shared_from_this<State>::shared_from_this;
    UUID userId;

    UUID agentId;

    string bio;

    string lore;

    string messageDirections;

    string postDirections;

    UUID roomId;

    string agentName;

    string senderName;

    string actors;

    array<std::shared_ptr<Actor>> actorsData;

    string goals;

    array<std::shared_ptr<Goal>> goalsData;

    string recentMessages;

    array<std::shared_ptr<Memory>> recentMessagesData;

    string actionNames;

    string actions;

    array<std::shared_ptr<Action>> actionsData;

    string actionExamples;

    string providers;

    std::shared_ptr<Content> responseData;

    array<std::shared_ptr<Memory>> recentInteractionsData;

    string recentInteractions;

    string formattedConversation;

    string knowledge;

    array<KnowledgeItem> knowledgeData;

    array<std::shared_ptr<RAGKnowledgeItem>> ragKnowledgeData;

    string text;
};

class Memory : public object, public std::enable_shared_from_this<Memory> {
public:
    using std::enable_shared_from_this<Memory>::shared_from_this;
    UUID id;

    UUID userId;

    UUID agentId;

    double createdAt;

    std::shared_ptr<Content> content;

    array<double> embedding;

    UUID roomId;

    boolean unique;

    double similarity;
};

class MessageExample : public object, public std::enable_shared_from_this<MessageExample> {
public:
    using std::enable_shared_from_this<MessageExample>::shared_from_this;
    string user;

    std::shared_ptr<Content> content;
};

class Action : public object, public std::enable_shared_from_this<Action> {
public:
    using std::enable_shared_from_this<Action>::shared_from_this;
    array<string> similes;

    string description;

    array<array<std::shared_ptr<ActionExample>>> examples;

    Handler handler;

    string name;

    Validator validate;

    boolean suppressInitialMessage;
};

class EvaluationExample : public object, public std::enable_shared_from_this<EvaluationExample> {
public:
    using std::enable_shared_from_this<EvaluationExample>::shared_from_this;
    string context;

    array<std::shared_ptr<ActionExample>> messages;

    string outcome;
};

class Evaluator : public object, public std::enable_shared_from_this<Evaluator> {
public:
    using std::enable_shared_from_this<Evaluator>::shared_from_this;
    boolean alwaysRun;

    string description;

    array<string> similes;

    array<std::shared_ptr<EvaluationExample>> examples;

    Handler handler;

    string name;

    Validator validate;
};

class Provider : public object, public std::enable_shared_from_this<Provider> {
public:
    using std::enable_shared_from_this<Provider>::shared_from_this;
    string name;

    string description;

    boolean dynamic;

    double position;

    boolean private;

    std::function<std::shared_ptr<Promise<any>>(std::shared_ptr<IAgentRuntime>, std::shared_ptr<Memory>, std::shared_ptr<State>)> get;
};

class Relationship : public object, public std::enable_shared_from_this<Relationship> {
public:
    using std::enable_shared_from_this<Relationship>::shared_from_this;
    UUID id;

    UUID userA;

    UUID userB;

    UUID userId;

    UUID roomId;

    string status;

    string createdAt;
};

class Account : public object, public std::enable_shared_from_this<Account> {
public:
    using std::enable_shared_from_this<Account>::shared_from_this;
    UUID id;

    string name;

    string username;

    object details;

    string email;

    string avatarUrl;
};

class Participant : public object, public std::enable_shared_from_this<Participant> {
public:
    using std::enable_shared_from_this<Participant>::shared_from_this;
    UUID id;

    std::shared_ptr<Account> account;
};

class Room : public object, public std::enable_shared_from_this<Room> {
public:
    using std::enable_shared_from_this<Room>::shared_from_this;
    UUID id;

    array<std::shared_ptr<Participant>> participants;
};

class IAgentConfig : public object, public std::enable_shared_from_this<IAgentConfig> {
public:
    using std::enable_shared_from_this<IAgentConfig>::shared_from_this;
};

class ModelConfiguration : public object, public std::enable_shared_from_this<ModelConfiguration> {
public:
    using std::enable_shared_from_this<ModelConfiguration>::shared_from_this;
    double temperature;

    double maxOutputTokens;

    double frequency_penalty;

    double presence_penalty;

    double maxInputTokens;

    TelemetrySettings experimental_telemetry;
};

class TwitterSpaceDecisionOptions : public object, public std::enable_shared_from_this<TwitterSpaceDecisionOptions> {
public:
    using std::enable_shared_from_this<TwitterSpaceDecisionOptions>::shared_from_this;
    double maxSpeakers;

    array<string> topics;

    double typicalDurationMinutes;

    double idleKickTimeoutMs;

    double minIntervalBetweenSpacesMinutes;

    boolean businessHoursOnly;

    double randomChance;

    boolean enableIdleMonitor;

    boolean enableSttTts;

    boolean enableRecording;

    string voiceId;

    string sttLanguage;

    double speakerMaxDurationMs;
};

class IDatabaseAdapter : public object, public std::enable_shared_from_this<IDatabaseAdapter> {
public:
    using std::enable_shared_from_this<IDatabaseAdapter>::shared_from_this;
    any db;

    virtual std::shared_ptr<Promise<void>> init() = 0;
    virtual std::shared_ptr<Promise<void>> close() = 0;
    virtual std::shared_ptr<Promise<any>> getAccountById(UUID userId) = 0;
    virtual std::shared_ptr<Promise<boolean>> createAccount(std::shared_ptr<Account> account) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemories(object params) = 0;
    virtual std::shared_ptr<Promise<any>> getMemoryById(UUID id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByIds(array<UUID> ids, string tableName = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByRoomIds(object params) = 0;
    virtual std::shared_ptr<Promise<array<object>>> getCachedEmbeddings(object params) = 0;
    virtual std::shared_ptr<Promise<void>> log(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Actor>>>> getActorDetails(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> searchMemories(object params) = 0;
    virtual std::shared_ptr<Promise<void>> updateGoalStatus(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> searchMemoriesByEmbedding(array<double> embedding, object params) = 0;
    virtual std::shared_ptr<Promise<void>> createMemory(std::shared_ptr<Memory> memory, string tableName, boolean unique = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> removeMemory(UUID memoryId, string tableName) = 0;
    virtual std::shared_ptr<Promise<void>> removeAllMemories(UUID roomId, string tableName) = 0;
    virtual std::shared_ptr<Promise<double>> countMemories(UUID roomId, boolean unique = undefined, string tableName = undefined) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Goal>>>> getGoals(object params) = 0;
    virtual std::shared_ptr<Promise<void>> updateGoal(std::shared_ptr<Goal> goal) = 0;
    virtual std::shared_ptr<Promise<void>> createGoal(std::shared_ptr<Goal> goal) = 0;
    virtual std::shared_ptr<Promise<void>> removeGoal(UUID goalId) = 0;
    virtual std::shared_ptr<Promise<void>> removeAllGoals(UUID roomId) = 0;
    virtual std::shared_ptr<Promise<any>> getRoom(UUID roomId) = 0;
    virtual std::shared_ptr<Promise<UUID>> createRoom(UUID roomId = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> removeRoom(UUID roomId) = 0;
    virtual std::shared_ptr<Promise<array<UUID>>> getRoomsForParticipant(UUID userId) = 0;
    virtual std::shared_ptr<Promise<array<UUID>>> getRoomsForParticipants(array<UUID> userIds) = 0;
    virtual std::shared_ptr<Promise<boolean>> addParticipant(UUID userId, UUID roomId) = 0;
    virtual std::shared_ptr<Promise<boolean>> removeParticipant(UUID userId, UUID roomId) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Participant>>>> getParticipantsForAccount(UUID userId) = 0;
    virtual std::shared_ptr<Promise<array<UUID>>> getParticipantsForRoom(UUID roomId) = 0;
    template <typename RET>
    virtual RET getParticipantUserState(UUID roomId, UUID userId) = 0;
    virtual std::shared_ptr<Promise<void>> setParticipantUserState(UUID roomId, UUID userId, P2 state) = 0;
    virtual std::shared_ptr<Promise<boolean>> createRelationship(object params) = 0;
    virtual std::shared_ptr<Promise<any>> getRelationship(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Relationship>>>> getRelationships(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<RAGKnowledgeItem>>>> getKnowledge(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<RAGKnowledgeItem>>>> searchKnowledge(object params) = 0;
    virtual std::shared_ptr<Promise<void>> createKnowledge(std::shared_ptr<RAGKnowledgeItem> knowledge) = 0;
    virtual std::shared_ptr<Promise<void>> removeKnowledge(UUID id) = 0;
    virtual std::shared_ptr<Promise<void>> clearKnowledge(UUID agentId, boolean shared = undefined) = 0;
};

class IDatabaseCacheAdapter : public object, public std::enable_shared_from_this<IDatabaseCacheAdapter> {
public:
    using std::enable_shared_from_this<IDatabaseCacheAdapter>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> getCache(object params) = 0;
    virtual std::shared_ptr<Promise<boolean>> setCache(object params) = 0;
    virtual std::shared_ptr<Promise<boolean>> deleteCache(object params) = 0;
};

class IMemoryManager : public object, public std::enable_shared_from_this<IMemoryManager> {
public:
    using std::enable_shared_from_this<IMemoryManager>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    string tableName;

    std::shared_ptr<Function> constructor;

    virtual std::shared_ptr<Promise<std::shared_ptr<Memory>>> addEmbeddingToMemory(std::shared_ptr<Memory> memory) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemories(object opts) = 0;
    virtual std::shared_ptr<Promise<array<object>>> getCachedEmbeddings(string content) = 0;
    virtual std::shared_ptr<Promise<any>> getMemoryById(UUID id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> getMemoriesByRoomIds(object params) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<Memory>>>> searchMemoriesByEmbedding(array<double> embedding, object opts) = 0;
    virtual std::shared_ptr<Promise<void>> createMemory(std::shared_ptr<Memory> memory, boolean unique = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> removeMemory(UUID memoryId) = 0;
    virtual std::shared_ptr<Promise<void>> removeAllMemories(UUID roomId) = 0;
    virtual std::shared_ptr<Promise<double>> countMemories(UUID roomId, boolean unique = undefined) = 0;
};

class IRAGKnowledgeManager : public object, public std::enable_shared_from_this<IRAGKnowledgeManager> {
public:
    using std::enable_shared_from_this<IRAGKnowledgeManager>::shared_from_this;
    std::shared_ptr<IAgentRuntime> runtime;

    string tableName;

    virtual std::shared_ptr<Promise<array<std::shared_ptr<RAGKnowledgeItem>>>> getKnowledge(object params) = 0;
    virtual std::shared_ptr<Promise<void>> createKnowledge(std::shared_ptr<RAGKnowledgeItem> item) = 0;
    virtual std::shared_ptr<Promise<void>> removeKnowledge(UUID id) = 0;
    virtual std::shared_ptr<Promise<array<std::shared_ptr<RAGKnowledgeItem>>>> searchKnowledge(object params) = 0;
    virtual std::shared_ptr<Promise<void>> clearKnowledge(boolean shared = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> processFile(object file) = 0;
    virtual std::shared_ptr<Promise<void>> cleanupDeletedKnowledgeFiles() = 0;
    virtual UUID generateScopedId(string path, boolean isShared) = 0;
};

enum struct CacheStore {
    REDIS = std::string("redis"), DATABASE = std::string("database"), FILESYSTEM = std::string("filesystem")
};
class ICacheManager : public object, public std::enable_shared_from_this<ICacheManager> {
public:
    using std::enable_shared_from_this<ICacheManager>::shared_from_this;
    template <typename T>
    std::shared_ptr<Promise<any>> get(string key) = 0;
    template <typename T>
    std::shared_ptr<Promise<void>> set(string key, T value, CacheOptions options = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> delete(string key) = 0;
};

class Service : public object, public std::enable_shared_from_this<Service> {
public:
    using std::enable_shared_from_this<Service>::shared_from_this;
    static any instance;

    static ServiceType get_serviceType();
    template <typename T>
    static T getInstance();
    virtual ServiceType get_serviceType();
    virtual std::shared_ptr<Promise<void>> initialize(std::shared_ptr<IAgentRuntime> runtime) = 0;
};

class IAgentRuntime : public object, public std::enable_shared_from_this<IAgentRuntime> {
public:
    using std::enable_shared_from_this<IAgentRuntime>::shared_from_this;
    UUID agentId;

    string serverUrl;

    std::shared_ptr<IDatabaseAdapter> databaseAdapter;

    any token;

    ModelProviderName modelProvider;

    ModelProviderName imageModelProvider;

    ModelProviderName imageVisionModelProvider;

    Character character;

    array<std::shared_ptr<Provider>> providers;

    array<std::shared_ptr<Action>> actions;

    array<std::shared_ptr<Evaluator>> evaluators;

    array<Plugin> plugins;

    any fetch;

    std::shared_ptr<IMemoryManager> messageManager;

    std::shared_ptr<IMemoryManager> descriptionManager;

    std::shared_ptr<IMemoryManager> documentsManager;

    std::shared_ptr<IMemoryManager> knowledgeManager;

    std::shared_ptr<IRAGKnowledgeManager> ragKnowledgeManager;

    std::shared_ptr<IMemoryManager> loreManager;

    std::shared_ptr<ICacheManager> cacheManager;

    std::shared_ptr<Map<ServiceType, std::shared_ptr<Service>>> services;

    array<ClientInstance> clients;

    virtual std::shared_ptr<Promise<void>> initialize() = 0;
    virtual void registerMemoryManager(std::shared_ptr<IMemoryManager> manager) = 0;
    virtual any getMemoryManager(string name) = 0;
    template <typename T>
    any getService(ServiceType service) = 0;
    virtual void registerService(std::shared_ptr<Service> service) = 0;
    virtual any getSetting(string key) = 0;
    virtual double getConversationLength() = 0;
    virtual std::shared_ptr<Promise<void>> processActions(std::shared_ptr<Memory> message, array<std::shared_ptr<Memory>> responses, std::shared_ptr<State> state = undefined, HandlerCallback callback = undefined) = 0;
    virtual std::shared_ptr<Promise<any>> evaluate(std::shared_ptr<Memory> message, std::shared_ptr<State> state = undefined, boolean didRespond = undefined, HandlerCallback callback = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> ensureParticipantExists(UUID userId, UUID roomId) = 0;
    virtual std::shared_ptr<Promise<void>> ensureUserExists(UUID userId, any userName, any name, any source) = 0;
    virtual void registerAction(std::shared_ptr<Action> action) = 0;
    virtual std::shared_ptr<Promise<void>> ensureConnection(UUID userId, UUID roomId, string userName = undefined, string userScreenName = undefined, string source = undefined) = 0;
    virtual std::shared_ptr<Promise<void>> ensureParticipantInRoom(UUID userId, UUID roomId) = 0;
    virtual std::shared_ptr<Promise<void>> ensureRoomExists(UUID roomId) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<State>>> composeState(std::shared_ptr<Memory> message, object additionalKeys = undefined) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<State>>> updateRecentMessageState(std::shared_ptr<State> state) = 0;
};

class IImageDescriptionService : public Service, public std::enable_shared_from_this<IImageDescriptionService> {
public:
    using std::enable_shared_from_this<IImageDescriptionService>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> describeImage(string imageUrl) = 0;
};

class ITranscriptionService : public Service, public std::enable_shared_from_this<ITranscriptionService> {
public:
    using std::enable_shared_from_this<ITranscriptionService>::shared_from_this;
    virtual std::shared_ptr<Promise<any>> transcribeAttachment(std::shared_ptr<ArrayBuffer> audioBuffer) = 0;
    virtual std::shared_ptr<Promise<any>> transcribeAttachmentLocally(std::shared_ptr<ArrayBuffer> audioBuffer) = 0;
    virtual std::shared_ptr<Promise<any>> transcribe(std::shared_ptr<ArrayBuffer> audioBuffer) = 0;
    virtual std::shared_ptr<Promise<any>> transcribeLocally(std::shared_ptr<ArrayBuffer> audioBuffer) = 0;
};

class IVideoService : public Service, public std::enable_shared_from_this<IVideoService> {
public:
    using std::enable_shared_from_this<IVideoService>::shared_from_this;
    virtual boolean isVideoUrl(string url) = 0;
    virtual std::shared_ptr<Promise<Media>> fetchVideoInfo(string url) = 0;
    virtual std::shared_ptr<Promise<string>> downloadVideo(Media videoInfo) = 0;
    virtual std::shared_ptr<Promise<Media>> processVideo(string url, std::shared_ptr<IAgentRuntime> runtime) = 0;
};

class ITextGenerationService : public Service, public std::enable_shared_from_this<ITextGenerationService> {
public:
    using std::enable_shared_from_this<ITextGenerationService>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> initializeModel() = 0;
    virtual std::shared_ptr<Promise<any>> queueMessageCompletion(string context, double temperature, array<string> stop, double frequency_penalty, double presence_penalty, double max_tokens) = 0;
    virtual std::shared_ptr<Promise<string>> queueTextCompletion(string context, double temperature, array<string> stop, double frequency_penalty, double presence_penalty, double max_tokens) = 0;
    virtual std::shared_ptr<Promise<any>> getEmbeddingResponse(string input) = 0;
};

class IBrowserService : public Service, public std::enable_shared_from_this<IBrowserService> {
public:
    using std::enable_shared_from_this<IBrowserService>::shared_from_this;
    virtual std::shared_ptr<Promise<void>> closeBrowser() = 0;
    virtual std::shared_ptr<Promise<object>> getPageContent(string url, std::shared_ptr<IAgentRuntime> runtime) = 0;
};

class ISpeechService : public Service, public std::enable_shared_from_this<ISpeechService> {
public:
    using std::enable_shared_from_this<ISpeechService>::shared_from_this;
    virtual std::shared_ptr<ISpeechService> getInstance() = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<Readable>>> generate(std::shared_ptr<IAgentRuntime> runtime, string text) = 0;
};

class IPdfService : public Service, public std::enable_shared_from_this<IPdfService> {
public:
    using std::enable_shared_from_this<IPdfService>::shared_from_this;
    virtual std::shared_ptr<IPdfService> getInstance() = 0;
    virtual std::shared_ptr<Promise<string>> convertPdfToText(std::shared_ptr<Buffer> pdfBuffer) = 0;
};

class IAwsS3Service : public Service, public std::enable_shared_from_this<IAwsS3Service> {
public:
    using std::enable_shared_from_this<IAwsS3Service>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> uploadFile(string imagePath, string subDirectory, boolean useSignedUrl, double expiresIn) = 0;
    virtual std::shared_ptr<Promise<string>> generateSignedUrl(string fileName, double expiresIn) = 0;
};

class UploadIrysResult : public object, public std::enable_shared_from_this<UploadIrysResult> {
public:
    using std::enable_shared_from_this<UploadIrysResult>::shared_from_this;
    boolean success;

    string url;

    string error;

    any data;
};

class DataIrysFetchedFromGQL : public object, public std::enable_shared_from_this<DataIrysFetchedFromGQL> {
public:
    using std::enable_shared_from_this<DataIrysFetchedFromGQL>::shared_from_this;
    boolean success;

    any data;

    string error;
};

class GraphQLTag : public object, public std::enable_shared_from_this<GraphQLTag> {
public:
    using std::enable_shared_from_this<GraphQLTag>::shared_from_this;
    string name;

    array<any> values;
};

enum struct IrysMessageType {
    REQUEST = std::string("REQUEST"), DATA_STORAGE = std::string("DATA_STORAGE"), REQUEST_RESPONSE = std::string("REQUEST_RESPONSE")
};
enum struct IrysDataType {
    FILE = std::string("FILE"), IMAGE = std::string("IMAGE"), OTHER = std::string("OTHER")
};
class IrysTimestamp : public object, public std::enable_shared_from_this<IrysTimestamp> {
public:
    using std::enable_shared_from_this<IrysTimestamp>::shared_from_this;
    double from;

    double to;
};

class IIrysService : public Service, public std::enable_shared_from_this<IIrysService> {
public:
    using std::enable_shared_from_this<IIrysService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<DataIrysFetchedFromGQL>>> getDataFromAnAgent(array<string> agentsWalletPublicKeys, array<std::shared_ptr<GraphQLTag>> tags, std::shared_ptr<IrysTimestamp> timestamp) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UploadIrysResult>>> workerUploadDataOnIrys(any data, IrysDataType dataType, IrysMessageType messageType, array<string> serviceCategory, array<string> protocol, array<double> validationThreshold, array<double> minimumProviders, array<boolean> testProvider, array<double> reputation) = 0;
    virtual std::shared_ptr<Promise<std::shared_ptr<UploadIrysResult>>> providerUploadDataOnIrys(any data, IrysDataType dataType, array<string> serviceCategory, array<string> protocol) = 0;
};

class ITeeLogService : public Service, public std::enable_shared_from_this<ITeeLogService> {
public:
    using std::enable_shared_from_this<ITeeLogService>::shared_from_this;
    virtual std::shared_ptr<ITeeLogService> getInstance() = 0;
    virtual std::shared_ptr<Promise<boolean>> log(string agentId, string roomId, string userId, string type, string content) = 0;
};

enum struct ServiceType {
    IMAGE_DESCRIPTION = std::string("image_description"), TRANSCRIPTION = std::string("transcription"), VIDEO = std::string("video"), TEXT_GENERATION = std::string("text_generation"), BROWSER = std::string("browser"), SPEECH_GENERATION = std::string("speech_generation"), PDF = std::string("pdf"), INTIFACE = std::string("intiface"), AWS_S3 = std::string("aws_s3"), BUTTPLUG = std::string("buttplug"), SLACK = std::string("slack"), VERIFIABLE_LOGGING = std::string("verifiable_logging"), IRYS = std::string("irys"), TEE_LOG = std::string("tee_log"), GOPLUS_SECURITY = std::string("goplus_security"), WEB_SEARCH = std::string("web_search"), EMAIL_AUTOMATION = std::string("email_automation"), NKN_CLIENT_SERVICE = std::string("nkn_client_service"), DATABASE_MIGRATION = std::string("database_migration")
};
enum struct LoggingLevel {
    DEBUG = std::string("debug"), VERBOSE = std::string("verbose"), NONE = std::string("none")
};
class RAGKnowledgeItem : public object, public std::enable_shared_from_this<RAGKnowledgeItem> {
public:
    using std::enable_shared_from_this<RAGKnowledgeItem>::shared_from_this;
    UUID id;

    UUID agentId;

    object content;

    std::shared_ptr<Float32Array> embedding;

    double createdAt;

    double similarity;

    double score;
};

class ActionResponse : public object, public std::enable_shared_from_this<ActionResponse> {
public:
    using std::enable_shared_from_this<ActionResponse>::shared_from_this;
    boolean like;

    boolean retweet;

    boolean quote;

    boolean reply;
};

class ISlackService : public Service, public std::enable_shared_from_this<ISlackService> {
public:
    using std::enable_shared_from_this<ISlackService>::shared_from_this;
    any client;
};

enum struct TokenizerType {
    Auto = std::string("auto"), TikToken = std::string("tiktoken")
};
enum struct TranscriptionProvider {
    OpenAI = std::string("openai"), Deepgram = std::string("deepgram"), Local = std::string("local")
};
enum struct ActionTimelineType {
    ForYou = std::string("foryou"), Following = std::string("following")
};
enum struct KnowledgeScope {
    SHARED = std::string("shared"), PRIVATE = std::string("private")
};
enum struct CacheKeyPrefix {
    KNOWLEDGE = std::string("knowledge")
};
class DirectoryItem : public object, public std::enable_shared_from_this<DirectoryItem> {
public:
    using std::enable_shared_from_this<DirectoryItem>::shared_from_this;
    string directory;

    boolean shared;
};

class ChunkRow : public object, public std::enable_shared_from_this<ChunkRow> {
public:
    using std::enable_shared_from_this<ChunkRow>::shared_from_this;
    string id;
};

template <typename T>
T Service::getInstance()
{
    if (!Service::instance) {
        Service::instance = std::make_shared<(as<any>(Service))>();
    }
    return as<T>(Service::instance);
}

#endif
