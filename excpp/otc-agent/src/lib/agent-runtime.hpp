#include "agent.hpp"
#include "elizaos/core.hpp"
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

// Serverless-compatible agent runtime with Drizzle ORM for Next.js
// DO NOT replace with an agent-simple.ts, it won't work!

// Global state for serverless environment persistence
struct GlobalElizaState {
    std::optional<bool> __elizaMigrationsRan;
    std::optional<bool> __elizaManagerLogged;
    std::optional<AgentRuntime | null> __elizaRuntime;
    std::optional<{> logger;
    typeof console.log log;
    typeof console.info info;
    typeof console.warn warn;
    typeof console.error error;
    typeof console.debug debug;
};

class AgentRuntimeManager {
  private static instance: AgentRuntimeManager;
  public runtime: AgentRuntime | null = null;
  private hasRunMigrations = false;
  private initializationPromise: Promise<AgentRuntime> | null = null;

  private constructor() {
    // Configure the elizaLogger to use console
    if (elizaLogger) {
      elizaLogger.log = console.log.bind(console);
      elizaLogger.info = console.info.bind(console);
      elizaLogger.warn = console.warn.bind(console);
      elizaLogger.error = console.error.bind(console);
      elizaLogger.debug = console.debug.bind(console);
      elizaLogger.success = (msg: string | Record<string, unknown> | Error) =>
        console.log(`✓ ${typeof msg === "string" ? msg : JSON.stringify(msg)}`);
      // elizaLogger doesn't have notice in types but may be used at runtime
      const logger = elizaLogger as typeof elizaLogger & {
        notice?: typeof console.info;
      };
      logger.notice = console.info.bind(console);
    }

    // Also configure global console if needed
    if (!globalState.logger) {
      globalState.logger = {
        log: console.log.bind(console),
        info: console.info.bind(console),
        warn: console.warn.bind(console),
        error: console.error.bind(console),
        debug: console.debug.bind(console),
      };
    }

    if (!globalState.__elizaManagerLogged) {
      // Silence noisy init log; keep flag to avoid repeated work
      globalState.__elizaManagerLogged = true;
    }
  }

  // Helper method to get or create the runtime instance
  // Uses a single initialization promise to prevent concurrent initialization attempts
    // Priority 1: Reuse instance runtime if already set

    // Priority 2: Reuse global cached runtime (persists across warm serverless containers)

    // Priority 3: If initialization is already in progress, wait for it

    // Priority 4: Create new runtime with single initialization promise

      // Clear the promise so next attempt can try again

  // Separate method for actual runtime creation (called once)

    // Initialize runtime with database configuration for SQL plugin
    // In localnet mode, connects to Docker PostgreSQL on port 5439
    // In production (Vercel), uses Neon Storage integration variables

    // Check for Vercel Neon Storage variables first, then fallback to standard names
    // Log which env vars are present (without values) for debugging

    // Validate database URL format

    // Validate URL format (basic check)
      // Check for hostname (basic validation)

    // Use the existing agent ID from DB (b850bc30-45f8-0041-a00a-83df46d8555d)

    // Cache globally for reuse in warm container

    // Ensure runtime has a logger with all required methods

    // Ensure SQL plugin built-in tables exist (idempotent)

    // Initialize runtime - this calls ensureAgentExists internally (runtime.ts:405)
    // which creates both the agent record AND its entity record

    // Log registered services

    // Database adapter and migrations are handled by @elizaos/plugin-sql during runtime.initialize()
    // Quotes and user sessions are stored via runtime.getCache/setCache

  // Helper method to handle messages

    // Ensure room and entity connection (follows Eliza's ensureConnection pattern)

    // Create user message
    // Emit MESSAGE_RECEIVED and delegate handling to plugins
    // Note: The message handler plugin (otcDeskPlugin) is responsible for:
    // 1. Saving the user message to memory
    // 2. Generating the agent response
    // 3. Saving the agent response to memory
    // The callback is just for notification - do NOT create memory here
    // to avoid duplicate agent messages.
        // Callback is for notification only - memory is saved by the message handler

// Export singleton instance

} // namespace elizaos
