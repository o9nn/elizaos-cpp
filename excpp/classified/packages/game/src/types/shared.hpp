#pragma once
#include <functional>
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
 * Shared type definitions for the ElizaOS Game Application
 */

// Import types from core

// Common interfaces
struct BaseMessage {
    std::string id;
    std::string content;
    std::variant<Date, std::string> timestamp;
};

struct TauriWindow {
};

// WebSocket message types
struct WebSocketMessage {
    std::string type;
    std::optional<std::string> id;
    std::optional<std::string> content;
    std::optional<std::string> text;
    std::optional<std::string> userId;
    std::optional<std::string> author;
    std::optional<std::string> name;
    std::optional<std::variant<std::string, Date>> timestamp;
};

// Configuration types
struct ConfigurationValue {
    std::string key;
    std::variant<std::string, double, bool> value;
    std::variant<'string', 'number', 'boolean', 'url', 'api_key'> type;
};

struct ValidationResult {
    bool valid;
    std::optional<std::string> error;
    std::optional<std::variant<std::string, double, bool>> sanitizedValue;
};

// Agent and Runtime types
struct AgentConfiguration {
    std::string name;
    std::vector<std::string> plugins;
};

struct RuntimeSettings {
};

// Error types

// Log entry types
struct LogEntry {
    std::variant<std::string, Date> timestamp;
    std::variant<'debug', 'info', 'warn', 'error'> level;
    std::string message;
    std::optional<std::string> source;
};

// HTTP Request/Response types
struct HttpRequestOptions {
    std::optional<std::variant<'GET', 'POST', 'PUT', 'DELETE', 'PATCH'>> method;
    std::optional<double> timeout;
};

// Provider and Model types
struct ModelProvider {
    std::string name;
    std::string displayName;
    std::variant<'available', 'not_configured', 'error'> status;
    std::string message;
    std::optional<std::vector<std::string>> models;
};

struct ModelInfo {
    std::string name;
    std::string description;
    std::string provider;
    std::vector<std::string> capabilities;
};

// Memory and Knowledge types - Extended from @elizaos/core
struct MemoryEntry {
    std::string id;
    std::variant<'knowledge', 'conversation', 'goal', 'user', 'relationship'> type;
    std::variant<Date, std::string> createdAt;
    std::optional<std::variant<Date, std::string>> updatedAt;
    std::optional<double> importance;
    std::optional<std::string> entityId;
    std::optional<std::string> agentId;
    std::optional<std::string> userId;
};

// Re-unified knowledge types from core

// Goal and Todo types
struct Goal {
    std::string id;
    std::string name;
    std::string description;
    bool isCompleted;
    std::variant<std::string, Date> createdAt;
};

struct Todo {
    std::string id;
    std::string name;
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::variant<'daily', 'one-off', 'aspirational'> type;
    bool isCompleted;
    std::optional<double> priority;
    std::variant<std::string, Date> createdAt;
    std::optional<std::variant<std::string, Date>> dueDate;
};

// Capability and Status types
struct CapabilityStatus {
    bool enabled;
    std::variant<'active', 'inactive', 'error'> status;
    std::optional<std::string> error;
    std::optional<std::variant<std::string, Date>> lastUsed;
};

struct AgentStatus {
    std::string name;
    std::variant<'online', 'offline', 'thinking'> status;
    std::optional<std::string> lastThought;
    std::optional<std::string> lastAction;
    std::optional<std::string> currentGoal;
    std::optional<double> uptime;
};

// File and Upload types
struct FileUpload {
    std::string name;
    std::string type;
    double size;
    std::variant<std::string, ArrayBuffer> content;
    std::optional<double> lastModified;
};

// Use unified knowledge item type for knowledge files
using KnowledgeFile = KnowledgeItem;

// Backup types
struct BackupInfo {
    std::string id;
    std::variant<std::string, Date> timestamp;
    std::variant<'manual', 'automatic', 'shutdown'> backup_type;
    double size_bytes;
    std::vector<BackupComponent> components;
    BackupMetadata metadata;
};

struct BackupComponent {
    std::string name;
    std::string component_type;
    double size_bytes;
};

struct BackupMetadata {
    std::string agent_name;
    std::string eliza_version;
    std::optional<std::string> notes;
};

struct RestoreOptions {
    bool restore_database;
    bool restore_agent_state;
    bool restore_knowledge;
    bool restore_logs;
    bool force;
};

// Container and System types
struct ContainerInfo {
    std::string name;
    std::variant<'running', 'stopped', 'error'> status;
    std::optional<double> uptime;
    std::vector<LogEntry> logs;
};

struct SystemInfo {
    std::string platform;
    std::string arch;
    std::string version;
    double total;
    double available;
    double used;
};

// Event callback types
using UnsubscribeFunction = std::function<void()>;

// API Response types for Tauri

struct HealthCheckResponse {
    std::variant<'healthy', 'unhealthy'> status;
    bool database;
};

struct ValidationResponse {
    bool valid;
    std::vector<std::string> errors;
};

struct TestConfigurationResponse {
    bool success;
};

struct AutonomyStatusResponse {
    bool enabled;
    bool running;
    double interval;
};

struct CapabilityStatusResponse {
    bool enabled;
    bool service_available;
};

struct AgentInfoResponse {
    std::string id;
    std::string name;
    std::string version;
};

struct DatabaseStatsResponse {
    double connections;
    double queries;
    double uptime;
};

struct PluginRoutesResponse {
    std::string name;
    std::string path;
    std::optional<std::string> display_name;
};

struct TabContentResponse {
    std::string content;
};

struct OllamaModelStatus {
    bool models_ready;
    std::vector<std::string> missing_models;
    std::optional<std::string> downloading;
    std::optional<double> progress;
};

// Additional response types for TauriService
struct TauriMemoryResponse {
    bool success;
    std::optional<std::vector<MemoryEntry>> memories;
    std::optional<std::vector<Goal>> goals;
    std::optional<std::vector<Todo>> todos;
    std::optional<std::vector<KnowledgeFile>> files;
    std::optional<std::vector<LogEntry>> logs;
    std::optional<std::vector<std::string>> tables;
    std::string path;
    std::optional<std::string> content;
    std::optional<std::vector<MemoryEntry>> memories;
};

struct TauriStringResponse {
    bool success;
    std::optional<std::string> data;
};

struct TauriSettingsResponse {
    bool success;
};


} // namespace elizaos
