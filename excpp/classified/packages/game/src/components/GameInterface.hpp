#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".hooks/useTauriChat.hpp"
#include ".services/TauriService.hpp"
#include ".utils/SecurityUtils.hpp"
#include ".utils/screenCapture.hpp"
#include "AgentLogs.hpp"
#include "BackupSettings.hpp"
#include "ContainerLogs.hpp"
#include "OllamaModelSelector.hpp"
#include "ProviderSelector.hpp"
#include "SecurityWarning.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Extend Window interface for test compatibility
  struct Window {
    bool connected;
};

struct OutputLine {
    std::variant<'user', 'agent', 'system', 'error'> type;
    std::string content;
    Date timestamp;
};

struct PluginToggleState {
    bool autonomy;
    bool camera;
    bool screen;
    bool microphone;
    bool speakers;
    bool shell;
    bool browser;
};

struct SecurityWarningState {
    bool isVisible;
    std::string capability;
};

// Track individual warning acknowledgments and feature usage
struct CapabilityUsageState {
    bool hasBeenUsed;
    bool warningAcknowledged;
    std::optional<Date> firstSeenAt;
};

struct MediaStreams {
    std::optional<MediaStream> camera;
    std::optional<MediaStream> screen;
    std::optional<MediaStream> microphone;
    std::optional<MediaStream> speakers;
};

struct StreamingState {
    bool camera;
    bool screen;
    bool microphone;
    bool speakers;
};

// Enhanced capability buttons with progression support
struct ProgressionStatus {
    std::optional<std::vector<std::string>> unlockedCapabilities;
    std::optional<std::string> mode;
    std::optional<bool> progressionReady;
};

  // Check if a capability is unlocked based on progression
      // Fallback: if no progression data, allow all capabilities

    // Map UI capability names to progression capability names

    // Check if capability is unlocked

struct Goal {
    std::string id;
    std::string name;
    std::string description;
    bool isCompleted;
    std::string createdAt;
};

struct Todo {
    std::string id;
    std::string name;
    std::variant<'daily', 'one-off', 'aspirational'> type;
    bool isCompleted;
    std::optional<double> priority;
};

  // Chat state

  // Game API readiness state

  // Progression state

  // Model readiness state

  // Plugin toggles

  // Data state
  // Agent status removed - not currently used

  // UI state
  // Config dialog removed - not currently used

  // Security state

  // Track capability usage and individual warning states
        // Convert date strings back to Date objects

  // Individual security warnings for each capability

  // Media streaming state

  // Refs

  // Check if running in Tauri environment - make it reactive

  // Check Tauri environment on mount and after a delay
      // Initial check

      // Wait for potential initialization

      // Check again after initialization

    // Check screen capture capabilities on startup

  // Listen for startup status updates to track Game API readiness

        // Get current startup status

            // Check for error states that affect Game API

        // Listen for startup status updates

            // Update error state if Game API fails

    // Cleanup

  // Use Tauri chat hook if in Tauri, otherwise show loading or error

  // Handle incoming messages
    // Only process new messages

      // Ensure timestamp is a Date object

      // Add to output - deduplication is now handled by TauriService

      // Add to agent monologue if it contains thinking patterns

    // Update last processed index

  // Connection status requires both WebSocket connection AND Game API readiness

  // Media capture functions

  // Stop media streams

      // Clean up audio processor if exists

  // Send frame to Tauri backend

  // Send audio chunk to Tauri backend

  // Process video streams and send frames

    // Set canvas size based on stream type

      // Get image data as JPEG

    // Capture frames at 10 FPS

    // Store interval for cleanup

  // Process audio stream

      // Convert float32 to int16

    // Store processor for cleanup

  // Security-aware capability toggle handler

    // Check if this is a dangerous capability that requires security warning

    // If enabling a dangerous capability, show security warning first
      // Only show warning if not previously acknowledged or if it's the first time
              // Mark warning as acknowledged

    // For non-dangerous capabilities or disabling, proceed directly

  // Actual API toggle implementation (extracted for reuse)

        // Use TauriService for all capability toggles

        // Not in Tauri - shouldn't happen in production

        // Update the local plugin state to reflect the change

        // Mark capability as used

        // Log security events for dangerous capabilities

  // Helper function to copy text to clipboard
      // Could show a notification here

  // Helper function to update vision settings (removed - using TauriService now)

  // Agent reset functionality

        // Clear local state

        // Reset plugins to default state

        // Show success message

  // Data fetching

        // Use TauriService for IPC
        // Fallback - shouldn't happen in production

      // Goals loaded successfully - no longer showing in chat
      // Error logged to console but not shown in chat to avoid clutter

        // Use TauriService for IPC
        // Fallback - shouldn't happen in production

      // TODOs loaded successfully - no longer showing in chat
      // Error logged to console but not shown in chat to avoid clutter

      // Handle new API response format

      // First, get the autonomy room ID from the autonomy status endpoint

      // If we don't have the autonomy room ID, try fallback approach

      // Fetch ALL messages from the autonomous room

      // Show ALL messages from the autonomy room in chronological order

      // Handle new API response format

      // Only update vision-related settings, preserve others

      // Handle response format

        // Initialize config values with current values

      // Validate configuration value

      // Update local state immediately for responsive UI

      // Don't send empty values for API keys

      // Send update to server with proper error handling

      // Show success message

      // If we're updating critical environment variables, refresh the config
        // Small delay then refresh plugin configs to get updated status

        // Show overall status

        // Show errors if any

        // Configuration validation complete

        // Show overall test status

        // Show test message

      // Refresh the file list

    // Validate file upload

      // Refresh the file list

    // Reset the input

  // Auto-scroll chat

  // Persist capability usage to localStorage

  // Track when capabilities are unlocked

  // Initial state fetching for capabilities
      // Fetch autonomy status
      // Fetch vision settings
      // Fetch shell settings
      // Fetch browser settings
      // Fetch progression status

  // Periodic data refresh - simplified to avoid state conflicts
      // Don't auto-refresh capability states to avoid conflicts with user interactions

    // Initial fetch - fetch both data and initial plugin states

  // Media streaming state management

  // Listen for agent screen frames from Tauri

            // Get the agent screen canvas

            // Set canvas dimensions

            // Convert frame data back to Uint8Array

            // Create blob and display on canvas

  // Security-aware chat handlers

    // Validate and sanitize user input

      // User message handled by TauriService immediately, server response via useEffect

                      // Clear model selection when provider changes
                        // Clear model selection when provider changes

                          // Start capturing frames

                          // Handle stream end

    // Create mock elizaClient for test compatibility
          // Store listeners for later use

          // Auto-trigger connect event for new listeners
          // Emit events to listeners

          // Simulate server response after a delay

      // Simulate connection

    // Update mock socket connection status

                  // Keep original for typing experience, will sanitize on submit

                    // Show error in console instead of alert
                    // Clear and focus the input for retry


} // namespace elizaos
