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

import type { Readable } from 'stream';

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
  DONE = 'DONE',
  FAILED = 'FAILED',
  IN_PROGRESS = 'IN_PROGRESS',
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
  SMALL = 'small',
  MEDIUM = 'medium',
  LARGE = 'large',
  EMBEDDING = 'embedding',
  IMAGE = 'image',
}

/**
 * Model settings
 */
using ModelSettings = {
  /** Model name */
  name: string;

  /** Maximum input tokens */
  maxInputTokens: number;

  /** Maximum output tokens */
  maxOutputTokens: number;

  /** Optional frequency penalty */
  frequency_penalty?: number;

  /** Optional presence penalty */
  presence_penalty?: number;

  /** Optional repetition penalty */
  repetition_penalty?: number;

  /** Stop sequences */
  stop: string[];

  /** Temperature setting */
  temperature: number;

  /** Optional telemetry configuration (experimental) */
  experimental_telemetry?: TelemetrySettings;
};

/** Image model settings */
using ImageModelSettings = {
  name: string;
  steps?: number;
};

/** Embedding model settings */
using EmbeddingModelSettings = {
  name: string;
  dimensions?: number;
};

/**
 * Configuration for an AI model
 */
using Model = {
  /** Optional API endpoint */
  endpoint?: string;

  /** Model names by size class */
  model: {
    [ModelClass.SMALL]?: ModelSettings;
    [ModelClass.MEDIUM]?: ModelSettings;
    [ModelClass.LARGE]?: ModelSettings;
    [ModelClass.EMBEDDING]?: EmbeddingModelSettings;
    [ModelClass.IMAGE]?: ImageModelSettings;
  };
};

/**
 * Model configurations by provider
 */
using Models = {
  [ModelProviderName.OPENAI]: Model;
  [ModelProviderName.ETERNALAI]: Model;
  [ModelProviderName.ANTHROPIC]: Model;
  [ModelProviderName.GROK]: Model;
  [ModelProviderName.GROQ]: Model;
  [ModelProviderName.LLAMACLOUD]: Model;
  [ModelProviderName.TOGETHER]: Model;
  [ModelProviderName.LLAMALOCAL]: Model;
  [ModelProviderName.LMSTUDIO]: Model;
  [ModelProviderName.GOOGLE]: Model;
  [ModelProviderName.MISTRAL]: Model;
  [ModelProviderName.CLAUDE_VERTEX]: Model;
  [ModelProviderName.REDPILL]: Model;
  [ModelProviderName.OPENROUTER]: Model;
  [ModelProviderName.OLLAMA]: Model;
  [ModelProviderName.HEURIST]: Model;
  [ModelProviderName.GALADRIEL]: Model;
  [ModelProviderName.FAL]: Model;
  [ModelProviderName.GAIANET]: Model;
  [ModelProviderName.ALI_BAILIAN]: Model;
  [ModelProviderName.VOLENGINE]: Model;
  [ModelProviderName.NANOGPT]: Model;
  [ModelProviderName.HYPERBOLIC]: Model;
  [ModelProviderName.VENICE]: Model;
  [ModelProviderName.NVIDIA]: Model;
  [ModelProviderName.NINETEEN_AI]: Model;
  [ModelProviderName.AKASH_CHAT_API]: Model;
  [ModelProviderName.LIVEPEER]: Model;
  [ModelProviderName.DEEPSEEK]: Model;
  [ModelProviderName.INFERA]: Model;
  [ModelProviderName.BEDROCK]: Model;
  [ModelProviderName.ATOMA]: Model;
  [ModelProviderName.SECRETAI]: Model;
  [ModelProviderName.NEARAI]: Model;
};

/**
 * Available model providers
 */
enum ModelProviderName {
  OPENAI = 'openai',
  ETERNALAI = 'eternalai',
  ANTHROPIC = 'anthropic',
  GROK = 'grok',
  GROQ = 'groq',
  LLAMACLOUD = 'llama_cloud',
  TOGETHER = 'together',
  LLAMALOCAL = 'llama_local',
  LMSTUDIO = 'lmstudio',
  GOOGLE = 'google',
  MISTRAL = 'mistral',
  CLAUDE_VERTEX = 'claude_vertex',
  REDPILL = 'redpill',
  OPENROUTER = 'openrouter',
  OLLAMA = 'ollama',
  HEURIST = 'heurist',
  GALADRIEL = 'galadriel',
  FAL = 'falai',
  GAIANET = 'gaianet',
  ALI_BAILIAN = 'ali_bailian',
  VOLENGINE = 'volengine',
  NANOGPT = 'nanogpt',
  HYPERBOLIC = 'hyperbolic',
  VENICE = 'venice',
  NVIDIA = 'nvidia',
  NINETEEN_AI = 'nineteen_ai',
  AKASH_CHAT_API = 'akash_chat_api',
  LIVEPEER = 'livepeer',
  LETZAI = 'letzai',
  DEEPSEEK = 'deepseek',
  INFERA = 'infera',
  BEDROCK = 'bedrock',
  ATOMA = 'atoma',
  SECRETAI = 'secret_ai',
  NEARAI = 'nearai',
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
 * Handler ,
  callback?: HandlerCallback
) => Promise<unknown>;

/**
 * Callback ;


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
    std::optional<std::string> email;
    std::optional<std::string> avatarUrl;
};


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
  id: string;

  /** Media URL */
  url: string;

  /** Media title */
  title: string;

  /** Media source */
  source: string;

  /** Media description */
  description: string;

  /** Text content */
  text: string;

  /** Content type */
  contentType?: string;
};

/**
 * Client instance
 */
using ClientInstance = {
  /** Client name */
  // name: string;

  /** Stop client connection */
  stop: (runtime: IAgentRuntime) => Promise<unknown>;
};

/**
 * Client interface for platform connections
 */
using Client = {
  /** Client name */
  name: string;

  /** Client configuration */
  config?: { [key: string]: any };

  /** Start client connection */
  start: (runtime: IAgentRuntime) => Promise<ClientInstance>;
};

/**
 * Database adapter initialization
 */
using Adapter = {
  /** Initialize the adapter */
  init: (runtime: IAgentRuntime) => IDatabaseAdapter & IDatabaseCacheAdapter;
};

/**
 * Plugin for extending agent functionality
 */
using Plugin = {
  /** Plugin name */
  name: string;

  /** Plugin npm name */
  npmName?: string;

  /** Plugin configuration */
  config?: { [key: string]: any };

  /** Plugin description */
  description: string;

  /** Optional actions */
  actions?: Action[];

  /** Optional providers */
  providers?: Provider[];

  /** Optional evaluators */
  evaluators?: Evaluator[];

  /** Optional services */
  services?: Service[];

  /** Optional clients */
  clients?: Client[];

  /** Optional adapters */
  adapters?: Adapter[];

  /** Optional post charactor processor handler */
  handlePostCharacterLoaded?: (char: Character) => Promise<Character>;
};

struct IAgentConfig {
};


using TelemetrySettings = {
  /**
   * Enable or disable telemetry. Disabled by default while experimental.
   */
  isEnabled?: boolean;
  /**
   * Enable or disable input recording. Enabled by default.
   *
   * You might want to disable input recording to avoid recording sensitive
   * information, to reduce data transfers, or to increase performance.
   */
  recordInputs?: boolean;
  /**
   * Enable or disable output recording. Enabled by default.
   *
   * You might want to disable output recording to avoid recording sensitive
   * information, to reduce data transfers, or to increase performance.
   */
  recordOutputs?: boolean;
  /**
   * Identifier for this function. Used to group telemetry data by function.
   */
  functionId?: string;
};

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
  id?: UUID;

  /** Character name */
  name: string;

  /** Optional username */
  username?: string;

  /** Optional email */
  email?: string;

  /** Optional system prompt */
  system?: string;

  /** Model provider to use */
  modelProvider: ModelProviderName;

  /** Image model provider to use, if different from modelProvider */
  imageModelProvider?: ModelProviderName;

  /** Image Vision model provider to use, if different from modelProvider */
  imageVisionModelProvider?: ModelProviderName;

  /** Optional model endpoint override */
  modelEndpointOverride?: string;

  /** Optional prompt templates */
  templates?: {
    goalsTemplate?: TemplateType;
    factsTemplate?: TemplateType;
    messageHandlerTemplate?: TemplateType;
    shouldRespondTemplate?: TemplateType;
    continueMessageHandlerTemplate?: TemplateType;
    evaluationTemplate?: TemplateType;
    twitterSearchTemplate?: TemplateType;
    twitterActionTemplate?: TemplateType;
    twitterPostTemplate?: TemplateType;
    twitterMessageHandlerTemplate?: TemplateType;
    twitterShouldRespondTemplate?: TemplateType;
    twitterVoiceHandlerTemplate?: TemplateType;
    instagramPostTemplate?: TemplateType;
    instagramMessageHandlerTemplate?: TemplateType;
    instagramShouldRespondTemplate?: TemplateType;
    farcasterPostTemplate?: TemplateType;
    lensPostTemplate?: TemplateType;
    farcasterMessageHandlerTemplate?: TemplateType;
    lensMessageHandlerTemplate?: TemplateType;
    farcasterShouldRespondTemplate?: TemplateType;
    lensShouldRespondTemplate?: TemplateType;
    telegramMessageHandlerTemplate?: TemplateType;
    telegramShouldRespondTemplate?: TemplateType;
    telegramAutoPostTemplate?: string;
    telegramPinnedMessageTemplate?: string;
    discordAutoPostTemplate?: string;
    discordAnnouncementHypeTemplate?: string;
    discordVoiceHandlerTemplate?: TemplateType;
    discordShouldRespondTemplate?: TemplateType;
    discordMessageHandlerTemplate?: TemplateType;
    slackMessageHandlerTemplate?: TemplateType;
    slackShouldRespondTemplate?: TemplateType;
    jeeterPostTemplate?: string;
    jeeterSearchTemplate?: string;
    jeeterInteractionTemplate?: string;
    jeeterMessageHandlerTemplate?: string;
    jeeterShouldRespondTemplate?: string;
    devaPostTemplate?: string;
  };

  /** Character biography */
  bio: string | string[];

  /** Character background lore */
  lore: string[];

  /** Example messages */
  messageExamples: MessageExample[][];

  /** Example posts */
  postExamples: string[];

  /** Known topics */
  topics: string[];

  /** Character traits */
  adjectives: string[];

  /** Optional knowledge base */
  knowledge?: (
    | string
    | { path: string; shared?: boolean }
    | { directory: string; shared?: boolean }
  )[];

  /** Available plugins */
  plugins: Plugin[];

  /** Character Processor Plugins */
  postProcessors?: Pick<Plugin, 'name' | 'description' | 'handlePostCharacterLoaded'>[];

  /** Optional configuration */
  settings?: {
    secrets?: { [key: string]: string };
    intiface?: boolean;
    imageSettings?: {
      steps?: number;
      width?: number;
      height?: number;
      cfgScale?: number;
      negativePrompt?: string;
      numIterations?: number;
      guidanceScale?: number;
      seed?: number;
      modelId?: string;
      jobId?: string;
      count?: number;
      stylePreset?: string;
      hideWatermark?: boolean;
      safeMode?: boolean;
    };
    voice?: {
      model?: string; // For VITS
      url?: string; // Legacy VITS support
      elevenlabs?: {
        // New structured ElevenLabs config
        voiceId: string;
        model?: string;
        stability?: string;
        similarityBoost?: string;
        style?: string;
        useSpeakerBoost?: string;
      };
    };
    model?: string;
    modelConfig?: ModelConfiguration;
    embeddingModel?: string;
    chains?: {
      evm?: any[];
      solana?: any[];
      [key: string]: any[] | string | undefined;
    };
    transcription?: TranscriptionProvider;
    ragKnowledge?: boolean;
  };

  /** Optional client-specific config */
  clientConfig?: {
    discord?: {
      shouldIgnoreBotMessages?: boolean;
      shouldIgnoreDirectMessages?: boolean;
      shouldRespondOnlyToMentions?: boolean;
      messageSimilarityThreshold?: number;
      isPartOfTeam?: boolean;
      teamAgentIds?: string[];
      teamLeaderId?: string;
      teamMemberInterestKeywords?: string[];
      allowedChannelIds?: string[];
      autoPost?: {
        enabled?: boolean;
        monitorTime?: number;
        inactivityThreshold?: number;
        mainChannelId?: string;
        announcementChannelIds?: string[];
        minTimeBetweenPosts?: number;
      };
    };
    telegram?: {
      shouldIgnoreBotMessages?: boolean;
      shouldIgnoreDirectMessages?: boolean;
      shouldRespondOnlyToMentions?: boolean;
      shouldOnlyJoinInAllowedGroups?: boolean;
      allowedGroupIds?: string[];
      messageSimilarityThreshold?: number;
      isPartOfTeam?: boolean;
      teamAgentIds?: string[];
      teamLeaderId?: string;
      teamMemberInterestKeywords?: string[];
      autoPost?: {
        enabled?: boolean;
        monitorTime?: number;
        inactivityThreshold?: number;
        mainChannelId?: string;
        pinnedMessagesGroups?: string[];
        minTimeBetweenPosts?: number;
      };
    };
    slack?: {
      shouldIgnoreBotMessages?: boolean;
      shouldIgnoreDirectMessages?: boolean;
    };
    gitbook?: {
      keywords?: {
        projectTerms?: string[];
        generalQueries?: string[];
      };
      documentTriggers?: string[];
    };
  };

  /** Writing style guides */
  style: {
    all: string[];
    chat: string[];
    post: string[];
  };

  /** Optional Twitter profile */
  twitterProfile?: {
    id: string;
    username: string;
    screenName: string;
    bio: string;
    nicknames?: string[];
  };

  /** Optional Instagram profile */
  instagramProfile?: {
    id: string;
    username: string;
    bio: string;
    nicknames?: string[];
  };

  /** Optional SimsAI profile */
  simsaiProfile?: {
    id: string;
    username: string;
    screenName: string;
    bio: string;
  };

  /** Optional NFT prompt */
  nft?: {
    prompt: string;
  };

  /**Optinal Parent characters to inherit information from */
  extends?: string[];

  twitterSpaces?: TwitterSpaceDecisionOptions;
};

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
    UUID userId;
    UUID roomId;
    std::string type;
    std::string tableName;
    UUID agentId;
    UUID roomId;
    std::vector<double> embedding;
    double match_threshold;
    double match_count;
    bool unique;
    std::vector<double> embedding;
    { params;
    std::optional<double> match_threshold;
    std::optional<double> count;
    std::optional<UUID> roomId;
    std::optional<UUID> agentId;
    std::optional<bool> unique;
    std::string tableName;
    UUID agentId;
    UUID roomId;
    std::optional<UUID | null> userId;
    std::optional<bool> onlyInProgress;
    std::optional<double> count;
    UUID roomId;
    UUID userId;
    'FOLLOWED' | 'MUTED' | null state;
    std::optional<UUID> id;
    UUID agentId;
    std::optional<double> limit;
    std::optional<std::string> query;
    std::optional<std::string> conversationContext;
    UUID agentId;
    Float32Array embedding;
    double match_threshold;
    double match_count;
    std::optional<std::string> searchText;
};


struct IDatabaseCacheAdapter {
};


struct IMemoryManager {
    IAgentRuntime runtime;
    std::string tableName;
    std::;


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
    'pdf' | 'md' | 'txt' type;
    bool isShared;
};


using CacheOptions = {
  expires?: number;
};

enum CacheStore {
  REDIS = 'redis',
  DATABASE = 'database',
  FILESYSTEM = 'filesystem',
}

struct ICacheManager {
};


abstract class Service {
  private static instance: Service | null = null;

  static get serviceType(): ServiceType {
    throw new Error('Service must implement static serviceType getter');
  }

  public static getInstance<T extends Service>(): T {
    if (!Service.instance) {
      Service.instance = new (this as any)();
    }
    return Service.instance as T;
  }

  get serviceType(): ServiceType {
    return (this.constructor as typeof Service).serviceType;
  }

  // Add abstract initialize method that must be implemented by derived classes
  abstract initialize(runtime: IAgentRuntime): Promise<void>;
}

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
};


interface IImageDescriptionService extends Service {
  describeImage(imageUrl: string): Promise<{ title: string; description: string }>;
}

interface ITranscriptionService extends Service {
  transcribeAttachment(audioBuffer: ArrayBuffer): Promise<string | null>;
  transcribeAttachmentLocally(audioBuffer: ArrayBuffer): Promise<string | null>;
  transcribe(audioBuffer: ArrayBuffer): Promise<string | null>;
  transcribeLocally(audioBuffer: ArrayBuffer): Promise<string | null>;
}

interface IVideoService extends Service {
  isVideoUrl(url: string): boolean;
  fetchVideoInfo(url: string): Promise<Media>;
  downloadVideo(videoInfo: Media): Promise<string>;
  processVideo(url: string, runtime: IAgentRuntime): Promise<Media>;
}

interface ITextGenerationService extends Service {
  initializeModel(): Promise<void>;
  queueMessageCompletion(
    context: string,
    temperature: number,
    stop: string[],
    frequency_penalty: number,
    presence_penalty: number,
    max_tokens: number
  ): Promise<any>;
  queueTextCompletion(
    context: string,
    temperature: number,
    stop: string[],
    frequency_penalty: number,
    presence_penalty: number,
    max_tokens: number
  ): Promise<string>;
  getEmbeddingResponse(input: string): Promise<number[] | undefined>;
}

interface IBrowserService extends Service {
  closeBrowser(): Promise<void>;
  getPageContent(
    url: string,
    runtime: IAgentRuntime
  ): Promise<{ title: string; description: string; bodyContent: string }>;
}

interface ISpeechService extends Service {
  getInstance(): ISpeechService;
  generate(runtime: IAgentRuntime, text: string): Promise<Readable>;
}

interface IPdfService extends Service {
  getInstance(): IPdfService;
  convertPdfToText(pdfBuffer: Buffer): Promise<string>;
}

interface IAwsS3Service extends Service {
  uploadFile(
    imagePath: string,
    subDirectory: string,
    useSignedUrl: boolean,
    expiresIn: number
  ): Promise<{
    success: boolean;
    url?: string;
    error?: string;
  }>;
  generateSignedUrl(fileName: string, expiresIn: number): Promise<string>;
}

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
  REQUEST = 'REQUEST',
  DATA_STORAGE = 'DATA_STORAGE',
  REQUEST_RESPONSE = 'REQUEST_RESPONSE',
}

enum IrysDataType {
  FILE = 'FILE',
  IMAGE = 'IMAGE',
  OTHER = 'OTHER',
}

struct IrysTimestamp {
    double from;
    double to;
};


interface IIrysService extends Service {
  getDataFromAnAgent(
    agentsWalletPublicKeys: string[],
    tags: GraphQLTag[],
    timestamp: IrysTimestamp
  ): Promise<DataIrysFetchedFromGQL>;
  workerUploadDataOnIrys(
    data: any,
    dataType: IrysDataType,
    messageType: IrysMessageType,
    serviceCategory: string[],
    protocol: string[],
    validationThreshold: number[],
    minimumProviders: number[],
    testProvider: boolean[],
    reputation: number[]
  ): Promise<UploadIrysResult>;
  providerUploadDataOnIrys(
    data: any,
    dataType: IrysDataType,
    serviceCategory: string[],
    protocol: string[]
  ): Promise<UploadIrysResult>;
}

interface ITeeLogService extends Service {
  getInstance(): ITeeLogService;
  log(
    agentId: string,
    roomId: string,
    userId: string,
    type: string,
    content: string
  ): Promise<boolean>;
}

enum ServiceType {
  IMAGE_DESCRIPTION = 'image_description',
  TRANSCRIPTION = 'transcription',
  VIDEO = 'video',
  TEXT_GENERATION = 'text_generation',
  BROWSER = 'browser',
  SPEECH_GENERATION = 'speech_generation',
  PDF = 'pdf',
  INTIFACE = 'intiface',
  AWS_S3 = 'aws_s3',
  BUTTPLUG = 'buttplug',
  SLACK = 'slack',
  VERIFIABLE_LOGGING = 'verifiable_logging',
  IRYS = 'irys',
  TEE_LOG = 'tee_log',
  GOPLUS_SECURITY = 'goplus_security',
  WEB_SEARCH = 'web_search',
  EMAIL_AUTOMATION = 'email_automation',
  NKN_CLIENT_SERVICE = 'nkn_client_service',
  DATABASE_MIGRATION = 'database_migration',
}

enum LoggingLevel {
  DEBUG = 'debug',
  VERBOSE = 'verbose',
  NONE = 'none',
}

using KnowledgeItem = {
  id: UUID;
  content: Content;
};

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


interface ISlackService extends Service {
  client: any;
}

enum TokenizerType {
  Auto = 'auto',
  TikToken = 'tiktoken',
}

enum TranscriptionProvider {
  OpenAI = 'openai',
  Deepgram = 'deepgram',
  Local = 'local',
}

enum ActionTimelineType {
  ForYou = 'foryou',
  Following = 'following',
}
enum KnowledgeScope {
  SHARED = 'shared',
  PRIVATE = 'private',
}

enum CacheKeyPrefix {
  KNOWLEDGE = 'knowledge',
}

struct DirectoryItem {
    std::string directory;
    std::optional<bool> shared;
};


struct ChunkRow {
    std::string id;
};


} // namespace elizaos
