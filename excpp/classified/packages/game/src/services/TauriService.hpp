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
#include ".types/shared.hpp"
#include ".types/tauri-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * TauriService - Comprehensive Tauri IPC service
 * Replaces all WebSocket/API client usage with native Tauri IPC
 */

// Type-only imports to avoid runtime issues with optional dependencies
using DialogSaveOptions = {

using DialogOpenOptions = {

using TauriDialogAPI = std::variant<std::function<void()>, null>>;

struct TauriMessage {
    std::string id;
    std::string content;
    std::variant<'user', 'agent', 'system', 'error'> type;
    std::string authorId;
    std::string authorName;
    Date timestamp;
};

struct TauriGoal {
    std::string id;
    std::string text;
    bool completed;
    std::string createdAt;
};

struct TauriTodo {
    std::string id;
    std::string title;
    std::optional<std::string> description;
    bool completed;
    std::optional<std::string> dueDate;
    std::optional<std::variant<'low', 'medium', 'high'>> priority;
    std::string createdAt;
};

struct TauriAgentStatus {
    std::string name;
    std::variant<'online', 'offline', 'thinking'> status;
    std::optional<std::string> lastThought;
    std::optional<std::string> lastAction;
    std::optional<std::string> currentGoal;
};

// Use unified knowledge item type instead of local interface
using TauriKnowledgeFile = KnowledgeItem;

struct StartupStatus {
    std::string phase;
    std::string message;
    std::optional<double> progress;
    std::optional<bool> isComplete;
    std::optional<std::string> error;
};

struct ContainerStatus {
    bool containerRunning;
    bool agentHealthy;
    bool ollamaHealthy;
    std::vector<std::string> logs;
};

struct ContainerLog {
    Date timestamp;
    std::string service;
    std::string message;
    std::optional<std::variant<'info', 'warn', 'error'>> level;
};

struct CapabilityStatus {
    bool enabled;
    std::optional<std::variant<'active', 'inactive', 'error'>> status;
    std::optional<std::string> error;
    std::optional<std::string> lastUsed;
};

struct MemoryQuery {
    std::optional<std::variant<'knowledge', 'conversation', 'goal', 'user', 'relationship'>> type;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<std::variant<'asc', 'desc'>> orderDirection;
};

struct MemoryResult {
    std::string id;
    std::string type;
    std::string content;
    Date createdAt;
    std::optional<double> importance;
};

using OllamaModelStatus = SharedOllamaModelStatus;

struct ModelDownloadProgress {
    std::string model;
    double progress;
    std::variant<'downloading', 'completed', 'failed'> status;
    std::optional<std::string> error;
};

// Define Tauri function types
using TauriInvokeFunction = std::function<std::future<unknown>(std::string, Record<string, auto)>;
using TauriListenFunction = std::function<void()>;

// WebSocket event payload interfaces
struct WebSocketEventData {
    std::string type;
    std::optional<std::string> type;
    std::optional<std::string> id;
    std::optional<std::string> content;
    std::optional<std::string> text;
    std::optional<std::string> userId;
    std::optional<std::string> author;
    std::optional<std::string> name;
    std::optional<std::string> timestamp;
};

struct RealtimeUpdateData {
    std::string type;
    std::optional<TauriMessage> data;
};

struct WebSocketErrorData {
    std::optional<std::string> message;
    std::optional<std::string> error;
    std::optional<std::string> timestamp;
};

class TauriServiceClass {
public:
    TauriServiceClass();
    Promise< tryImportDialog();
    std::future<void> checkAndInitializeTauri();
    std::future<void> initialize();
    std::future<void> setupEventListeners();
    std::future<unknown> ensureInitializedAndInvoke(const std::string& command, std::optional<Record<string> args, auto unknown>);
    bool isRunningInTauri();
     getInitializationStatus();
    std::future<bool> ensureInitialized();
    std::string getUserId();
    std::string getAgentId();
    void destroy();
    std::future<void> connectWebSocket(string = 'ws://localhost:7777' url);
    std::future<void> disconnectWebSocket();
    std::future<void> joinChannel(const std::string& channelId);
    std::future<bool> isWebSocketConnected();
    std::future<std::string> sendMessage(const std::string& content);
    std::future<std::unordered_map<std::string, unknown>> getCapabilities();
    std::future<void> updateCapability(const std::string& capability, bool enabled);
    std::future<std::unordered_map<std::string, unknown>> getSettings();
    std::future<void> updateSettings(const std::optional<std::any>& settings);
    std::future<HealthCheckResponse> checkAgentHealth();
    std::future<std::vector<ContainerStatus>> getContainerStatus();
    std::future<void> restartContainer(const std::string& name);
    std::future<bool> checkServerStatus();
    std::future<void> startServer();
    std::future<void> stopServer();
    std::future<std::vector<TauriGoal>> fetchGoals();
    std::future<std::vector<TauriTodo>> fetchTodos();
    std::future<void> createGoal(const std::string& name, const std::string& description, std::optional<Record<string> metadata, auto unknown>);
    std::future<void> createTodo(const std::string& name, std::optional<std::string> description, std::optional<double> priority, std::optional<std::string> todoType);
    std::future<std::vector<TauriKnowledgeFile>> fetchKnowledgeFiles();
    std::future<void> uploadKnowledgeFile(File file);
    std::future<void> deleteKnowledgeFile(const std::string& fileId);
    std::future<std::vector<TauriMessage>> fetchMemories(number = 50 limit);
    std::future<std::vector<TauriMessage>> fetchMemoriesFromRoom(const std::string& roomId, number = 50 limit);
    std::future<std::vector<std::unordered_map<std::string, unknown>>> fetchPlugins();
    std::future<void> updatePluginConfig(const std::string& pluginId, Record<string config, auto unknown>);
    std::future<void> togglePlugin(const std::string& pluginId, bool enabled);
    std::future<std::unordered_map<std::string, unknown>> fetchPluginConfigs();
    std::future<ValidationResponse> validateConfiguration();
    std::future<TestConfigurationResponse> testConfiguration();
    std::future<void> saveConfiguration(Record<string config, auto unknown>);
    std::future<std::unordered_map<std::string, unknown>> loadConfiguration();
    std::future<std::unordered_map<std::string, unknown>> getAgentConfiguration();
    std::future<std::unordered_map<std::string, unknown>> updateAgentConfiguration(Record<string updates, auto unknown>);
    std::future<std::unordered_map<std::string, unknown>> getAvailableProviders();
    std::future<std::unordered_map<std::string, unknown>> createBackup(const std::string& backupType, std::optional<std::string> notes);
    std::future<void> restoreBackup(const std::string& backupId, Record<string options, auto unknown>);
    std::future<std::vector<std::unordered_map<std::string, unknown>>> listBackups();
    std::future<void> deleteBackup(const std::string& backupId);
    std::future<std::unordered_map<std::string, unknown>> getBackupConfig();
    std::future<void> updateBackupConfig(Record<string config, auto unknown>);
    std::future<std::string> exportBackup(const std::string& backupId);
    std::future<std::unordered_map<std::string, unknown>> importBackup(const std::string& importPath);
    std::variant<Promise<string, null>> selectFile(std::optional<std::any> options);
    std::future<void> restartApplication();
    std::future<void> toggleAutonomy(bool enabled);
    Promise< getAutonomyStatus();
    std::future<std::unordered_map<std::string, unknown>> fetchAutonomyStatus();
    std::future<void> toggleCapability(const std::string& capability);
    Promise< getCapabilityStatus(const std::string& capability);
    std::future<std::unordered_map<std::string, unknown>> getVisionSettings();
    std::future<void> updateVisionSettings(Record<string settings, auto unknown>);
    std::future<void> updateAgentSetting(const std::string& key, unknown value);
    std::future<std::unordered_map<std::string, unknown>> getAgentSettings();
    std::future<void> refreshVisionService();
    std::future<void> resetAgent();
    std::future<std::vector<std::unordered_map<std::string, unknown>>> fetchLogs(std::optional<std::string> logType, std::optional<double> limit);
    Promise< getAgentInfo();
    std::future<std::vector<std::string>> fetchDatabaseTables();
    std::future<std::unordered_map<std::string, unknown>> fetchDatabaseTableData(const std::string& table, number = 1 page, number = 50 limit);
    std::future<std::unordered_map<std::string, unknown>> fetchDatabaseStats();
    std::future<std::unordered_map<std::string, unknown>> proxyApiRequest(const std::string& method, const std::string& path, std::optional<Record<string> body, auto unknown>);
    Array< fetchPluginRoutes();
    void if(auto response.success && response.data);
    std::future<std::string> fetchTabContent(const std::string& route);
    std::future<OllamaModelStatus> checkOllamaModels();
    std::future<void> pullMissingModels();
    std::future<void> shutdownApplication();

private:
    std::string userId_;
};

// Export singleton instance

} // namespace elizaos
