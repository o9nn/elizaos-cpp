#include "api/index.js.hpp"
#include "api/system/environment.js.hpp"
#include "authMiddleware.js.hpp"
#include "bus.js.hpp"
#include "db/ServerDatabaseAdapter.js.hpp"
#include "db/ServerMigrationService.js.hpp"
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
    std::optional<std::string> clientPath;
};

/**
 * Determines if the web UI should be enabled based on environment variables.
 *
 * @returns {boolean} - Returns true if UI should be enabled, false otherwise
 */
bool isWebUIEnabled();

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
  private isWebUIEnabled: boolean = true; // Default to enabled until initialized
  private clientPath?: string; // Optional path to client dist files

  public database!: DatabaseAdapter;
  public serverDatabase!: any; // ServerDatabaseAdapter for messaging functionality

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

      // Register signal handlers once in constructor to prevent accumulation
      this.registerSignalHandlers();
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

      // Initialize server-specific database adapter

      // Run server-specific migrations

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
      // Store the client path if provided

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

      // Determine if web UI should be enabled

        // Determine the reason for UI being disabled

      // Agent-specific media serving - only serve files from agent-specific directories

      // Channel-specific media serving

      // Add specific middleware to handle portal assets
        // Automatically detect and handle static assets based on file extension

        // Set correct content type based on file extension

        // Continue processing

      // Setup static file serving with proper MIME types
          // Set the correct content type for different file extensions

      // Resolve client path for both static serving and SPA fallback

      // Conditionally serve static assets from the client dist path
      // Client files are built into the server package's dist/client directory
        // Try multiple locations to find the client dist files
          // First priority: explicitly provided client path
          // Primary location: server's own dist/client directory
          // Development: relative to server package (monorepo) - direct client build
          // Fallback: using require.resolve to find client package (if installed as dependency)
          // Check if running from global CLI - look for client files in the same directory as the running process
              // When running from server, check for client files relative to the server dist
                // Also check in the same directory (for backwards compatibility)
              // Ignore errors
          // Global bun install: check global node_modules locations
              // Try to find the global server installation via bun
              // Bun stores global packages in ~/.bun/install/global/node_modules
              // Also try npm root as fallback (some users might use npm)
                // npm might not be installed
              // Ignore errors
          // Alternative global locations (common paths)
            // Check nvm installations
                // Ignore errors

        // Log process information for debugging

          // Store the resolved client path on the instance for use in the SPA fallback

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
          // Use the resolved clientPath (prefer local variable, fallback to instance variable)

            // Verify the file exists before attempting to serve it

            // Use sendFile with the directory as root and filename separately
            // This approach is more reliable for Express
        // Return 403 Forbidden for non-API routes when UI is disabled

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
   * @returns {Promise<void>} A promise that resolves when the server is listening.
   * @throws {Error} If the port is invalid or if there is an error while starting the server.
   */

        // Use http server instead of app.listen with explicit host binding and error handling
        // For tests and macOS compatibility, prefer 127.0.0.1 when specified

            // Only show the dashboard URL if UI is enabled
              // Display the dashboard URL with the correct port after the server is actually listening
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

    // Create the channel first

    // Add participants if provided

    // Add participants one by one since the method expects a single userId

    // Get all messages for the channel and delete them one by one

    // Get the channel details to find the server ID
      // Emit to internal message bus for agent consumption

    // ServerDatabaseAdapter doesn't support beforeTimestamp, so we get all messages
    // TODO: Add timestamp filtering if needed

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
