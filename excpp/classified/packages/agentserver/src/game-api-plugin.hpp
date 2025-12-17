#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "elizaos/plugin-autonomy.hpp"
#include "elizaos/plugin-goals.hpp"
#include "elizaos/plugin-inference.hpp"
#include "elizaos/plugin-knowledge.hpp"
#include "elizaos/plugin-todo.hpp"
#include "elizaos/plugin-vision.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define common request/response interfaces
struct GameApiRequest {
};

struct GameApiResponse {
};

// Media stream buffer interface
struct MediaStreamBuffer {
    std::vector<Uint8Array> videoFrames;
    std::vector<Uint8Array> audioChunks;
    double maxBufferSize;
};

// Validation result interfaces
struct ValidationResults {
    std::string overall;
    double timestamp;
};

struct ValidationConfig {
    std::string status;
    std::optional<std::string> apiKey;
    std::optional<std::string> status;
    std::optional<std::string> message;
    std::optional<bool> modelAvailable;
    std::optional<std::string> model;
};

struct EnvironmentValue {
    std::optional<std::string> value;
};

struct ServiceConfig {
    std::string status;
};

struct AgentServerInstance {
};

// Global media buffers that vision plugin can access

// Virtual screen capture state

// Start capturing agent's virtual screen
std::future<void> startAgentScreenCapture(IAgentRuntime runtime, std::optional<unknown> server);

// Stop capturing agent's virtual screen
std::future<void> stopAgentScreenCapture();

// Standard API response helpers
void successResponse(unknown data);

void errorResponse(const std::string& code, const std::string& message, std::optional<unknown> details);

/**
 * Generates configuration recommendations based on validation results
 */
std::vector<std::string> generateConfigRecommendations(ValidationResults validationResults);

/**
 * Creates initial todos and goals using plugin APIs
 */
std::future<void> createInitialTodosAndGoals(IAgentRuntime runtime);

// Game API Routes following ElizaOS patterns

  // Health check (custom game-specific health)
      // Health check should always return success for the server itself
      // Agent status is informational, not a requirement

      // Check if critical services are available - only if runtime exists

          // Debug: Log service lookup results

  // Goals API - Legacy redirects (goals plugin now provides /api/goals directly)
      // Redirect to the standard goals plugin endpoint

  // Create Goal API - Legacy redirect
      // Redirect to the standard goals plugin endpoint

  // Todos API - Legacy redirects (todos plugin now provides /api/todos directly)
      // Redirect to the standard todos plugin endpoint

  // Create Todo API - Legacy redirect
      // Redirect to the standard todos plugin endpoint

  // Memories API

  // Vision Settings API

      // Get settings from runtime first

      // Vision service configuration is handled through runtime settings

  // Vision Refresh API

        // Vision service restart is handled by the runtime

  // NOTE: Autonomy API endpoints are now handled by the autonomy plugin's native routes
  // The autonomy plugin registers its own /autonomy/status, /autonomy/enable, /autonomy/disable routes

  // Autonomy Toggle API

  // Shell Capability API

      // Check for shell service with different possible names

  // Shell Toggle API

  // Browser Capability API

      // Check for browser service with different possible names

  // Browser Toggle API

  // Generic Capability Toggle API (handles camera, microphone, speakers, etc.)

      // Define capability mappings

      // Get current state

      // Update all related settings

      // Special handling for autonomy

      // Vision capabilities are handled through runtime settings

      // Get service availability

  // Reset agent

      // Give it a moment to ensure operations complete

      // Create initial todos and goals again

      // Try express-fileupload format first
      // Try alternative file handling (check if uploaded via different parser)
        // If we got a file in the body somehow

        // Look for file-like properties in body

      // Create knowledge options

      // Add the knowledge using properly typed service

      // Use the knowledge service deleteMemory method to actually delete the document

  // Plugin Configuration Routes
      // Get all services and their configurations

      // Get configurations for each service

      // Add current environment configurations

      // Handle environment variables specially

        // Update specific plugin configuration

  // Debug API - Runtime State Serialization

      // Safely serialize runtime state for debugging
        // Basic runtime info

        // Character information (safe to expose)

        // Services information
            // Safe service state extraction

        // Plugin information

        // Actions information

        // Providers information

        // Evaluators information

        // Routes information

        // Event handlers information

        // Settings information (safe keys only, no sensitive data)
          // Only expose non-sensitive settings

        // Database connection status (if available)

        // Memory stats (if available)

        // Performance/status information

  // Debug API - Service Details

      // Safely extract service information

        // Common service methods/properties

        // Safe method checks

        // Try to get status if available

        // Try to get configuration if available (non-sensitive only)

  // Service debugging endpoint

      // Iterate through all services and log detailed information
            // For goal service
            // For todo service
            // For autonomy service
            // Service description

          // Specific service checks using getService instead of has()

  // Memories API for Monologue Tab

      // Fetch memories from the specified room

      // Return in the expected format

  // Logs API endpoint

      // Collect logs from various sources

      // Add recent agent activity logs
        // Get recent conversations

      // Add system logs if available
        // Get service status logs

        // Add runtime status

      // Sort logs by timestamp (newest first)

      // Apply limit

  // Autonomy control routes (since autonomy plugin routes aren't being registered)
      // Use uppercase 'AUTONOMY' as per the service's serviceType

        // List all available services for debugging

  // Game startup initialization

        // Check if services are available - use correct service names

          // Check if we already have goals
              // Create initial goals and todos

  // Initialize goals and todos manually

        // Check if services are available first - use correct service names

  // Configuration Validation API

      // Check MODEL_PROVIDER environment variable

      // Validate OpenAI configuration

      //   validationResults.providers.openai = {
      //     apiKey: openaiKey ? 'present' : 'missing',
      //     model: openaiModel,
      //     status: openaiKey ? 'healthy' : 'unhealthy',
      //     message: openaiKey
      //       ? `OpenAI configured with model: ${openaiModel}`
      //       : 'OpenAI API key missing',
      //   };

      //   // Test OpenAI connection if key is present
      //   if (openaiKey) {
      //       method: 'GET',
      //       headers: {
      //         Authorization: `Bearer ${openaiKey}`,
      //         'Content-Type': 'application/json',
      //       },
      //     });

      //     if (testResponse.ok) {
      //       validationResults.providers.openai.connectionTest = {
      //         status: 'success',
      //         modelAvailable: hasModel,
      //         message: hasModel
      //           ? 'Connection successful and model available'
      //           : `Connection successful but model ${openaiModel} not found`,
      //       };
      //       if (!hasModel) {
      //         validationResults.providers.openai.status = 'degraded';
      //       }
      //     } else {
      //       validationResults.providers.openai.connectionTest = {
      //         status: 'failed',
      //         error: `HTTP ${testResponse.status}: ${testResponse.statusText}`,
      //         message: 'Failed to connect to OpenAI API',
      //       };
      //       validationResults.providers.openai.status = 'unhealthy';
      //     }
      //   }
      // }

      // Validate Anthropic configuration

      //   validationResults.providers.anthropic = {
      //     apiKey: anthropicKey ? 'present' : 'missing',
      //     model: anthropicModel,
      //     status: anthropicKey ? 'healthy' : 'unhealthy',
      //     message: anthropicKey
      //       ? `Anthropic configured with model: ${anthropicModel}`
      //       : 'Anthropic API key missing',
      //   };

      //   // Test Anthropic connection if key is present
      //   if (anthropicKey) {
      //     // Anthropic doesn't have a simple models endpoint, so we'll test with a minimal request
      //       method: 'POST',
      //       headers: {
      //         Authorization: `Bearer ${anthropicKey}`,
      //         'Content-Type': 'application/json',
      //         'anthropic-version': '2023-06-01',
      //       },
      //         model: anthropicModel,
      //         max_tokens: 1,
      //         messages: [{ role: 'user', content: 'test' }],
      //       }),
      //     });

      //       // 400 is expected for this minimal test request
      //       validationResults.providers.anthropic.connectionTest = {
      //         status: 'success',
      //         message: 'Connection successful',
      //       };
      //     } else {
      //       validationResults.providers.anthropic.connectionTest = {
      //         status: 'failed',
      //         error: `HTTP ${testResponse.status}: ${testResponse.statusText}`,
      //         message: 'Failed to connect to Anthropic API',
      //       };
      //       validationResults.providers.anthropic.status = 'unhealthy';
      //     }
      //   }
      // }

      // Validate Ollama configuration

        // Test Ollama connectivity with a simple message

        // Test Ollama connection

          // Check if model is available

      // Check embedding model configuration

      // Test runtime services

      // Calculate overall status

  // Configuration Test API - Tests actual LLM functionality

      // Test basic LLM completion

      // Create a simple test prompt

      // Use the runtime to generate completion (this will use the configured provider)

      // Test embedding functionality

      // Test memory operations

      // Skip actual memory creation to avoid database constraints
      // Just test that the memory service is available

      // Calculate overall test status

  // Get available LLM providers and their status

  // Set selected provider

  // Set provider preferences

  // Media stream endpoint for receiving video/audio data

      // Initialize buffer for agent if not exists

      // Store data in appropriate buffer

      // Notify vision service if available

  // Get media buffer status

  // Update Settings API

  // Virtual screen control endpoints
        // Get server instance from request if available

      // Convert Uint8Array to base64 for proper transmission

      // Return raw JPEG image

  // Runtime State API (for tests)

          // Get list of loaded agents

  // Monologue API (for tests)

        // Get recent memories that represent the agent's monologue/thoughts

  // Message Ingestion API (for tests)

        // Validate required fields

        // Check if we have a runtime

        // Get the server instance from the global variable

        // Check if channel exists, create if not

          // Use the provided server_id or default

          // Create the channel

        // Create message in the database

        // Emit to the internal message bus for agent processing

  // Memory Query API

        // Get memories using the runtime's getMemories method

  // Legacy message endpoint (redirect to new endpoint)

        // Transform to new format

        // Forward to new endpoint
          return ingestHandler(req, res, runtime);

// Export functions for vision plugin to access media buffers
MediaStreamBuffer getMediaBuffer(const std::string& agentId);

void clearMediaBuffer(const std::string& agentId);

// Plugin const gameAPIPlugin: Plugin = {

    // Log each route being registered

    // Store reference to global ElizaOS server if available

    // Debug: List all registered services

    // Specifically check for autonomy service

    // Don't create initial todos/goals immediately - wait for a ready signal
    // This will be triggered by the /api/initialize-goals-todos endpoint if needed

    // Set a flag to indicate the plugin is ready

    // Schedule initial todos/goals creation after a delay to ensure everything is ready
        // Check if agent exists and has no goals yet

    // Auto-start screen capture for VNC streaming after a delay
        // Not fatal - can be started manually later


} // namespace elizaos
