#include "api/index.js.hpp"
#include "api/system/environment.js.hpp"
#include "authMiddleware.js.hpp"
#include "bus.js.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-sql.hpp"
#include "loader.js.hpp"
#include "services/message.js.hpp"
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

/**
 * Interface for defining server configuration options.
 * @typedef {Object} ServerOptions
 * @property {ServerMiddleware[]} [middlewares] - Optional array of server middlewares.
 * @property {string} [dataDir] - Optional directory for storing server data.
 * @property {string} [postgresUrl] - Optional URL for connecting to a PostgreSQL database.
 */
struct ServerOptions {
    std::optional<std::vector<ServerMiddleware>> middlewares;
    std::optional<std::string> dataDir;
    std::optional<std::string> postgresUrl;
};

/**
 * Class representing an agent server.
 */ /**
 * Represents an agent server which handles agents, database, and server functionalities.
 */
class AgentServer {
  public app!: express.Application;
  private agents: Map<UUID, IAgentRuntime>;
  public server!: http.Server;
  public socketIO!: SocketIOServer;
  public isInitialized: boolean = false; // Flag to prevent double initialization

  public database!: DatabaseAdapter;

  public startAgent!: (character: Character) => Promise<IAgentRuntime>;
  public stopAgent!: (runtime: IAgentRuntime) => void;
  public loadCharacterTryPath!: (characterPath: string) => Promise<Character>;
  public jsonToCharacter!: (character: unknown) => Promise<Character>;

  /**
   * Constructor for AgentServer class.
   *
   * @constructor
   */
  constructor() {
    try {
      logger.debug('Initializing AgentServer (constructor)...');
      this.agents = new Map();

      // Initialize character loading functions
      this.loadCharacterTryPath = loadCharacterTryPath;
      this.jsonToCharacter = jsonToCharacter;
    } catch (error) {
      logger.error('Failed to initialize AgentServer (constructor):', error);
      throw error;
    }
  }

  /**
   * Initializes the database and server.
   *
   * @param {ServerOptions} [options] - Optional server options.
   * @returns {Promise<void>} A promise that resolves when initialization is complete.
   */

      // Run migrations for the SQL plugin schema

        // Get the underlying database instance

        // Register the SQL plugin schema

        // Run the migrations

      // Add a small delay to ensure database is fully ready

      // Ensure default server exists

      // Check if the default server exists

      // Log all existing servers for debugging

        // Use raw SQL to ensure the server is created with the exact ID

          // Immediately check if it was created

          // Try creating with ORM as fallback

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

      // Apply custom middlewares if provided

      // Setup middleware for all requests

      // File uploads are now handled by individual routes using multer
      // No global file upload middleware needed

      // Optional Authentication Middleware
        // Apply middleware only to /api paths

      // Agent-specific media serving - only serve files from agent-specific directories
        // @ts-expect-error - this is a valid express route

        // @ts-expect-error - this is a valid express route

      // Channel-specific media serving

      // Add specific middleware to handle portal assets
        // Automatically detect and handle static assets based on file extension

        // Set correct content type based on file extension

        // Continue processing

      // Setup static file serving with proper MIME types
          // Set the correct content type for different file extensions

      // Serve static assets from the client dist path
      // Client files are built into the CLI package's dist directory

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

      // Add a catch-all route for API 404s
        // Check if this is an API route that wasn't handled
          // worms are going to hitting it all the time, use a reverse proxy if you need this type of logging
          //logger.warn(`API 404: ${req.method} ${req.path}`);
          // Not an API route, continue to next middleware

      // Main fallback for the SPA - must be registered after all other routes
      // Use a final middleware that handles all unmatched routes
        // For JavaScript requests that weren't handled by static middleware,
        // return a JavaScript response instead of HTML

        // For all other routes, serve the SPA's index.html
        // Client files are built into the CLI package's dist directory

      // Create HTTP server for Socket.io

      // Initialize Socket.io, passing the AgentServer instance

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
   * @throws {Error} If the port is invalid or if there is an error while starting the server.
   */

      // Use http server instead of app.listen with explicit host binding and error handling
      // For tests and macOS compatibility, prefer 127.0.0.1 when specified

          // Only show the dashboard URL in production mode
            // Display the dashboard URL with the correct port after the server is actually listening

          // Add log for test readiness

          // Provide helpful error messages for common issues

      // Enhanced graceful shutdown

        // Stop all agents first

        // Close database

        // Close server

        // Force close after timeout

  /**
   * Stops the server if it is running. Closes the server connection,
   * stops the database connection, and logs a success message.
   */

  // Central DB Data Access Methods

    // Get all messages for the channel and delete them one by one

    // Get the channel details to find the server ID
      // Emit to internal message bus for agent consumption

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

// Export loader utilities

// Export types
* from './types';

} // namespace elizaos
