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
 * Shared type definitions for the ElizaOS Game Application
 */

// Import types from core
import type { KnowledgeItem } from '@elizaos/core';

// Common interfaces
struct BaseMessage {
    std::string id;
    std::string content;
    Date | string timestamp;
    std::unordered_map<std::string, unknown> metadata;
};


interface TauriEvent<T = Record<string, unknown>> {
  payload: T;
}

struct TauriWindow {
    std::optional<{> __TAURI_INTERNALS__;
    (command: string, args?: Record<string, unknown>) => Promise<unknown> invoke;
    (event: string, callback: (event: TauriEvent) => void) => Promise<() => void> listen;
};


interface ApiResponse<T = unknown> {
  success: boolean;
  data?: T;
  error?: string;
  message?: string;
}

// WebSocket message types
struct WebSocketMessage {
    std::string type;
    std::optional<std::unordered_map<std::string, unknown>> data;
    std::optional<{> message;
    std::optional<std::string> id;
    std::optional<std::string> content;
    std::optional<std::string> text;
    std::optional<std::string> userId;
    std::optional<std::string> author;
    std::optional<std::string> name;
    std::optional<string | Date> timestamp;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


// Configuration types
struct ConfigurationValue {
    std::string key;
    string | number | boolean value;
    'string' | 'number' | 'boolean' | 'url' | 'api_key' type;
};


struct ValidationResult {
    bool valid;
    std::optional<std::string> error;
    std::optional<string | number | boolean> sanitizedValue;
};


// Agent and Runtime types
struct AgentConfiguration {
    std::string name;
    std::unordered_map<std::string, ConfigurationValue> settings;
    std::vector<std::string> plugins;
    std::unordered_map<std::string, bool> capabilities;
};


struct RuntimeSettings {
};


// Error types
interface AppError extends Error {
  code?: string;
  details?: Record<string, unknown>;
}

// Log entry types
struct LogEntry {
    string | Date timestamp;
    'debug' | 'info' | 'warn' | 'error' level;
    std::string message;
    std::optional<std::string> source;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


// HTTP Request/Response types
struct HttpRequestOptions {
    std::optional<'GET' | 'POST' | 'PUT' | 'DELETE' | 'PATCH'> method;
    std::optional<std::unordered_map<std::string, std::string>> headers;
    std::optional<string | FormData | Record<string, unknown>> body;
    std::optional<double> timeout;
};


interface HttpResponse<T = unknown> {
  ok: boolean;
  status: number;
  statusText: string;
  data?: T;
  error?: string;
}

// Provider and Model types
struct ModelProvider {
    std::string name;
    std::string displayName;
    'available' | 'not_configured' | 'error' status;
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
    'knowledge' | 'conversation' | 'goal' | 'user' | 'relationship' type;
    string | Record<string, unknown> content;
    Date | string createdAt;
    std::optional<Date | string> updatedAt;
    std::optional<double> importance;
    std::optional<std::string> entityId;
    std::optional<std::string> agentId;
    std::optional<std::string> userId;
    std::unordered_map<std::string, unknown> metadata;
};


// Re-unified knowledge types from core
type {
  KnowledgeItem,
  DocumentMetadata,
  IKnowledgeService,
  KnowledgeScope,
  DirectoryItem,
} from '@elizaos/core';

// Goal and Todo types
struct Goal {
    std::string id;
    std::string name;
    std::string description;
    bool isCompleted;
    string | Date createdAt;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


struct Todo {
    std::string id;
    std::string name;
    std::optional<std::string> title;
    std::optional<std::string> description;
    'daily' | 'one-off' | 'aspirational' type;
    bool isCompleted;
    std::optional<double> priority;
    string | Date createdAt;
    std::optional<string | Date> dueDate;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


// Capability and Status types
struct CapabilityStatus {
    bool enabled;
    'active' | 'inactive' | 'error' status;
    std::optional<std::string> error;
    std::optional<string | Date> lastUsed;
    std::unordered_map<std::string, unknown> metadata;
};


struct AgentStatus {
    std::string name;
    'online' | 'offline' | 'thinking' status;
    std::optional<std::string> lastThought;
    std::optional<std::string> lastAction;
    std::optional<std::string> currentGoal;
    std::optional<double> uptime;
    std::unordered_map<std::string, unknown> metadata;
};


// File and Upload types
struct FileUpload {
    std::string name;
    std::string type;
    double size;
    string | ArrayBuffer content;
    std::optional<double> lastModified;
};


// Use unified knowledge item type for knowledge files
using KnowledgeFile = KnowledgeItem;

// Backup types
struct BackupInfo {
    std::string id;
    string | Date timestamp;
    'manual' | 'automatic' | 'shutdown' backup_type;
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
    'running' | 'stopped' | 'error' status;
    std::optional<double> uptime;
    std::vector<LogEntry> logs;
};


struct SystemInfo {
    std::string platform;
    std::string arch;
    std::string version;
    { memory;
    double total;
    double available;
    double used;
};


// Event callback types
type EventCallback<T = Record<string, unknown>> = (data: T) => void;
using UnsubscribeFunction = () => void;

// API Response types for Tauri
interface TauriApiResponse<T = unknown> {
  success: boolean;
  data?: T;
  error?: string;
}

struct HealthCheckResponse {
    'healthy' | 'unhealthy' status;
    bool database;
    std::unordered_map<std::string, bool> services;
};


struct ValidationResponse {
    bool valid;
    std::vector<std::string> errors;
};


struct TestConfigurationResponse {
    bool success;
    std::unordered_map<std::string, unknown> results;
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
    Array<{ routes;
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
    string | null downloading;
    number | null progress;
};


// Additional response types for TauriService
struct TauriMemoryResponse {
    bool success;
    std::optional<{> data;
    std::optional<std::vector<MemoryEntry>> memories;
    std::optional<std::vector<Goal>> goals;
    std::optional<std::vector<Todo>> todos;
    std::optional<std::vector<KnowledgeFile>> files;
    std::optional<std::vector<std::unordered_map<std::string, unknown>>> plugins;
    std::optional<std::vector<LogEntry>> logs;
    std::optional<std::vector<std::string>> tables;
    std::optional<std::vector<{ name: string; path: string; display_name?: string }>> routes;
    std::optional<std::string> content;
    std::optional<std::vector<MemoryEntry>> memories;
};


struct TauriStringResponse {
    bool success;
    std::optional<std::string> data;
};


struct TauriSettingsResponse {
    bool success;
    std::optional<std::unordered_map<std::string, unknown>> data;
};


} // namespace elizaos
