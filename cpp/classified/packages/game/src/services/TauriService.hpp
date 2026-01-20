#include ".types/shared.hpp"
#include ".types/tauri-utils.hpp"
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
 * TauriService - Comprehensive Tauri IPC service
 * Replaces all WebSocket/API client usage with native Tauri IPC
 */

// Type-only imports to avoid runtime issues with optional dependencies
using DialogSaveOptions = {

using DialogOpenOptions = {

using TauriDialogAPI = std::variant<{

struct TauriMessage {
    std::string id;
    std::string content;
    'user' | 'agent' | 'system' | 'error' type;
    std::string authorId;
    std::string authorName;
    Date timestamp;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
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
    std::optional<'low' | 'medium' | 'high'> priority;
    std::string createdAt;
};

struct TauriAgentStatus {
    std::string name;
    'online' | 'offline' | 'thinking' status;
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
    std::optional<'info' | 'warn' | 'error'> level;
};

struct CapabilityStatus {
    bool enabled;
    std::optional<'active' | 'inactive' | 'error'> status;
    std::optional<std::string> error;
    std::optional<std::string> lastUsed;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

struct MemoryQuery {
    std::optional<'knowledge' | 'conversation' | 'goal' | 'user' | 'relationship'> type;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<'createdAt' | 'importance'> orderBy;
    std::optional<'asc' | 'desc'> orderDirection;
};

struct MemoryResult {
    std::string id;
    std::string type;
    std::string content;
    Date createdAt;
    std::optional<double> importance;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};

using OllamaModelStatus = SharedOllamaModelStatus;

struct ModelDownloadProgress {
    std::string model;
    double progress;
    'downloading' | 'completed' | 'failed' status;
    std::optional<std::string> error;
};

// Define Tauri function types
using TauriInvokeFunction = (command: string, args?: Record<string, unknown>) => Promise<unknown>;
using TauriListenFunction = <T = Record<string, unknown>>(

// WebSocket event payload interfaces
struct WebSocketEventData {
    std::string type;
    std::optional<{> data;
    std::optional<std::string> type;
    std::optional<{> message;
    std::optional<std::string> id;
    std::optional<std::string> content;
    std::optional<std::string> text;
    std::optional<std::string> userId;
    std::optional<std::string> author;
    std::optional<std::string> name;
    std::optional<std::string> timestamp;
    std::optional<std::unordered_map<std::string, unknown>> metadata;
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
  private tauriInvoke: TauriInvokeFunction | null = null;
  private tauriListen: TauriListenFunction | null = null;
  private isTauri: boolean = false;
  private messageListeners: Set<(message: TauriMessage) => void> = new Set();
  private statusListeners: Set<(status: StartupStatus) => void> = new Set();
  private containerLogListeners: Set<(log: ContainerLog) => void> = new Set();
  private unlistenFns: Array<() => void> = [];
  private userId: string;
  private agentId: string = '2fbc0c27-50f4-09f2-9fe4-9dd27d76d46f';
  private isInitialized = false;
  private processedMessageIds: Set<string> = new Set();
  private recentMessages: Array<{ content: string; type: string; timestamp: number }> = [];

    // Try to initialize Tauri immediately

      // Use standard Tauri dialog plugin imports
      // Plugin not available (running in browser environment)

      // Try to import Tauri v2 APIs

      // If imports succeed, we're in Tauri environment

      // Set up event listeners
      // Import failed - not in Tauri environment

    // This method is now called by components to ensure initialization

    // Try again in case it failed initially

    // Helper function to emit message with deduplication
      // Ensure message has an ID

      // Check if we've already processed this message by ID

      // Check for duplicate content within a 2-second window

      // Check if we've seen this exact message content very recently (within 2 seconds)

      // Add to processed set and recent messages

      // Clean up old entries

      // Clean up recent messages older than 5 seconds

    // Listen for incoming messages from the agent
      // Ensure type is set to 'agent' for agent messages

    // Listen for startup status updates

    // Listen for WebSocket events

    // Listen for real-time updates from the Rust WebSocket manager
        // Handle different types of realtime updates
          // If type isn't set and it's from the agent, set it to 'agent'

    // Listen for WebSocket errors
        // Create an error message to display in the chat

        // Only emit error messages that aren't related to user messages
        // (to avoid duplicating user messages as errors)

    // Listen for container logs

    // Ensure we're initialized before trying to invoke

    // Try to detect and initialize again

    // Wait a bit for initialization

  // Event subscription methods

  // Clean up event listeners

  // WebSocket management

    return Boolean(response);

  // Message handling

    // Create user message for immediate UI feedback

    // Notify listeners of the user message

    return String(response || '');

  // Capability management

  // Settings management

  // Health check

  // Container management

  // Server operations

  // Data fetching methods

    // Convert file to base64 for transport through Tauri IPC

    // Handle different response formats

    // If response is wrapped in an object (e.g., { memories: [...] } or { data: [...] })

    // Ensure memories is an array

    // Convert memories to TauriMessage format

    // Handle different response formats using type-safe extraction

    // Extract memories using type-safe utility function
      // Direct array response
      // Unexpected format

    // Convert memories to TauriMessage format

  // Additional plugin configuration methods

  // Configuration operations

  // Dynamic Configuration Management

  // Backup and Restore Operations

        return String(response || exportPath);
        // Fall through to fallback

    // Fallback if Tauri dialog is not available
    return String(response || '');

        // Fall through to fallback

    // Fallback if Tauri dialog is not available

    // For now, just reload the window

  // Autonomy management

  // Alias for compatibility with GameInterface

  // Shell/browser capability management

  // Vision settings management

  // Agent management

    // Use type-safe log extraction
    return convertToRecordArray(logEntries);

  // Database operations

  // Frontend helper methods for API proxy

  // Plugin HTTP routes

      return String(response.data.content || '');

  /**
   * Check if Ollama models are loaded and ready
   */

  /**
   * Pull missing Ollama models
   */

  /**
   * Listen for model download progress
   */

  /**
   * Listen for model download completion
   */

  /**
   * Gracefully shut down the application
   */

// Export singleton instance

} // namespace elizaos
