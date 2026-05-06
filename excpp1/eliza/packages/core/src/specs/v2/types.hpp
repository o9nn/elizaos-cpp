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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Defines a custom type UUID representing a universally unique identifier
 */
using UUID = `${string}-${string}-${string}-${string}-${string}`;

/**
 * Helper function to safely cast a string to strongly typed UUID
 * @param id The string UUID to validate and cast
 * @returns The same UUID with branded type information
 */
UUID asUUID(const std::string& id);

/**
 * Represents the content of a memory, message, or other information
 */
struct Content {
    std::optional<std::string> thought;
    std::optional<std::string> text;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::vector<std::string>> providers;
    std::optional<std::string> source;
    std::optional<std::string> url;
    std::optional<UUID> inReplyTo;
    std::optional<std::vector<Media>> attachments;
};

/**
 * Example content with associated user for demonstration purposes
 */
struct ActionExample {
    std::string name;
    Content content;
};

using ModelTypeName = std::variant<(typeof ModelType)[keyof typeof ModelType], std::string>;

/**
 * Defines the recognized types of models that the agent runtime can use.
 * These include models for text generation (small, large, reasoning, completion),
 * text embedding, tokenization (encode/decode), image generation and description,
 * audio transcription, text-to-speech, and generic object generation.
 * This constant is used throughout the system, particularly in `AgentRuntime.useModel`,
 * `AgentRuntime.registerModel`, and in `ModelParamsMap` / `ModelResultMap` to ensure
 * type safety and clarity when working with different AI models.
 * String values are used for extensibility with custom model types.
 */

/**
 * Core service type registry that can be extended by plugins via module augmentation.
 * Plugins can extend this interface to add their own service types:
 *
 * @example
 * ```typescript
 * declare module '@elizaos/core' {
 *   struct ServiceTypeRegistry {

 * }
 * ```
 */
struct ServiceTypeRegistry {
    'transcription' TRANSCRIPTION;
    'video' VIDEO;
    'browser' BROWSER;
    'pdf' PDF;
    'aws_s3' REMOTE_FILES;
    'web_search' WEB_SEARCH;
    'email' EMAIL;
    'tee' TEE;
    'task' TASK;
};

/**
 * Type for service names that includes both core services and any plugin-registered services
 */
using ServiceTypeName = ServiceTypeRegistry[keyof ServiceTypeRegistry];

/**
 * Helper type to extract service type values from the registry
 */

/**
 * Helper type to check if a service type exists in the registry
 */

/**
 * Type-safe service class definition
 */

/**
 * Map of service type names to their implementation classes
 */
struct ServiceClassMap {
};

/**
 * Helper to infer service instance type from service type name
 */

/**
 * Runtime service registry type
 */

/**
 * Enumerates the recognized types of services that can be registered and used by the agent runtime.
 * Services provide specialized functionalities like audio transcription, video processing,
 * web browsing, PDF handling, file storage (e.g., AWS S3), web search, email integration,
 * secure execution via TEE (Trusted Execution Environment), and task management.
 * This constant is used in `AgentRuntime` for service registration and retrieval (e.g., `getService`).
 * Each service typically implements the `Service` abstract class or a more specific interface like `IVideoService`.
 */

/**
 * Represents the current state or context of a conversation or agent interaction.
 * This interface is a flexible container for various pieces of information that define the agent's
 * understanding at a point in time. It includes:
 * - `values`: A key-value store for general state variables, often populated by providers.
 * - `data`: Another key-value store, potentially for more structured or internal data.
 * - `text`: A string representation of the current context, often a summary or concatenated history.
 * The `[key: string]: any;` allows for dynamic properties, though `EnhancedState` offers better typing.
 * This state object is passed to handlers for actions, evaluators, and providers.
 */
struct State {
    std::string text;
};

/**
 * Memory type enumeration for built-in memory types
 */
using MemoryTypeAlias = std::string;

/**
 * Enumerates the built-in types of memories that can be stored and retrieved.
 * - `DOCUMENT`: Represents a whole document or a large piece of text.
 * - `FRAGMENT`: A chunk or segment of a `DOCUMENT`, often created for embedding and search.
 * - `MESSAGE`: A conversational message, typically from a user or the agent.
 * - `DESCRIPTION`: A descriptive piece of information, perhaps about an entity or concept.
 * - `CUSTOM`: For any other type of memory not covered by the built-in types.
 * This enum is used in `MemoryMetadata` to categorize memories and influences how they are processed or queried.
 */
enum MemoryType {
}
/**
 * Defines the scope of a memory, indicating its visibility and accessibility.
 * - `shared`: The memory is accessible to multiple entities or across different contexts (e.g., a public fact).
 * - `private`: The memory is specific to a single entity or a private context (e.g., a user's personal preference).
 * - `room`: The memory is scoped to a specific room or channel.
 * This is used in `MemoryMetadata` to control how memories are stored and retrieved based on context.
 */
using MemoryScope = std::variant<'shared', 'private', 'room'>;

/**
 * Base interface for all memory metadata types.
 * It includes common properties for all memories, such as:
 * - `type`: The kind of memory (e.g., `MemoryType.MESSAGE`, `MemoryType.DOCUMENT`).
 * - `source`: An optional string indicating the origin of the memory (e.g., 'discord', 'user_input').
 * - `sourceId`: An optional UUID linking to a source entity or object.
 * - `scope`: The visibility scope of the memory (`shared`, `private`, or `room`).
 * - `timestamp`: An optional numerical timestamp (e.g., milliseconds since epoch) of when the memory was created or relevant.
 * - `tags`: Optional array of strings for categorizing or filtering memories.
 * Specific metadata types like `DocumentMetadata` or `MessageMetadata` extend this base.
 */
struct BaseMetadata {
    MemoryTypeAlias type;
    std::optional<std::string> source;
    std::optional<UUID> sourceId;
    std::optional<MemoryScope> scope;
    std::optional<double> timestamp;
    std::optional<std::vector<std::string>> tags;
};

using MemoryMetadata = std::variant<, DocumentMetadata, FragmentMetadata, MessageMetadata, DescriptionMetadata, CustomMetadata>;

/**
 * Represents a stored memory/message
 */
struct Memory {
    std::optional<UUID> id;
    UUID entityId;
    std::optional<UUID> agentId;
    std::optional<double> createdAt;
    Content content;
    std::optional<std::vector<double>> embedding;
    UUID roomId;
    std::optional<UUID> worldId;
    std::optional<bool> unique;
    std::optional<double> similarity;
    std::optional<MemoryMetadata> metadata;
};

/**
 * Represents a log entry
 */
struct Log {
    std::optional<UUID> id;
    UUID entityId;
    std::optional<UUID> roomId;
    std::string type;
    Date createdAt;
};

/**
 * Example message for demonstration
 */
struct MessageExample {
    std::string name;
    Content content;
};

/**
 * Handler function type for processing messages
 */
using Handler = std::function<std::future<unknown>(IAgentRuntime, Memory, State, std::any, HandlerCallback, std::vector<Memory>)>;

/**
 * Callback function type for handlers
 */
using HandlerCallback = std::function<std::future<std::vector<Memory>>(Content, std::any)>;

/**
 * Validator function type for actions/evaluators
 */
using Validator = std::function<std::future<bool>(IAgentRuntime, Memory, State)>;

/**
 * Represents an action the agent can perform
 */
struct Action {
    std::optional<std::vector<std::string>> similes;
    std::string description;
    std::optional<std::vector<std::vector<ActionExample>>> examples;
    Handler handler;
    std::string name;
    Validator validate;
};

/**
 * Example for evaluating agent behavior
 */
struct EvaluationExample {
    std::string prompt;
    std::vector<ActionExample> messages;
    std::string outcome;
};

/**
 * Evaluator for assessing agent responses
 */
struct Evaluator {
    std::optional<bool> alwaysRun;
    std::string description;
    std::optional<std::vector<std::string>> similes;
    std::vector<EvaluationExample> examples;
    Handler handler;
    std::string name;
    Validator validate;
};

struct ProviderResult {
    std::optional<std::string> text;
};

/**
 * Provider for external data/services
 */
struct Provider {
    std::string name;
    std::optional<std::string> description;
    std::optional<bool> dynamic;
    std::optional<double> position;
    std::optional<bool> private;
};

/**
 * Represents a relationship between users
 */
struct Relationship {
    UUID id;
    UUID sourceEntityId;
    UUID targetEntityId;
    UUID agentId;
    std::vector<std::string> tags;
    std::optional<std::string> createdAt;
};

struct Component {
    UUID id;
    UUID entityId;
    UUID agentId;
    UUID roomId;
    UUID worldId;
    UUID sourceEntityId;
    std::string type;
    double createdAt;
};

/**
 * Represents a user account
 */
struct Entity {
    std::optional<UUID> id;
    std::vector<std::string> names;
    UUID agentId;
    std::optional<std::vector<Component>> components;
};

using World = {

using RoomMetadata = {
  // No sourceChannelId needed here, Room::channelId IS the source/central channel ID

using Room = {

/**
 * Room participant with account details
 */
struct Participant {
    UUID id;
    Entity entity;
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

enum ContentType {
}

enum ChannelType {
  // Legacy types - kept for backward compatibility but should be replaced
}

using Route = std::variant<{

/**
 * Plugin for extending agent functionality
 */
using PluginEvents = std::vector<{

struct Plugin {
    std::string name;
    std::string description;
    std::string name;
    std::optional<std::vector<Action>> actions;
    std::optional<std::vector<Provider>> providers;
    std::optional<std::vector<Evaluator>> evaluators;
    std::optional<IDatabaseAdapter> adapter;
    std::optional<PluginEvents> events;
    std::optional<std::vector<Route>> routes;
    std::optional<std::vector<TestSuite>> tests;
    std::optional<std::vector<std::string>> dependencies;
    std::optional<double> priority;
};

struct ProjectAgent {
    Character character;
    std::optional<std::vector<Plugin>> plugins;
    std::optional<std::variant<TestSuite, std::vector<TestSuite>>> tests;
};

struct Project {
    std::vector<ProjectAgent> agents;
};

using TemplateType = std::variant<, std::string, ((options: { state: State, std::function<void()>>;

/**
 * Configuration for an agent's character, defining its personality, knowledge, and capabilities.
 * This is a central piece of an agent's definition, used by the `AgentRuntime` to initialize and operate the agent.
 * It includes:
 * - `id`: Optional unique identifier for the character.
 * - `name`, `username`: Identifying names for the character.
 * - `system`: A system prompt that guides the agent's overall behavior.
 * - `templates`: A map of prompt templates for various situations (e.g., message generation, summarization).
 * - `bio`: A textual biography or description of the character.
 * - `messageExamples`, `postExamples`: Examples of how the character communicates.
 * - `topics`, `adjectives`: Keywords describing the character's knowledge areas and traits.
 * - `knowledge`: Paths to knowledge files or directories to be loaded into the agent's memory.
 * - `plugins`: A list of plugin names to be loaded for this character.
 * - `settings`, `secrets`: Configuration key-value pairs, with secrets being handled more securely.
 * - `style`: Guidelines for the character's writing style in different contexts (chat, post).
 */
struct Character {
    std::optional<UUID> id;
    std::string name;
    std::optional<std::string> username;
    std::optional<std::string> system;
    std::variant<std::string, std::vector<std::string>> bio;
    std::optional<std::vector<std::vector<MessageExample>>> messageExamples;
    std::optional<std::vector<std::string>> postExamples;
    std::optional<std::vector<std::string>> topics;
    std::optional<std::vector<std::string>> adjectives;
    std::optional<std::vector<std::string>> plugins;
    std::optional<std::vector<std::string>> all;
    std::optional<std::vector<std::string>> chat;
    std::optional<std::vector<std::string>> post;
};

enum AgentStatus {
}

/**
 * Represents an operational agent, extending the `Character` definition with runtime status and timestamps.
 * While `Character` defines the blueprint, `Agent` represents an instantiated and potentially running version.
 * It includes:
 * - `enabled`: A boolean indicating if the agent is currently active or disabled.
 * - `status`: The current operational status, typically `AgentStatus.ACTIVE` or `AgentStatus.INACTIVE`.
 * - `createdAt`, `updatedAt`: Timestamps for when the agent record was created and last updated in the database.
 * This interface is primarily used by the `IDatabaseAdapter` for agent management.
 */

/**
 * Interface for database operations
 */
struct IDatabaseAdapter {
    std::any db;
    std::optional<UUID> sourceEntityId;
    std::optional<UUID> entityId;
    std::optional<UUID> agentId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::string tableName;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::string tableName;
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::string query_table_name;
    double query_threshold;
    std::string query_input;
    std::string query_field_name;
    std::string query_field_sub_name;
    double query_match_count;
    UUID entityId;
    UUID roomId;
    std::string type;
    UUID entityId;
    std::optional<UUID> roomId;
    std::optional<std::string> type;
    std::optional<double> count;
    std::optional<double> offset;
    std::vector<double> embedding;
    std::optional<double> match_threshold;
    std::optional<double> count;
    std::optional<bool> unique;
    std::string tableName;
    std::optional<std::string> query;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::optional<UUID> entityId;
    std::variant<'FOLLOWED', 'MUTED'> state;
    UUID sourceEntityId;
    UUID targetEntityId;
    std::optional<std::vector<std::string>> tags;
    UUID sourceEntityId;
    UUID targetEntityId;
    std::optional<std::vector<std::string>> tags;
    UUID worldId;
    std::optional<double> count;
    std::optional<std::string> tableName;
};

/**
 * Result interface for embedding similarity searches
 */
struct EmbeddingSearchResult {
    std::vector<double> embedding;
    double levenshtein_score;
};

/**
 * Options for memory retrieval operations
 */
struct MemoryRetrievalOptions {
    UUID roomId;
    std::optional<double> count;
    std::optional<bool> unique;
    std::optional<double> start;
    std::optional<double> end;
    std::optional<UUID> agentId;
};

/**
 * Options for memory search operations
 */
struct MemorySearchOptions {
    std::vector<double> embedding;
    std::optional<double> match_threshold;
    std::optional<double> count;
    UUID roomId;
    std::optional<UUID> agentId;
    std::optional<bool> unique;
    std::optional<std::optional<MemoryMetadata>> metadata;
};

/**
 * Options for multi-room memory retrieval
 */
struct MultiRoomMemoryOptions {
    std::vector<UUID> roomIds;
    std::optional<double> limit;
    std::optional<UUID> agentId;
};

/**
 * Unified options pattern for memory operations
 * Provides a simpler, more consistent interface
 */
struct UnifiedMemoryOptions {
    UUID roomId;
    std::optional<double> limit;
    std::optional<UUID> agentId;
    std::optional<bool> unique;
    std::optional<double> start;
    std::optional<double> end;
};

/**
 * Specialized memory search options
 */

/**
 * Information describing the target of a message.
 */
struct TargetInfo {
    std::string source;
    std::optional<UUID> roomId;
    std::optional<std::string> channelId;
    std::optional<std::string> serverId;
    std::optional<UUID> entityId;
    std::optional<std::string> threadId;
};

/**
 * Function signature for handlers responsible for sending messages to specific platforms.
 */
using SendHandlerFunction = std::function<std::future<void>(IAgentRuntime, TargetInfo, Content)>;

/**
 * Represents the core runtime environment for an agent.
 * Defines methods for database interaction, plugin management, event handling,
 * state composition, model usage, and task management.
 */
  // Properties

  // Methods

  // Keep these methods for backward compatibility

  /**
   * Use a model with strongly typed parameters and return values based on model type
   * @template T - The model type to use
   * @template R - The expected return type, defaults to the type defined in ModelResultMap[T]
   * @param {T} modelType - The type of model to use
   * @param {ModelParamsMap[T] | any} params - The parameters for the model, typed based on model type
   * @returns {Promise<R>} - The model result, typed based on the provided generic type parameter
   */

  // In-memory task definition methods

  // Run tracking methods

  // easy/compat wrappers

  /**
   * Registers a handler function responsible for sending messages to a specific source/platform.
   * @param source - The unique identifier string for the source (e.g., 'discord', 'telegram').
   * @param handler - The SendHandlerFunction to be called for this source.
   */

  /**
   * Sends a message to a specified target using the appropriate registered handler.
   * @param target - Information describing the target recipient and platform.
   * @param content - The message content to send.
   * @returns Promise resolving when the message sending process is initiated or completed.
   */

/**
 * Interface for settings object with key-value pairs.
 */
/**
 * Interface representing settings with string key-value pairs.
 */
struct RuntimeSettings {
};

/**
 * Represents a single item of knowledge that can be processed and stored by the agent.
 * Knowledge items consist of content (text and optional structured data) and metadata.
 * These items are typically added to the agent's knowledge base via `AgentRuntime.addKnowledge`
 * and retrieved using `AgentRuntime.getKnowledge`.
 * The `id` is a unique identifier for the knowledge item, often derived from its source or content.
 */
using KnowledgeItem = {
  /** A Universally Unique Identifier for this specific knowledge item:: */
  /** The actual content of the knowledge item, which must include text and can have other fields. */
  /** Optional metadata associated with this knowledge item, conforming to `MemoryMetadata`. */

/**
 * Defines the scope or visibility of knowledge items within the agent's system.
 * - `SHARED`: Indicates knowledge that is broadly accessible, potentially across different agents or users if the system architecture permits.
 * - `PRIVATE`: Indicates knowledge that is restricted, typically to the specific agent or user context it belongs to.
 * This enum is used to manage access and retrieval of knowledge items, often in conjunction with `AgentRuntime.addKnowledge` or `AgentRuntime.getKnowledge` scopes.
 */
enum KnowledgeScope {
}

/**
 * Specifies prefixes for keys used in caching mechanisms, helping to namespace cached data.
 * For example, `KNOWLEDGE` might be used to prefix keys for cached knowledge embeddings or processed documents.
 * This helps in organizing the cache and avoiding key collisions.
 * Used internally by caching strategies, potentially within `IDatabaseAdapter` cache methods or runtime caching layers.
 */
enum CacheKeyPrefix {
}

/**
 * Represents an item within a directory listing, specifically for knowledge loading.
 * When an agent's `Character.knowledge` configuration includes a directory, this type
 * is used to specify the path to that directory and whether its contents should be treated as shared.
 * - `directory`: The path to the directory containing knowledge files.
 * - `shared`: An optional boolean (defaults to false) indicating if the knowledge from this directory is considered shared or private.
 */
struct DirectoryItem {
    std::string directory;
    std::optional<bool> shared;
};

/**
 * Represents a row structure, typically from a database query related to text chunking or processing.
 * This interface is quite minimal and seems to be a placeholder or a base for more specific chunk-related types.
 * The `id` would be the unique identifier for the chunk.
 * It might be used when splitting large documents into smaller, manageable pieces for embedding or analysis.
 */
struct ChunkRow {
    std::string id;
};

/**
 * Parameters for generating text using a language model.
 * This structure is typically passed to `AgentRuntime.useModel` when the `modelType` is one of
 * `ModelType.TEXT_SMALL`, `ModelType.TEXT_LARGE`, `ModelType.TEXT_REASONING_SMALL`,
 * `ModelType.TEXT_REASONING_LARGE`, or `ModelType.TEXT_COMPLETION`.
 * It includes essential information like the prompt, model type, and various generation controls.
 */
using GenerateTextParams = {
  /** The `AgentRuntime` instance, providing access to models and other services:: */
  /** The input string or prompt that the language model will use to generate text. */
  /** Specifies the type of text generation model to use (e.g., TEXT_LARGE, REASONING_SMALL). */
  /** Optional. The maximum number of tokens to generate in the response. */
  /** Optional. Controls randomness (0.0-1.0). Lower values are more deterministic, higher are more creative. */
  /** Optional. Penalizes new tokens based on their existing frequency in the text so far. */
  /** Optional. Penalizes new tokens based on whether they appear in the text so far. */
  /** Optional. A list of sequences at which the model will stop generating further tokens. */

/**
 * Parameters for tokenizing text, i.e., converting a string into a sequence of numerical tokens.
 * This is a common preprocessing step for many language models.
 * This structure is used with `AgentRuntime.useModel` when the `modelType` is `ModelType.TEXT_TOKENIZER_ENCODE`.
 */
struct TokenizeTextParams {
    std::string prompt;
    ModelTypeName modelType;
};

/**
 * Parameters for detokenizing text, i.e., converting a sequence of numerical tokens back into a string.
 * This is the reverse operation of tokenization.
 * This structure is used with `AgentRuntime.useModel` when the `modelType` is `ModelType.TEXT_TOKENIZER_DECODE`.
 */
struct DetokenizeTextParams {
    std::vector<double> tokens;
    ModelTypeName modelType;
};

/**
 * Represents a test case for evaluating agent or plugin functionality.
 * Each test case has a name and a function that contains the test logic.
 * The test function receives the `IAgentRuntime` instance, allowing it to interact with the agent's capabilities.
 * Test cases are typically grouped into `TestSuite`s.
 */
struct TestCase {
    std::string name;
};

/**
 * Represents a suite of related test cases for an agent or plugin.
 * This helps in organizing tests and running them collectively.
 * A `ProjectAgent` can define one or more `TestSuite`s.
 */
struct TestSuite {
    std::string name;
    std::vector<TestCase> tests;
};

// Represents an agent in the TeeAgent table, containing details about the agent.
/**
 * Represents an agent's registration details within a Trusted Execution Environment (TEE) context.
 * This is typically stored in a database table (e.g., `TeeAgent`) to manage agents operating in a TEE.
 * It allows for multiple registrations of the same `agentId` to support scenarios where an agent might restart,
 * generating a new keypair and attestation each time.
 */
struct TeeAgent {
    std::string id;
    std::string agentId;
    std::string agentName;
    double createdAt;
    std::string publicKey;
    std::string attestation;
};

/**
 * Defines the operational modes for a Trusted Execution Environment (TEE).
 * This enum is used to configure how TEE functionalities are engaged, allowing for
 * different setups for local development, Docker-based development, and production.
 */
enum TEEMode {
  /** TEE functionality is completely disabled. */
  /** For local development, potentially using a TEE simulator. */
  /** For Docker-based development environments, possibly with a TEE simulator. */
  /** For production deployments, using actual TEE hardware without a simulator. */
}

/**
 * Represents a quote obtained during remote attestation for a Trusted Execution Environment (TEE).
 * This quote is a piece of evidence provided by the TEE, cryptographically signed, which can be
 * verified by a relying party to ensure the TEE's integrity and authenticity.
 */
struct RemoteAttestationQuote {
    std::string quote;
    double timestamp;
};

/**
 * Data structure used in the attestation process for deriving a key within a Trusted Execution Environment (TEE).
 * This information helps establish a secure channel or verify the identity of the agent instance
 * requesting key derivation.
 */
struct DeriveKeyAttestationData {
    std::string agentId;
    std::string publicKey;
    std::optional<std::string> subject;
};

/**
 * Represents a message that has been attested by a Trusted Execution Environment (TEE).
 * This structure binds a message to an agent's identity and a timestamp, all within the
 * context of a remote attestation process, ensuring the message originated from a trusted TEE instance.
 */
struct RemoteAttestationMessage {
    std::string agentId;
    double timestamp;
    std::string entityId;
    std::string roomId;
    std::string content;
};

/**
 * Enumerates different types or vendors of Trusted Execution Environments (TEEs).
 * This allows the system to adapt to specific TEE technologies, like Intel TDX on DSTACK.
 */
enum TeeType {
  /** Represents Intel Trusted Domain Extensions (TDX) running on DSTACK infrastructure. */
}

/**
 * Configuration options specific to a particular Trusted Execution Environment (TEE) vendor.
 * This allows for vendor-specific settings to be passed to the TEE plugin or service.
 * The structure is a generic key-value map, as configurations can vary widely between vendors.
 */
struct TeeVendorConfig {
};

/**
 * Configuration for a TEE (Trusted Execution Environment) plugin.
 * This allows specifying the TEE vendor and any vendor-specific configurations.
 * It's used to initialize and configure TEE-related functionalities within the agent system.
 */
struct TeePluginConfig {
    std::optional<std::string> vendor;
    std::optional<TeeVendorConfig> vendorConfig;
};

/**
 * Defines the contract for a Task Worker, which is responsible for executing a specific type of task.
 * Task workers are registered with the `AgentRuntime` and are invoked when a `Task` of their designated `name` needs processing.
 * This pattern allows for modular and extensible background task processing.
 */
struct TaskWorker {
    std::string name;
    Task task;
};

/**
 * Defines metadata associated with a `Task`.
 * This can include scheduling information like `updateInterval` or UI-related details
 * for presenting task options to a user.
 * The `[key: string]: unknown;` allows for additional, unspecified metadata fields.
 */
using TaskMetadata = {
  /** Optional. If the task is recurring, this specifies the interval in milliseconds between updates or executions. */
  /** Optional. Describes options or parameters that can be configured for this task, often for UI presentation. */
  /** Allows for other dynamic metadata properties related to the task. */

/**
 * Represents a task to be performed, often in the background or at a later time.
 * Tasks are managed by the `AgentRuntime` and processed by registered `TaskWorker`s.
 * They can be associated with a room, world, and tagged for categorization and retrieval.
 * The `IDatabaseAdapter` handles persistence of task data.
 */
struct Task {
    std::optional<UUID> id;
    std::string name;
    std::optional<double> updatedAt;
    std::optional<TaskMetadata> metadata;
    std::string description;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::optional<UUID> entityId;
    std::vector<std::string> tags;
};

/**
 * Defines roles within a system, typically for access control or permissions, often within a `World`.
 * - `OWNER`: Represents the highest level of control, typically the creator or primary administrator.
 * - `ADMIN`: Represents administrative privileges, usually a subset of owner capabilities.
 * - `NONE`: Indicates no specific role or default, minimal permissions.
 * These roles are often used in `World.metadata.roles` to assign roles to entities.
 */
enum Role {
}

struct Setting {
    std::string name;
    std::string description;
    std::string usageDescription;
    std::variant<std::string, bool> value;
    bool required;
    std::optional<bool> public;
    std::optional<bool> secret;
    std::optional<std::vector<std::string>> dependsOn;
};

struct WorldSettings {
};

struct OnboardingConfig {
};

/**
 * Base parameters common to all model types
 */
struct BaseModelParams {
    IAgentRuntime runtime;
};

/**
 * Parameters for text generation models
 */
  /** The prompt to generate text from */
  /** Model temperature (0.0 to 1.0, lower is more deterministic) */
  /** Maximum number of tokens to generate */
  /** Sequences that should stop generation when encountered */
  /** Frequency penalty to apply */
  /** Presence penalty to apply */

/**
 * Parameters for text embedding models
 */
  /** The text to create embeddings for */

/**
 * Parameters for image generation models
 */
  /** The prompt describing the image to generate */
  /** The dimensions of the image to generate */
  /** Number of images to generate */

/**
 * Parameters for image description models
 */
  /** The URL or path of the image to describe */
  /** Optional prompt to guide the description */

/**
 * Parameters for transcription models
 */
  /** The URL or path of the audio file to transcribe */
  /** Optional prompt to guide transcription */

/**
 * Parameters for text-to-speech models
 */
  /** The text to convert to speech */
  /** The voice to use */
  /** The speaking speed */

/**
 * Parameters for audio processing models
 */
  /** The URL or path of the audio file to process */
  /** The type of audio processing to perform */

/**
 * Parameters for video processing models
 */
  /** The URL or path of the video file to process */
  /** The type of video processing to perform */

/**
 * Optional JSON schema for validating generated objects
 */
using JSONSchema = {

/**
 * Parameters for object generation models
 * @template T - The expected return type, inferred from schema if provided
 */
  /** The prompt describing the object to generate */
  /** Optional JSON schema for validation */
  /** Type of object to generate */
  /** For enum type, the allowed values */
  /** Model type to use */
  /** Model temperature (0.0 to 1.0) */
  /** Sequences that should stop generation */

/**
 * Map of model types to their parameter types
 */
struct ModelParamsMap {
};

/**
 * Map of model types to their return value types
 */
struct ModelResultMap {
};

/**
 * Standard event types across all platforms
 */
enum EventType {
  // World events

  // Entity events

  // Room events

  // Message events

  // Channel events

  // Voice events

  // Interaction events

  // Run events

  // Action events

  // Evaluator events

  // Model events
}

/**
 * Platform-specific event type prefix
 */
enum PlatformPrefix {
}

/**
 * Base payload interface for all events
 */
struct EventPayload {
    IAgentRuntime runtime;
    std::string source;
};

/**
 * Payload for world-related events
 */

/**
 * Payload for entity-related events
 */

/**
 * Payload for reaction-related events
 */

/**
 * Payload for channel cleared events
 */

/**
 * Payload for events that are invoked without a message
 */

/**
 * Run event payload type
 */

/**
 * Action event payload type
 */

/**
 * Evaluator event payload type
 */

/**
 * Model event payload type
 */

/**
 * Represents the parameters for a message received handler.
 * @typedef {Object} MessageReceivedHandlerParams
 * @property {IAgentRuntime} runtime - The agent runtime associated with the message.
 * @property {Memory} message - The message received.
 * @property {HandlerCallback} callback - The callback function to be executed after handling the message.
 */
using MessageReceivedHandlerParams = {

/**
 * Maps event types to their corresponding payload types
 */
struct EventPayloadMap {
};

/**
 * Event handler function type
 */

/**
 * Update the Plugin interface with typed events
 */

enum SOCKET_MESSAGE_TYPE {
}

/**
 * Specialized memory type for messages with enhanced type checking
 */

/**
 * Factory function to create a new message memory with proper defaults
 */
MessageMemory createMessageMemory(std::optional<std::any> params);

/**
 * Generic service interface that provides better type checking for services
 * @template ConfigType The configuration type for this service
 * @template ResultType The result type returned by the service operations
 */
  /**
   * The configuration for this service instance
   */

  /**
   * Process an input with this service
   * @param input The input to process
   * @returns A promise resolving to the result
   */

/**
 * Generic factory function to create a typed service instance
 * @param runtime The agent runtime
 * @param serviceType The type of service to get
 * @returns The service instance or null if not available
 */

/**
 * Type guard to check if a memory metadata is a DocumentMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a DocumentMetadata
 */

/**
 * Type guard to check if a memory metadata is a FragmentMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a FragmentMetadata
 */

/**
 * Type guard to check if a memory metadata is a MessageMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a MessageMetadata
 */

/**
 * Type guard to check if a memory metadata is a DescriptionMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a DescriptionMetadata
 */

/**
 * Type guard to check if a memory metadata is a CustomMetadata
 * @param metadata The metadata to check
 * @returns True if the metadata is a CustomMetadata
 */

/**
 * Standardized service error type for consistent error handling
 */
struct ServiceError {
    std::string code;
    std::string message;
    std::optional<unknown> details;
    std::optional<Error> cause;
};

/**
 * Memory type guard for document memories
 */

/**
 * Memory type guard for fragment memories
 */

/**
 * Safely access the text content of a memory
 * @param memory The memory to extract text from
 * @param defaultValue Optional default value if no text is found
 * @returns The text content or default value
 */
std::string getMemoryText(Memory memory, auto defaultValue = '');

/**
 * Safely create a ServiceError from any caught error
 */
ServiceError createServiceError(unknown error, auto code = 'UNKNOWN_ERROR');

/**
 * Replace 'any' types with more specific types
 */

// Replace 'any' in State interface components
/**
 * Defines the possible primitive types or structured types for a value within the agent's state.
 * This type is used to provide more specific typing for properties within `StateObject` and `StateArray`,
 * moving away from a generic 'any' type for better type safety and clarity in state management.
 */
using StateValue = std::variant<std::string, double, bool, nullptr, StateObject, StateArray>;
/**
 * Represents a generic object structure within the agent's state, where keys are strings
 * and values can be any `StateValue`. This allows for nested objects within the state.
 * It's a fundamental part of the `EnhancedState` interface.
 */
struct StateObject {
};

/**
 * Represents an array of `StateValue` types within the agent's state.
 * This allows for lists of mixed or uniform types to be stored in the state,
 * contributing to the structured definition of `EnhancedState`.
 */
using StateArray = std::vector<StateValue>;

/**
 * Enhanced State interface with more specific types for its core properties.
 * This interface provides a more structured representation of an agent's conversational state,
 * building upon the base `State` by typing `values` and `data` as `StateObject`.
 * The `text` property typically holds a textual summary or context derived from the state.
 * Additional dynamic properties are still allowed via the index signature `[key: string]: StateValue;`.
 */
struct EnhancedState {
    StateObject values;
    StateObject data;
    std::string text;
};

// Replace 'any' in component data
/**
 * A generic type for the `data` field within a `Component`.
 * While `Record<string, unknown>` allows for flexibility, developers are encouraged
 * to define more specific types for component data where possible to improve type safety
 * and code maintainability. This type serves as a base for various component implementations.
 */
using ComponentData = std::unordered_map<std::string, unknown>;

// Replace 'any' in event handlers
/**
 * Represents a generic data object that can be passed as a payload in an event.
 * This type is often used in `TypedEventHandler` to provide a flexible yet somewhat
 * structured way to handle event data. Specific event handlers might cast this to a
 * more concrete type based on the event being processed.
 */
using EventDataObject = std::unordered_map<std::string, unknown>;

/**
 * Defines a more specific type for event handlers, expecting an `EventDataObject`.
 * This aims to improve upon generic 'any' type handlers, providing a clearer contract
 * for functions that respond to events emitted within the agent runtime (see `emitEvent` in `AgentRuntime`).
 * Handlers can be synchronous or asynchronous.
 */
using TypedEventHandler = std::variant<std::function<std::future<void>(EventDataObject)>, void>;

// Replace 'any' in database adapter
/**
 * Represents a generic database connection object.
 * The actual type of this connection will depend on the specific database adapter implementation
 * (e.g., a connection pool object for PostgreSQL, a client instance for a NoSQL database).
 * This `unknown` type serves as a placeholder in the abstract `IDatabaseAdapter`.
 */
using DbConnection = unknown;

/**
 * A generic type for metadata objects, often used in various parts of the system like
 * `Relationship` metadata or other extensible data structures.
 * It allows for arbitrary key-value pairs where values are of `unknown` type,
 * encouraging consumers to perform type checking or casting.
 */
using MetadataObject = std::unordered_map<std::string, unknown>;

// Replace 'any' in model handlers
/**
 * Defines the structure for a model handler registration within the `AgentRuntime`.
 * Each model (e.g., for text generation, embedding) is associated with a handler function,
 * the name of the provider (plugin or system) that registered it, and an optional priority.
 * The `priority` (higher is more preferred) helps in selecting which handler to use if multiple
 * handlers are registered for the same model type. The `registrationOrder` (not in type, but used in runtime)
 * serves as a tie-breaker. See `AgentRuntime.registerModel` and `AgentRuntime.getModel`.
 */
struct ModelHandler {
    std::string provider;
    std::optional<double> priority;
    std::optional<double> registrationOrder;
};

// Replace 'any' for service configurationa
/**
 * A generic type for service configurations.
 * Services (like `IVideoService`, `IBrowserService`) can have their own specific configuration
 * structures. This type allows for a flexible way to pass configuration objects,
 * typically used during service initialization within a plugin or the `AgentRuntime`.
 */
using ServiceConfig = std::unordered_map<std::string, unknown>;

// Allowable vector dimensions

/**
 * Interface for control messages sent from the backend to the frontend
 * to manage UI state and interaction capabilities
 */
struct ControlMessage {
    'control' type;
    std::variant<'disable_input', 'enable_input'> action;
    std::optional<std::string> target;
    UUID roomId;
};

/**
 * Client instance
 */
    std::future<void> stop();
    std::future<Service> start(IAgentRuntime _runtime);
    std::future<unknown> stop(IAgentRuntime _runtime);


} // namespace elizaos
