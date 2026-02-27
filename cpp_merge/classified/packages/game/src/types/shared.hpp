#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_GAME_SRC_TYPES_SHARED_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_GAME_SRC_TYPES_SHARED_H
#include "core.hpp"
// External dependency removed

typedef KnowledgeItem KnowledgeFile;

template <typename T>
using EventCallback = std::function<void(T)>;

typedef std::function<void()> UnsubscribeFunction;

class BaseMessage;
template <typename T>
class TauriEvent;
class TauriWindow;
template <typename T>
class ApiResponse;
class WebSocketMessage;
class ConfigurationValue;
class ValidationResult;
class AgentConfiguration;
class RuntimeSettings;
class AppError;
class LogEntry;
class HttpRequestOptions;
template <typename T>
class HttpResponse;
class ModelProvider;
class ModelInfo;
class MemoryEntry;
class Goal;
class Todo;
class CapabilityStatus;
class AgentStatus;
class FileUpload;
class BackupInfo;
class BackupComponent;
class BackupMetadata;
class RestoreOptions;
class ContainerInfo;
class SystemInfo;
template <typename T>
class TauriApiResponse;
class HealthCheckResponse;
class ValidationResponse;
class TestConfigurationResponse;
class AutonomyStatusResponse;
class CapabilityStatusResponse;
class AgentInfoResponse;
class DatabaseStatsResponse;
class PluginRoutesResponse;
class TabContentResponse;
class OllamaModelStatus;
class TauriMemoryResponse;
class TauriStringResponse;
class TauriSettingsResponse;

class BaseMessage : public object, public std::enable_shared_from_this<BaseMessage> {
public:
    using std::enable_shared_from_this<BaseMessage>::shared_from_this;
    string id;

    string content;

    any timestamp;

    Record<string, any> metadata;
};

template <typename T>
class TauriEvent : public object, public std::enable_shared_from_this<TauriEvent<T>> {
public:
    using std::enable_shared_from_this<TauriEvent<T>>::shared_from_this;
    T payload;
};

class TauriWindow : public object, public std::enable_shared_from_this<TauriWindow> {
public:
    using std::enable_shared_from_this<TauriWindow>::shared_from_this;
    object __TAURI_INTERNALS__;
};

template <typename T>
class ApiResponse : public object, public std::enable_shared_from_this<ApiResponse<T>> {
public:
    using std::enable_shared_from_this<ApiResponse<T>>::shared_from_this;
    boolean success;

    T data;

    string error;

    string message;
};

class WebSocketMessage : public object, public std::enable_shared_from_this<WebSocketMessage> {
public:
    using std::enable_shared_from_this<WebSocketMessage>::shared_from_this;
    string type;

    Record<string, any> data;

    object message;
};

class ConfigurationValue : public object, public std::enable_shared_from_this<ConfigurationValue> {
public:
    using std::enable_shared_from_this<ConfigurationValue>::shared_from_this;
    string key;

    any value;

    any type;
};

class ValidationResult : public object, public std::enable_shared_from_this<ValidationResult> {
public:
    using std::enable_shared_from_this<ValidationResult>::shared_from_this;
    boolean valid;

    string error;

    any sanitizedValue;
};

class AgentConfiguration : public object, public std::enable_shared_from_this<AgentConfiguration> {
public:
    using std::enable_shared_from_this<AgentConfiguration>::shared_from_this;
    string name;

    Record<string, std::shared_ptr<ConfigurationValue>> settings;

    array<string> plugins;

    Record<string, boolean> capabilities;
};

class RuntimeSettings : public object, public std::enable_shared_from_this<RuntimeSettings> {
public:
    using std::enable_shared_from_this<RuntimeSettings>::shared_from_this;
};

class AppError : public Error, public std::enable_shared_from_this<AppError> {
public:
    using std::enable_shared_from_this<AppError>::shared_from_this;
    string code;

    Record<string, any> details;
};

class LogEntry : public object, public std::enable_shared_from_this<LogEntry> {
public:
    using std::enable_shared_from_this<LogEntry>::shared_from_this;
    any timestamp;

    any level;

    string message;

    string source;

    Record<string, any> metadata;
};

class HttpRequestOptions : public object, public std::enable_shared_from_this<HttpRequestOptions> {
public:
    using std::enable_shared_from_this<HttpRequestOptions>::shared_from_this;
    any method;

    Record<string, string> headers;

    any body;

    double timeout;
};

template <typename T>
class HttpResponse : public object, public std::enable_shared_from_this<HttpResponse<T>> {
public:
    using std::enable_shared_from_this<HttpResponse<T>>::shared_from_this;
    boolean ok;

    double status;

    string statusText;

    T data;

    string error;
};

class ModelProvider : public object, public std::enable_shared_from_this<ModelProvider> {
public:
    using std::enable_shared_from_this<ModelProvider>::shared_from_this;
    string name;

    string displayName;

    any status;

    string message;

    array<string> models;
};

class ModelInfo : public object, public std::enable_shared_from_this<ModelInfo> {
public:
    using std::enable_shared_from_this<ModelInfo>::shared_from_this;
    string name;

    string description;

    string provider;

    array<string> capabilities;
};

class MemoryEntry : public object, public std::enable_shared_from_this<MemoryEntry> {
public:
    using std::enable_shared_from_this<MemoryEntry>::shared_from_this;
    string id;

    any type;

    any content;

    any createdAt;

    any updatedAt;

    double importance;

    string entityId;

    string agentId;

    string userId;

    Record<string, any> metadata;
};

class Goal : public object, public std::enable_shared_from_this<Goal> {
public:
    using std::enable_shared_from_this<Goal>::shared_from_this;
    string id;

    string name;

    string description;

    boolean isCompleted;

    any createdAt;

    Record<string, any> metadata;
};

class Todo : public object, public std::enable_shared_from_this<Todo> {
public:
    using std::enable_shared_from_this<Todo>::shared_from_this;
    string id;

    string name;

    string title;

    string description;

    any type;

    boolean isCompleted;

    double priority;

    any createdAt;

    any dueDate;

    Record<string, any> metadata;
};

class CapabilityStatus : public object, public std::enable_shared_from_this<CapabilityStatus> {
public:
    using std::enable_shared_from_this<CapabilityStatus>::shared_from_this;
    boolean enabled;

    any status;

    string error;

    any lastUsed;

    Record<string, any> metadata;
};

class AgentStatus : public object, public std::enable_shared_from_this<AgentStatus> {
public:
    using std::enable_shared_from_this<AgentStatus>::shared_from_this;
    string name;

    any status;

    string lastThought;

    string lastAction;

    string currentGoal;

    double uptime;

    Record<string, any> metadata;
};

class FileUpload : public object, public std::enable_shared_from_this<FileUpload> {
public:
    using std::enable_shared_from_this<FileUpload>::shared_from_this;
    string name;

    string type;

    double size;

    any content;

    double lastModified;
};

class BackupInfo : public object, public std::enable_shared_from_this<BackupInfo> {
public:
    using std::enable_shared_from_this<BackupInfo>::shared_from_this;
    string id;

    any timestamp;

    any backup_type;

    double size_bytes;

    array<std::shared_ptr<BackupComponent>> components;

    std::shared_ptr<BackupMetadata> metadata;
};

class BackupComponent : public object, public std::enable_shared_from_this<BackupComponent> {
public:
    using std::enable_shared_from_this<BackupComponent>::shared_from_this;
    string name;

    string component_type;

    double size_bytes;
};

class BackupMetadata : public object, public std::enable_shared_from_this<BackupMetadata> {
public:
    using std::enable_shared_from_this<BackupMetadata>::shared_from_this;
    string agent_name;

    string eliza_version;

    string notes;
};

class RestoreOptions : public object, public std::enable_shared_from_this<RestoreOptions> {
public:
    using std::enable_shared_from_this<RestoreOptions>::shared_from_this;
    boolean restore_database;

    boolean restore_agent_state;

    boolean restore_knowledge;

    boolean restore_logs;

    boolean force;
};

class ContainerInfo : public object, public std::enable_shared_from_this<ContainerInfo> {
public:
    using std::enable_shared_from_this<ContainerInfo>::shared_from_this;
    string name;

    any status;

    double uptime;

    array<std::shared_ptr<LogEntry>> logs;
};

class SystemInfo : public object, public std::enable_shared_from_this<SystemInfo> {
public:
    using std::enable_shared_from_this<SystemInfo>::shared_from_this;
    string platform;

    string arch;

    string version;

    object memory;
};

template <typename T>
class TauriApiResponse : public object, public std::enable_shared_from_this<TauriApiResponse<T>> {
public:
    using std::enable_shared_from_this<TauriApiResponse<T>>::shared_from_this;
    boolean success;

    T data;

    string error;
};

class HealthCheckResponse : public object, public std::enable_shared_from_this<HealthCheckResponse> {
public:
    using std::enable_shared_from_this<HealthCheckResponse>::shared_from_this;
    any status;

    boolean database;

    Record<string, boolean> services;
};

class ValidationResponse : public object, public std::enable_shared_from_this<ValidationResponse> {
public:
    using std::enable_shared_from_this<ValidationResponse>::shared_from_this;
    boolean valid;

    array<string> errors;
};

class TestConfigurationResponse : public object, public std::enable_shared_from_this<TestConfigurationResponse> {
public:
    using std::enable_shared_from_this<TestConfigurationResponse>::shared_from_this;
    boolean success;

    Record<string, any> results;
};

class AutonomyStatusResponse : public object, public std::enable_shared_from_this<AutonomyStatusResponse> {
public:
    using std::enable_shared_from_this<AutonomyStatusResponse>::shared_from_this;
    boolean enabled;

    boolean running;

    double interval;
};

class CapabilityStatusResponse : public object, public std::enable_shared_from_this<CapabilityStatusResponse> {
public:
    using std::enable_shared_from_this<CapabilityStatusResponse>::shared_from_this;
    boolean enabled;

    boolean service_available;
};

class AgentInfoResponse : public object, public std::enable_shared_from_this<AgentInfoResponse> {
public:
    using std::enable_shared_from_this<AgentInfoResponse>::shared_from_this;
    string id;

    string name;

    string version;
};

class DatabaseStatsResponse : public object, public std::enable_shared_from_this<DatabaseStatsResponse> {
public:
    using std::enable_shared_from_this<DatabaseStatsResponse>::shared_from_this;
    double connections;

    double queries;

    double uptime;
};

class PluginRoutesResponse : public object, public std::enable_shared_from_this<PluginRoutesResponse> {
public:
    using std::enable_shared_from_this<PluginRoutesResponse>::shared_from_this;
    array<object> routes;
};

class TabContentResponse : public object, public std::enable_shared_from_this<TabContentResponse> {
public:
    using std::enable_shared_from_this<TabContentResponse>::shared_from_this;
    string content;
};

class OllamaModelStatus : public object, public std::enable_shared_from_this<OllamaModelStatus> {
public:
    using std::enable_shared_from_this<OllamaModelStatus>::shared_from_this;
    boolean models_ready;

    array<string> missing_models;

    any downloading;

    any progress;
};

class TauriMemoryResponse : public object, public std::enable_shared_from_this<TauriMemoryResponse> {
public:
    using std::enable_shared_from_this<TauriMemoryResponse>::shared_from_this;
    boolean success;

    object data;

    array<std::shared_ptr<MemoryEntry>> memories;
};

class TauriStringResponse : public object, public std::enable_shared_from_this<TauriStringResponse> {
public:
    using std::enable_shared_from_this<TauriStringResponse>::shared_from_this;
    boolean success;

    string data;
};

class TauriSettingsResponse : public object, public std::enable_shared_from_this<TauriSettingsResponse> {
public:
    using std::enable_shared_from_this<TauriSettingsResponse>::shared_from_this;
    boolean success;

    Record<string, any> data;
};

#endif
