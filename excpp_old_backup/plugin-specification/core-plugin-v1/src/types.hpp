#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Represents a UUID string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 */
using UUID = `${string}-${string}-${string}-${string}-${string}`;

/**
 * Represents the content of a message or communication
 */
struct Content {
    std::string text;
    std::optional<std::string> action;
    std::optional<std::string> source;
    std::optional<std::string> url;
    std::optional<UUID> inReplyTo;
    std::optional<std::vector<Media>> attachments;
};

/**
 * Example content with associated user for demonstration purposes
 */
struct ActionExample {
    std::string user;
    Content content;
};

/**
 * Example conversation content with user ID
 */
struct ConversationExample {
    UUID userId;
    Content content;
};

/**
 * Represents an actor/participant in a conversation
 */
struct Actor {
    std::string name;
    std::string username;
    { details;
    std::string tagline;
    std::string summary;
    std::string quote;
    UUID id;
};

/**
 * Represents a single objective within a goal
 */
struct Objective {
    std::optional<std::string> id;
    std::string description;
    bool completed;
};

/**
 * Status enum for goals
 */
enum GoalStatus {
  DONE = "DONE",
  FAILED = "FAILED",
  IN_PROGRESS = "IN_PROGRESS",
}

/**
 * Represents a high-level goal composed of objectives
 */
struct Goal {
    std::optional<UUID> id;
    UUID roomId;
    UUID userId;
    std::string name;
    GoalStatus status;
    std::vector<Objective> objectives;
};

/**
 * Model size/type classification
 */
enum ModelClass {
  SMALL = "small",
  MEDIUM = "medium",
  LARGE = "large",
  EMBEDDING = "embedding",
  IMAGE = "image",
}

/**
 * Model settings
 */
using ModelSettings = {
  /** Model name */

  /** Maximum input tokens */

  /** Maximum output tokens */

  /** Optional frequency penalty */

  /** Optional presence penalty */

  /** Optional repetition penalty */

  /** Stop sequences */

  /** Temperature setting */

  /** Optional telemetry configuration (experimental) */

/** Image model settings */
using ImageModelSettings = {

/** Embedding model settings */
using EmbeddingModelSettings = {

/**
 * Configuration for an AI model
 */
using Model = {
  /** Optional API endpoint */

  /** Model names by size class */

/**
 * Model configurations by provider
 */
using Models = {

/**
 * Available model providers
 */
enum ModelProviderName {
  OPENAI = "openai",
  ETERNALAI = "eternalai",
  ANTHROPIC = "anthropic",
  GROK = "grok",
  GROQ = "groq",
  LLAMACLOUD = "llama_cloud",
  TOGETHER = "together",
  LLAMALOCAL = "llama_local",
  LMSTUDIO = "lmstudio",
  GOOGLE = "google",
  MISTRAL = "mistral",
  CLAUDE_VERTEX = "claude_vertex",
  REDPILL = "redpill",
  OPENROUTER = "openrouter",
  OLLAMA = "ollama",
  HEURIST = "heurist",
  GALADRIEL = "galadriel",
  FAL = "falai",
  GAIANET = "gaianet",
  ALI_BAILIAN = "ali_bailian",
  VOLENGINE = "volengine",
  NANOGPT = "nanogpt",
  HYPERBOLIC = "hyperbolic",
  VENICE = "venice",
  NVIDIA = "nvidia",
  NINETEEN_AI = "nineteen_ai",
  AKASH_CHAT_API = "akash_chat_api",
  LIVEPEER = "livepeer",
  LETZAI = "letzai",
  DEEPSEEK = "deepseek",
  INFERA = "infera",
  BEDROCK = "bedrock",
  ATOMA = "atoma",
  SECRETAI = "secret_ai",
  NEARAI = "nearai",
}

/**
 * Represents the current state/context of a conversation
 */
struct State {
    std::optional<UUID> userId;
    std::optional<UUID> agentId;
    std::optional<std::string> bio;
    std::optional<std::string> lore;
    std::optional<std::string> messageDirections;
    std::optional<std::string> postDirections;
    std::optional<UUID> roomId;
    std::optional<std::string> agentName;
    std::optional<std::string> senderName;
    std::optional<std::string> actors;
    std::optional<std::vector<Actor>> actorsData;
    std::optional<std::string> goals;
    std::optional<std::vector<Goal>> goalsData;
    std::optional<std::string> recentMessages;
    std::optional<std::vector<Memory>> recentMessagesData;
    std::optional<std::string> actionNames;
    std::optional<std::string> actions;
    std::optional<std::vector<Action>> actionsData;
    std::optional<std::string> actionExamples;
    std::optional<std::string> providers;
    std::optional<Content> responseData;
    std::optional<std::vector<Memory>> recentInteractionsData;
    std::optional<std::string> recentInteractions;
    std::optional<std::string> formattedConversation;
    std::optional<std::string> knowledge;
    std::optional<std::vector<KnowledgeItem>> knowledgeData;
    std::optional<std::vector<RAGKnowledgeItem>> ragKnowledgeData;
    std::optional<std::string> text;
};

/**
 * Represents a stored memory/message
 */
struct Memory {
    std::optional<UUID> id;
    UUID userId;
    UUID agentId;
    std::optional<double> createdAt;
    Content content;
    std::optional<std::vector<double>> embedding;
    UUID roomId;
    std::optional<bool> unique;
    std::optional<double> similarity;
};

/**
 * Example message for demonstration
 */
struct MessageExample {
    std::string user;
    Content content;
};

/**
 * Handler function type for processing messages
 */
using Handler = (

/**
 * Callback function type for handlers
 */
using HandlerCallback = (

/**
 * Validator function type for actions/evaluators
 */
using Validator = (

/**
 * Represents an action the agent can perform
 */
struct Action {
    std::vector<std::string> similes;
    std::string description;
    std::vector<std::vector<ActionExample>> examples;
    Handler handler;
    std::string name;
    Validator validate;
    std::optional<bool> suppressInitialMessage;
};

/**
 * Example for evaluating agent behavior
 */
struct EvaluationExample {
    std::string context;
    std::vector<ActionExample> messages;
    std::string outcome;
};

/**
 * Evaluator for assessing agent responses
 */
struct Evaluator {
    std::optional<bool> alwaysRun;
    std::string description;
    std::vector<std::string> similes;
    std::vector<EvaluationExample> examples;
    Handler handler;
    std::string name;
    Validator validate;
};

/**
 * Provider for external data/services
 */
struct Provider {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<bool> dynamic;
    std::optional<double> position;
    std::optional<bool> private;
    (runtime: IAgentRuntime, message: Memory, state?: State) => Promise<any> get;
};

/**
 * Represents a relationship between users
 */
struct Relationship {
    UUID id;
    UUID userA;
    UUID userB;
    UUID userId;
    UUID roomId;
    std::string status;
    std::optional<std::string> createdAt;
};

/**
 * Represents a user account
 */
struct Account {
    UUID id;
    std::string name;
    std::string username;
    std::optional<{ [key: string]: any }> details;

/**
 * Room participant with account details
 */
struct Participant {
    UUID id;
    Account account;
};

/**
 * Represents a conversation room
 */
struct Room {
    UUID id;
    std::vector<Participant> participants;
};

/**
 * Represents a media attachment
 */
using Media = {
  /** Unique identifier */

  /** Media URL */

  /** Media title */

  /** Media source */

  /** Media description */

  /** Text content */

  /** Content type */

/**
 * Client instance
 */
using ClientInstance = {
  /** Client name */
  // name: string;

  /** Stop client connection */

/**
 * Client interface for platform connections
 */
using Client = {
  /** Client name */

  /** Client configuration */

  /** Start client connection */

/**
 * Database adapter initialization
 */
using Adapter = {
  /** Initialize the adapter */

/**
 * Plugin for extending agent functionality
 */
using Plugin = {
  /** Plugin name */

  /** Plugin npm name */

  /** Plugin configuration */

  /** Plugin description */

  /** Optional actions */

  /** Optional providers */

  /** Optional evaluators */

  /** Optional services */

  /** Optional clients */

  /** Optional adapters */

  /** Optional post charactor processor handler */

struct IAgentConfig {
};

using TelemetrySettings = {
  /**
   * Enable or disable telemetry. Disabled by default while experimental.
   */
  /**
   * Enable or disable input recording. Enabled by default.
   *
   * You might want to disable input recording to avoid recording sensitive
   * information, to reduce data transfers, or to increase performance.
   */
  /**
   * Enable or disable output recording. Enabled by default.
   *
   * You might want to disable output recording to avoid recording sensitive
   * information, to reduce data transfers, or to increase performance.
   */
  /**
   * Identifier for this function. Used to group telemetry data by function.
   */

struct ModelConfiguration {
    std::optional<double> temperature;
    std::optional<double> maxOutputTokens;
    std::optional<double> frequency_penalty;
    std::optional<double> presence_penalty;
    std::optional<double> maxInputTokens;
    std::optional<TelemetrySettings> experimental_telemetry;
};

using TemplateType = std::variant<std::string, ((options: { state: State }) => string)>;

/**
 * Configuration for an agent character
 */
using Character = {
  /** Optional unique identifier */

  /** Character name */

  /** Optional username */

  /** Optional email */

  /** Optional system prompt */

  /** Model provider to use */

  /** Image model provider to use, if different from modelProvider */

  /** Image Vision model provider to use, if different from modelProvider */

  /** Optional model endpoint override */

  /** Optional prompt templates */

  /** Character biography */

  /** Character background lore */

  /** Example messages */

  /** Example posts */

  /** Known topics */

  /** Character traits */

  /** Optional knowledge base */

  /** Available plugins */

  /** Character Processor Plugins */

  /** Optional configuration */
        // New structured ElevenLabs config

  /** Optional client-specific config */

  /** Writing style guides */

  /** Optional Twitter profile */

  /** Optional Instagram profile */

  /** Optional SimsAI profile */

  /** Optional NFT prompt */

  /**Optinal Parent characters to inherit information from */

struct TwitterSpaceDecisionOptions {
    std::optional<double> maxSpeakers;
    std::optional<std::vector<std::string>> topics;
    std::optional<double> typicalDurationMinutes;
    std::optional<double> idleKickTimeoutMs;
    std::optional<double> minIntervalBetweenSpacesMinutes;
    std::optional<bool> businessHoursOnly;
    std::optional<double> randomChance;
    std::optional<bool> enableIdleMonitor;
    std::optional<bool> enableSttTts;
    std::optional<bool> enableRecording;
    std::optional<std::string> voiceId;
    std::optional<std::string> sttLanguage;
    std::optional<double> speakerMaxDurationMs;
};

/**
 * Interface for database operations
 */
struct IDatabaseAdapter {
    std::any db;
    UUID roomId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::string tableName;
    UUID agentId;
    std::optional<double> start;
    std::optional<double> end;
    std::string tableName;
    UUID agentId;
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::string query_table_name;
    double query_threshold;
    std::string query_input;
    std::string query_field_name;
    std::string query_field_sub_name;
    double query_match_count;
    { [key: string]: unknown } body;

struct IDatabaseCacheAdapter {
    UUID agentId;
    std::string key;
    std::string value;
};

struct IMemoryManager {
    IAgentRuntime runtime;
    std::string tableName;
    std::function constructor;
    UUID roomId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::optional<double> start;
    std::optional<double> end;
    std::string content;
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::vector<double> embedding;
    { opts;
    std::optional<double> match_threshold;
    std::optional<double> count;
    UUID roomId;
    std::optional<bool> unique;
};

struct IRAGKnowledgeManager {
    IAgentRuntime runtime;
    std::string tableName;
    std::optional<std::string> query;
    std::optional<UUID> id;
    std::optional<double> limit;
    std::optional<std::string> conversationContext;
    std::optional<UUID> agentId;
    UUID agentId;
    std::vector<Float32Array | number> embedding;
    std::optional<double> match_threshold;
    std::optional<double> match_count;
    std::optional<std::string> searchText;
    std::string path;
    std::string content;
    "pdf" | "md" | "txt" type;
    bool isShared;
};

using CacheOptions = {

enum CacheStore {
  REDIS = "redis",
  DATABASE = "database",
  FILESYSTEM = "filesystem",
}

struct ICacheManager {
};

  // Add abstract initialize method that must be implemented by derived classes

struct IAgentRuntime {
    UUID agentId;
    std::string serverUrl;
    IDatabaseAdapter databaseAdapter;
    string | null token;
    ModelProviderName modelProvider;
    ModelProviderName imageModelProvider;
    ModelProviderName imageVisionModelProvider;
    Character character;
    std::vector<Provider> providers;
    std::vector<Action> actions;
    std::vector<Evaluator> evaluators;
    std::vector<Plugin> plugins;
    std::optional<typeof fetch | null> fetch;
    IMemoryManager messageManager;
    IMemoryManager descriptionManager;
    IMemoryManager documentsManager;
    IMemoryManager knowledgeManager;
    IRAGKnowledgeManager ragKnowledgeManager;
    IMemoryManager loreManager;
    ICacheManager cacheManager;
    std::unordered_map<ServiceType, Service> services;
    std::vector<ClientInstance> clients;
    Memory message;
    std::vector<Memory> responses;
    std::optional<State> state;
    std::optional<HandlerCallback> callback;
    Memory message;
    std::optional<State> state;
    std::optional<bool> didRespond;
    std::optional<HandlerCallback> callback;
    UUID userId;
    string | null userName;
    string | null name;
    string | null source;
    UUID userId;
    UUID roomId;
    std::optional<std::string> userName;
    std::optional<std::string> userScreenName;
    std::optional<std::string> source;
    Memory message;
    std::optional<{ [key: string]: unknown }> additionalKeys;

struct UploadIrysResult {
    bool success;
    std::optional<std::string> url;
    std::optional<std::string> error;
    std::optional<std::any> data;
};

struct DataIrysFetchedFromGQL {
    bool success;
    std::any data;
    std::optional<std::string> error;
};

struct GraphQLTag {
    std::string name;
    std::vector<std::any> values;
};

enum IrysMessageType {
  REQUEST = "REQUEST",
  DATA_STORAGE = "DATA_STORAGE",
  REQUEST_RESPONSE = "REQUEST_RESPONSE",
}

enum IrysDataType {
  FILE = "FILE",
  IMAGE = "IMAGE",
  OTHER = "OTHER",
}

struct IrysTimestamp {
    double from;
    double to;
};

enum ServiceType {
  IMAGE_DESCRIPTION = "image_description",
  TRANSCRIPTION = "transcription",
  VIDEO = "video",
  TEXT_GENERATION = "text_generation",
  BROWSER = "browser",
  SPEECH_GENERATION = "speech_generation",
  PDF = "pdf",
  INTIFACE = "intiface",
  AWS_S3 = "aws_s3",
  BUTTPLUG = "buttplug",
  SLACK = "slack",
  VERIFIABLE_LOGGING = "verifiable_logging",
  IRYS = "irys",
  TEE_LOG = "tee_log",
  GOPLUS_SECURITY = "goplus_security",
  WEB_SEARCH = "web_search",
  EMAIL_AUTOMATION = "email_automation",
  NKN_CLIENT_SERVICE = "nkn_client_service",
}

enum LoggingLevel {
  DEBUG = "debug",
  VERBOSE = "verbose",
  NONE = "none",
}

using KnowledgeItem = {

struct RAGKnowledgeItem {
    UUID id;
    UUID agentId;
    { content;
    std::string text;
    std::optional<{> metadata;
    std::optional<bool> isMain;
    std::optional<bool> isChunk;
    std::optional<UUID> originalId;
    std::optional<double> chunkIndex;
    std::optional<std::string> source;
    std::optional<std::string> type;
    std::optional<bool> isShared;
    std::optional<Float32Array> embedding;
    std::optional<double> createdAt;
    std::optional<double> similarity;
    std::optional<double> score;
};

struct ActionResponse {
    bool like;
    bool retweet;
    std::optional<bool> quote;
    std::optional<bool> reply;
};

enum TokenizerType {
  Auto = "auto",
  TikToken = "tiktoken",
}

enum TranscriptionProvider {
  OpenAI = "openai",
  Deepgram = "deepgram",
  Local = "local",
}

enum ActionTimelineType {
  ForYou = "foryou",
  Following = "following",
}
enum KnowledgeScope {
  SHARED = "shared",
  PRIVATE = "private",
}

enum CacheKeyPrefix {
  KNOWLEDGE = "knowledge",
}

struct DirectoryItem {
    std::string directory;
    std::optional<bool> shared;
};

struct ChunkRow {
    std::string id;
};


} // namespace elizaos
