#include "api/index.hpp"
#include "api/system/environment.hpp"
#include "authMiddleware.hpp"
#include "bus.hpp"
#include "database/ServerDatabaseAdapter.hpp"
#include "database/ServerMigrationService.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
#include "elizaos/plugin-todo.hpp"
#include "game-api-plugin.hpp"
#include "loader.hpp"
#include "services/message.hpp"
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



// CRITICAL: Ensure http module is available globally before importing ws
// This fixes Bun bundling issues with the ws module

  // Ensure STATUS_CODES is available - this is needed for WebSocket compatibility
    // Just ensure the http module has what it needs without full replacement

  // Override require to ensure http is always available
    // Ignore if Module is not available

// Delay WebSocketServer import until after http polyfill

/**
 * Expands a file path starting with `~` to the project directory.
 *
 * @param filepath - The path to expand.
 * @returns The expanded path.
 */
std::string expandTildePath(const std::string& filepath);

std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir);

/**
 * Represents a function that acts as a server middleware.
 * @param {express.Request} req - The request object.
 * @param {express.Response} res - The response object.
 * @param {express.NextFunction} next - The next function to be called in the middleware chain.
 * @returns {void}
 */
using ServerMiddleware = (

// Define a type for Express errors

// Define WebSocket message data interface
struct WebSocketMessageData {
    std::string channelId;
    std::string agentId;
    std::string content;
    std::string author;
    std::unordered_map<std::string, unknown> metadata;
};

// Define log entry interface
struct LogEntry {
    std::string timestamp;
    std::string level;
    std::string message;
    std::optional<std::string> agentName;
};

/**
 * Class representing an agent server.
 */ /**
 * Represents an agent server which handles agents, database, and server functionalities.
 */
class AgentServer {
  public app!: express.Application;
  public agents: Map<UUID, IAgentRuntime>;
  public server!: http.Server;
  public webSocketServer!: WebSocketServer; // Native WebSocket server
  private logStreamConnections: Map<string, { agentName?: string; level?: string }> = new Map();

  /**
   * Constructor for AgentServer class.
   *
   * @constructor
   */

      // Initialize character loading functions

      // Register signal handlers once in constructor to prevent accumulation

  /**
   * Initializes the database and server.
   *
   * @param {ServerOptions} [options] - Optional server options.
   * @returns {Promise<void>} A promise that resolves when initialization is complete.
   */

      // Create a temporary database adapter just to get the raw database connection

      // Get the raw database connection (BaseDrizzleAdapter has getDatabase method)
      struct BaseDrizzleAdapter {
};

      // Create the server-specific database adapter

      // Keep the agent database adapter for backward compatibility

      // Run server-specific migrations first

      // Then run plugin migrations for core ElizaOS tables

      // Register all plugin schemas: SQL (core tables), Todo, and Goals

      // Run the migrations

      // Verify critical tables exist after migrations
          // Handle different result structures - Drizzle may return rows differently

      // Add a small delay to ensure database is fully ready

      // Ensure default server exists

    // Check if the default server exists using server database

    // Log all existing servers for debugging

    // Verify it was created

  /**
   * Initializes the server with the provided options.
   *
   * @param {ServerOptions} [options] - Optional server options.
   * @returns {Promise<void>} - A promise that resolves once the server is initialized.
   */
    // Initialize middleware and database

    // Security headers first - before any other middleware
        // Content Security Policy - environment-aware configuration
              // Production CSP - includes upgrade-insecure-requests
                // upgrade-insecure-requests is added by helmet automatically
              // Development CSP - minimal policy without upgrade-insecure-requests
                // Note: upgrade-insecure-requests is intentionally omitted for Safari compatibility
        // Cross-Origin Embedder Policy - disabled for compatibility
        // Cross-Origin Resource Policy
        // Frame Options - allow same-origin iframes to align with frameSrc CSP
        // Hide Powered-By header
        // HTTP Strict Transport Security - only in production
        // No Sniff
        // Referrer Policy
        // X-XSS-Protection

    // Setup middleware for all requests

    // File uploads are now handled by individual routes using multer
    // No global file upload middleware needed

    // Optional Authentication Middleware
      // Apply middleware only to /api paths

    // Agent-specific media serving - only serve files from agent-specific directories

    // Channel-specific media serving

    // Add specific middleware to handle portal assets
      // Automatically detect and handle static assets based on file extension

      // Set correct content type based on file extension

      // Continue processing

    // Setup static file serving with proper MIME types
        // Set the correct content type for different file extensions

    // *** NEW: Mount the plugin route handler BEFORE static serving ***

    // Mount the core API router under /api
    // This router handles all API endpoints including:
    // - /api/agents/* - Agent management and interactions
    // - /api/messaging/* - Message handling and channels
    // - /api/media/* - File uploads and media serving
    // - /api/memory/* - Memory management and retrieval
    // - /api/audio/* - Audio processing and transcription
    // - /api/server/* - Runtime and server management
    // - /api/tee/* - TEE (Trusted Execution Environment) operations
    // - /api/system/* - System configuration and health checks

    // Store server instance for plugins to access

    // Also store as global for plugins that need WebSocket broadcasting

    // Serve the simple messaging UI
      // Try multiple paths including Docker container path

    // Add a catch-all route for API 404s
      // Check if this is an API route that wasn't handled
        // worms are going to hitting it all the time, use a reverse proxy if you need this type of logging
        //logger.warn(`API 404: ${req.method} ${req.path}`);
        // Not an API route, continue to next middleware

    // Return 403 Forbidden for non-API routes when UI is disabled

    // Create HTTP server for Socket.io and WebSocket

    // Initialize native WebSocket server for Tauri client compatibility

    // Setup log streaming integration

    // Setup message bus listener for agent responses

  /**
   * Sets up listener for agent responses from internal message bus
   */

      // Cast to access properties (response structure is dynamic)

      // Broadcast to WebSocket clients in the channel

  /**
   * Sets up the native WebSocket server for real-time communication
   * This provides the /ws endpoint that Tauri clients expect
   */

      // Handle initial connection message

            // Handle connection setup

            // Send connection acknowledgment

            // Handle incoming messages from client

            // Process message through agent

            // Handle media stream data

            // Decode base64 data if provided as string
              // Assume base64 encoded
              // Array of bytes

            // Get agent runtime
              // Store in media buffer

              // Notify vision service
              struct VisionServiceMethods {
    std::optional<(data: any) => Promise<void>> processMediaStream;
};

              // Send acknowledgment

            // Handle log streaming subscription

            // Handle log streaming unsubscription

            // Handle log filter updates

        // Clean up log stream connections

      // Send initial connection message with delay to ensure handshake completion

  /**
   * Process WebSocket messages and route them to agents
   */

      // Validate required fields

      // Convert channelId string to UUID

      // Find the agent runtime

      // Check if channel exists, if not create it

        // Get or create a default server

          // Create a default server if none exists

        // Create the channel

      // Create message for internal processing

      // Store message in central database

      // Send acknowledgment back to WebSocket client

      // The createMessage method already emits to the internal bus, so we don't need to emit again

      // Don't broadcast user messages back immediately - let the agent response come through the message bus
      // This prevents the echo issue where users see their own messages as "agent messages"
      /*
      // Broadcast message to other WebSocket clients in the same channel
      */

  /**
   * Broadcast messages to WebSocket clients
   */

        // WebSocket.OPEN
        // TODO: Add channel-based filtering once we track client channel subscriptions
        // For now, broadcast to all connected clients

  /**
   * Setup log streaming integration with the logger
   */

    // Access the logger's destination to hook into log events

      // Store original write method

      // Override write method to broadcast logs via WebSocket
        // Call original write first

        // Parse and broadcast log entry

          // Add timestamp if not present

          // Broadcast to WebSocket clients with log subscriptions
          // Ignore JSON parse errors for non-log data

  /**
   * Broadcast log entries to subscribed WebSocket clients
   */

      // Find the WebSocket client by clientId
      // Note: We need to track clientId to WebSocket mapping
          // WebSocket.OPEN

          // Apply agent name filter

          // Apply log level filter

  /**
   * Registers an agent with the provided runtime.
   *
   * @param {IAgentRuntime} runtime - The runtime object containing agent information.
   * @throws {Error} if the runtime is null/undefined, if agentId is missing, if character configuration is missing,
   * or if there are any errors during registration.
   */

      // Auto-register the MessageBusConnector plugin
        // Decide if this is a fatal error for the agent.

      // Register TEE plugin if present

  /**
   * Unregisters an agent from the system.
   *
   * @param {UUID} agentId - The unique identifier of the agent to unregister.
   * @returns {void}
   */

      // Retrieve the agent before deleting it from the map

        // Stop all services of the agent before unregistering it

      // Delete the agent from the map

  /**
   * Add middleware to the server's request handling pipeline
   * @param {ServerMiddleware} middleware - The middleware function to be registered
   */

  /**
   * Starts the server on the specified port.
   *
   * @param {number} port - The port number on which the server should listen.
   * @returns {Promise<void>} A promise that resolves when the server is listening.
   * @throws {Error} If the port is invalid or if there is an error while starting the server.
   */

        // Use http server instead of app.listen with explicit host binding and error handling
        // For tests and macOS compatibility, prefer 127.0.0.1 when specified

            // Use actual host or localhost

            // Add log for test readiness

            // Resolve the promise now that the server is actually listening

            // Provide helpful error messages for common issues

            // Reject the promise on error

        // Server is now listening successfully

  /**
   * Stops the server if it is running. Closes the server connection,
   * stops the database connection, and logs a success message.
   */

  // Central DB Data Access Methods

    // Convert Channel to MessageChannel

    // Add participants if provided

    // Get all messages for the channel and delete them one by one

    // Convert Message to CentralRootMessage

    // Get the channel details to find the server ID
      // Emit to internal message bus for agent consumption

      // Also broadcast to WebSocket clients if this is from an agent response

  // Optional: Method to remove a participant
    // Since we don't have a direct method for this, we'll need to handle it at the channel level

  // ===============================
  // Server-Agent Association Methods
  // ===============================

  /**
   * Add an agent to a server
   * @param {UUID} serverId - The server ID
   * @param {UUID} agentId - The agent ID to add
   */
    // First, verify the server exists

  /**
   * Remove an agent from a server
   * @param {UUID} serverId - The server ID
   * @param {UUID} agentId - The agent ID to remove
   */

  /**
   * Get all agents associated with a server
   * @param {UUID} serverId - The server ID
   * @returns {Promise<UUID[]>} Array of agent IDs
   */

  /**
   * Get all servers an agent belongs to
   * @param {UUID} agentId - The agent ID
   * @returns {Promise<UUID[]>} Array of server IDs
   */
    // This method isn't directly supported in the adapter, so we need to implement it differently

  /**
   * Registers signal handlers for graceful shutdown.
   * This is called once in the constructor to prevent handler accumulation.
   */

      // Stop all agents first

      // Close database

      // Close server

        // Force close after timeout

// Export loader utilities

// Export types
* from './types';

} // namespace elizaos
