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
;
;

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


const globalState = globalThis as unknown as GlobalElizaState;
if (typeof globalState.__elizaMigrationsRan === "undefined")
  globalState.__elizaMigrationsRan = false;
if (typeof globalState.__elizaManagerLogged === "undefined")
  globalState.__elizaManagerLogged = false;

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

  public static getInstance(): AgentRuntimeManager {
    if (!AgentRuntimeManager.instance) {
      AgentRuntimeManager.instance = new AgentRuntimeManager();
    }
    return AgentRuntimeManager.instance;
  }

  public isReady(): boolean {
    return true;
  }

  // Helper method to get or create the runtime instance
  // Uses a single initialization promise to prevent concurrent initialization attempts
  async getRuntime(): Promise<AgentRuntime> {
    // Priority 1: Reuse instance runtime if already set
    if (this.runtime) {
      return this.runtime;
    }

    // Priority 2: Reuse global cached runtime (persists across warm serverless containers)
    if (globalState.__elizaRuntime) {
      this.runtime = globalState.__elizaRuntime;
      return this.runtime;
    }

    // Priority 3: If initialization is already in progress, wait for it
    if (this.initializationPromise) {
      return this.initializationPromise;
    }

    // Priority 4: Create new runtime with single initialization promise
    this.initializationPromise = this.createRuntime();

    try {
      const runtime = await this.initializationPromise;
      return runtime;
    } catch (error) {
      // Clear the promise so next attempt can try again
      this.initializationPromise = null;
      throw error;
    }
  }

  // Separate method for actual runtime creation (called once)
  private async createRuntime(): Promise<AgentRuntime> {
    console.log("[AgentRuntime] Creating runtime instance");

    // Initialize runtime with database configuration for SQL plugin
    // In localnet mode, connects to Docker PostgreSQL on port 5439
    // In production (Vercel), uses Neon Storage integration variables
    const dbPort =
      process.env.POSTGRES_DEV_PORT ||
      process.env.VENDOR_OTC_DESK_DB_PORT ||
      5439;
    const DEFAULT_POSTGRES_URL = `postgres://eliza:password@localhost:${dbPort}/eliza`;

    // Check for Vercel Neon Storage variables first, then fallback to standard names
    // Log which env vars are present (without values) for debugging
    const dbEnvVars = {
      DATABASE_POSTGRES_URL: !!process.env.DATABASE_POSTGRES_URL,
      DATABASE_URL_UNPOOLED: !!process.env.DATABASE_URL_UNPOOLED,
      POSTGRES_URL: !!process.env.POSTGRES_URL,
      POSTGRES_DATABASE_URL: !!process.env.POSTGRES_DATABASE_URL,
    };
    console.log("[AgentRuntime] Database env vars present:", dbEnvVars);

    const postgresUrl =
      process.env.DATABASE_POSTGRES_URL || // Vercel Neon Storage (pooled)
      process.env.DATABASE_URL_UNPOOLED || // Vercel Neon Storage (unpooled)
      process.env.POSTGRES_URL || // Standard
      process.env.POSTGRES_DATABASE_URL || // Alternative standard
      DEFAULT_POSTGRES_URL; // Local development

    // Validate database URL format
    if (!postgresUrl || postgresUrl === DEFAULT_POSTGRES_URL) {
      const isProduction = process.env.NODE_ENV === "production";
      if (isProduction && postgresUrl === DEFAULT_POSTGRES_URL) {
        console.error(
          "[AgentRuntime] ERROR: No database URL found in production!",
        );
        console.error(
          "[AgentRuntime] Expected one of: DATABASE_POSTGRES_URL, DATABASE_URL_UNPOOLED, POSTGRES_URL, POSTGRES_DATABASE_URL",
        );
        throw new Error(
          "Database connection failed: No database URL configured in production. " +
            "Vercel Neon Storage should provide DATABASE_POSTGRES_URL automatically. " +
            "Please check your Vercel project settings.",
        );
      }
    }

    // Validate URL format (basic check)
    if (postgresUrl && !postgresUrl.includes("localhost")) {
      const isValidFormat =
        postgresUrl.startsWith("postgres://") ||
        postgresUrl.startsWith("postgresql://");
      if (!isValidFormat) {
        console.warn(
          "[AgentRuntime] WARNING: Database URL doesn't start with postgres:// or postgresql://",
        );
      }
      // Check for hostname (basic validation)
      const url = new URL(
        postgresUrl.replace(/^postgres(ql)?:\/\//, "http://"),
      );
      if (!url.hostname || url.hostname === "") {
        throw new Error(
          "Database connection failed: Invalid database URL format (missing hostname)",
        );
      }
    }

    console.log(
      `[AgentRuntime] Database config: ${postgresUrl.includes("localhost") ? "localhost:" + dbPort : "remote (Vercel/Neon)"}`,
    );

    // Use the existing agent ID from DB (b850bc30-45f8-0041-a00a-83df46d8555d)
    const RUNTIME_AGENT_ID = "b850bc30-45f8-0041-a00a-83df46d8555d" as UUID;
    this.runtime = new AgentRuntime({
      ...agent,
      agentId: RUNTIME_AGENT_ID,
      settings: {
        GROQ_API_KEY: process.env.GROQ_API_KEY || "",
        SMALL_GROQ_MODEL: process.env.SMALL_GROQ_MODEL || "qwen/qwen3-32b",
        LARGE_GROQ_MODEL:
          process.env.LARGE_GROQ_MODEL || "moonshotai/kimi-k2-instruct-0905",
        POSTGRES_URL: postgresUrl,
        ...agent.character.settings,
      },
    } as any);

    // Cache globally for reuse in warm container
    globalState.__elizaRuntime = this.runtime;

    // Ensure runtime has a logger with all required methods
    if (!this.runtime.logger || !this.runtime.logger.log) {
      this.runtime.logger = {
        log: console.log.bind(console),
        info: console.info.bind(console),
        warn: console.warn.bind(console),
        error: console.error.bind(console),
        debug: console.debug.bind(console),
        success: (message: string) => console.log(`✓ ${message}`),
        notice: console.info.bind(console),
      } as any;
    }

    // Ensure SQL plugin built-in tables exist (idempotent)
    await this.ensureBuiltInTables();

    // Initialize runtime - this calls ensureAgentExists internally (runtime.ts:405)
    // which creates both the agent record AND its entity record
    await this.runtime.initialize();

    // Log registered services
    const services = Array.from(this.runtime.getAllServices().keys());
    console.log("[AgentRuntime] Registered services:", services);

    return this.runtime;
  }

  private async ensureBuiltInTables(): Promise<void> {
    if (this.hasRunMigrations || globalState.__elizaMigrationsRan) return;

    this.hasRunMigrations = true;
    globalState.__elizaMigrationsRan = true;

    // Database adapter and migrations are handled by @elizaos/plugin-sql during runtime.initialize()
    // Quotes and user sessions are stored via runtime.getCache/setCache
    console.log("[AgentRuntime] Using Eliza cache system for quote storage");
  }

  // Helper method to handle messages
  public async handleMessage(
    roomId: string,
    entityId: string,
    content: { text?: string; attachments?: any[] },
  ): Promise<Memory> {
    const runtime = await this.getRuntime();

    // Ensure room and entity connection (follows Eliza's ensureConnection pattern)
    const entityUuid = stringToUuid(entityId) as UUID;
    await runtime.ensureConnection({
      entityId: entityUuid,
      roomId: roomId as UUID,
      worldId: stringToUuid("otc-desk-world"),
      source: "web",
      type: ChannelType.DM,
      channelId: roomId,
      serverId: "otc-desk-server",
      userName: entityId,
    } as any);

    // Create user message
    const userMessage: Memory = {
      roomId: roomId as UUID,
      entityId: entityUuid,
      agentId: runtime.agentId as UUID,
      content: {
        text: content.text || "",
        attachments: content.attachments || [],
      },
    };
    // Emit MESSAGE_RECEIVED and delegate handling to plugins
    // Note: The message handler plugin (otcDeskPlugin) is responsible for:
    // 1. Saving the user message to memory
    // 2. Generating the agent response
    // 3. Saving the agent response to memory
    // The callback is just for notification - do NOT create memory here
    // to avoid duplicate agent messages.
    await runtime.emitEvent(EventType.MESSAGE_RECEIVED, {
      runtime,
      message: {
        id: userMessage.id,
        content: {
          text: content.text || "",
          attachments: content.attachments || [],
        },
        entityId: stringToUuid(entityId) as UUID,
        agentId: runtime.agentId,
        roomId: roomId,
        createdAt: Date.now(),
      },
      callback: async () => {
        // Callback is for notification only - memory is saved by the message handler
        console.log("[AgentRuntime] Message handler completed");
      },
    });

    return userMessage;
  }
}

// Export singleton instance
const agentRuntime = AgentRuntimeManager.getInstance();

} // namespace elizaos
