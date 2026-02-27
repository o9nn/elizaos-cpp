#!/usr/bin/env bun
/**
 * RS-SDK Bot Runner using ElizaOS Framework
 *
 * This script creates an Eliza agent with the RS-SDK plugin that runs autonomously.
 * All processing goes through runtime.messageService.handleMessage, which runs
 * the full Eliza pipeline: providers → LLM → actions → evaluators.
 *
 * The agent:
 * - Connects to the RuneScape bot via RS-SDK
 * - Gets world state via the botStateProvider (automatically included in composeState)
 * - Makes decisions via the LLM
 * - Executes actions via the registered Eliza actions (CHOP_TREE, ATTACK_NPC, etc.)
 */

import { config } from "dotenv";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import { v4 as uuidv4 } from "uuid";
import {
  AgentRuntime,
  ChannelType,
  type Character,
  createMessageMemory,
  type Plugin,
  type Provider,
  type ProviderResult,
  stringToUuid,
  type UUID,
  type Content,
  type Memory,
} from "@elizaos/core";
import type { IDatabaseAdapter, IAgentRuntime, State } from "@elizaos/core";

import { rsSdkPlugin, rsSdkActions } from "./plugin/index.js";
import { addEventLogEntry } from "./plugin/providers/index.js";
import { setCurrentLlmResponse, clearCurrentLlmResponse } from "./plugin/shared-state.js";

// Load environment variables from .env file
const envPaths = [".env", "../.env", resolve(process.cwd(), ".env")];
for (const envPath of envPaths) {
  if (existsSync(envPath)) {
    config({ path: envPath });
    console.log(`[RS-SDK] Loaded environment from: ${envPath}`);
    break;
  }
}

// Configuration from environment
const BOT_NAME = process.env.BOT_NAME || process.env.RS_SDK_BOT_NAME || "testbot";
const AUTONOMY_INTERVAL_MS = parseInt(process.env.AUTONOMY_INTERVAL_MS || "30000", 10);
const MAX_STEPS = parseInt(process.env.AUTONOMY_MAX_STEPS || "1000000", 10);

// LLM Provider Detection
interface LLMProvider {
  name: string;
  envKey: string;
  importPath: string;
  exportName: string;
}

const LLM_PROVIDERS: LLMProvider[] = [
  {
    name: "Groq (Qwen3)",
    envKey: "GROQ_API_KEY",
    importPath: "@elizaos/plugin-groq",
    exportName: "groqPlugin",
  },
  {
    name: "OpenAI",
    envKey: "OPENAI_API_KEY",
    importPath: "@elizaos/plugin-openai",
    exportName: "openaiPlugin",
  },
  {
    name: "Anthropic (Claude)",
    envKey: "ANTHROPIC_API_KEY",
    importPath: "@elizaos/plugin-anthropic",
    exportName: "anthropicPlugin",
  },
];

function hasValidApiKey(envKey: string): boolean {
  const value = process.env[envKey];
  return typeof value === "string" && value.trim().length > 0;
}

/**
 * Smoke-test an LLM API key by hitting a zero-cost endpoint.
 * Returns { ok: true } or { ok: false, status, message } so we can
 * print a clear error and skip to the next provider (or bail entirely).
 */
interface KeyValidationResult {
  ok: boolean;
  status?: number;
  message?: string;
}

async function validateApiKey(envKey: string, apiKey: string): Promise<KeyValidationResult> {
  try {
    let response: Response;

    if (envKey === "GROQ_API_KEY") {
      // GET /models — free, returns 200 for valid keys
      response = await fetch("https://api.groq.com/openai/v1/models", {
        headers: { Authorization: `Bearer ${apiKey}` },
      });
    } else if (envKey === "OPENAI_API_KEY") {
      // GET /models — free, returns 200 for valid keys
      response = await fetch("https://api.openai.com/v1/models", {
        headers: { Authorization: `Bearer ${apiKey}` },
      });
    } else if (envKey === "ANTHROPIC_API_KEY") {
      // POST /messages with empty body — valid key → 400, invalid key → 401/403
      // Zero tokens consumed either way.
      response = await fetch("https://api.anthropic.com/v1/messages", {
        method: "POST",
        headers: {
          "x-api-key": apiKey,
          "anthropic-version": "2023-06-01",
          "content-type": "application/json",
        },
        body: "{}",
      });
      // 400 = key is valid but request body is bad (expected). Anything else is an auth failure.
      if (response.status === 400) {
        return { ok: true };
      }
    } else {
      // Unknown provider — skip validation, assume ok
      return { ok: true };
    }

    if (response!.ok || response!.status === 400) {
      return { ok: true };
    }

    return {
      ok: false,
      status: response!.status,
      message: response!.status === 401
        ? "Invalid API key (401 Unauthorized)"
        : response!.status === 403
          ? "API key lacks permissions (403 Forbidden)"
          : `Unexpected status ${response!.status}`,
    };
  } catch (err) {
    return {
      ok: false,
      message: `Network error: ${err instanceof Error ? err.message : String(err)}`,
    };
  }
}

async function loadLLMPlugin(): Promise<{ plugin: Plugin; providerName: string } | null> {
  const triedProviders: string[] = [];

  for (const provider of LLM_PROVIDERS) {
    if (!hasValidApiKey(provider.envKey)) continue;

    triedProviders.push(provider.name);
    const apiKey = (process.env[provider.envKey] as string).trim();

    // Smoke-test the key before loading the plugin
    console.log(`[RS-SDK] Validating ${provider.name} API key...`);
    const validation = await validateApiKey(provider.envKey, apiKey);

    if (!validation.ok) {
      const masked = apiKey.length > 12
        ? `${apiKey.slice(0, 6)}...${apiKey.slice(-4)}`
        : "***";
      console.error("");
      console.error("╔══════════════════════════════════════════════════════════╗");
      console.error(`║  ❌  ${provider.name} API KEY FAILED                     `);
      console.error("╠══════════════════════════════════════════════════════════╣");
      console.error(`║  Key:    ${masked}`);
      console.error(`║  Error:  ${validation.message}`);
      console.error(`║  Env:    ${provider.envKey}`);
      console.error("║                                                          ");
      console.error("║  Fix: update the key in your .env file and restart.      ");
      console.error("╚══════════════════════════════════════════════════════════╝");
      console.error("");
      continue;
    }

    console.log(`[RS-SDK] ✅ ${provider.name} API key is valid`);

    try {
      const module = await import(provider.importPath);
      const plugin = module[provider.exportName] || module.default;
      if (plugin) {
        console.log(`[RS-SDK] ✅ Using LLM provider: ${provider.name}`);
        return { plugin, providerName: provider.name };
      }
    } catch (error) {
      console.warn(`[RS-SDK] ⚠️ Failed to load ${provider.name} plugin:`, error);
      continue;
    }
  }

  // All providers failed — give a clear summary
  console.error("");
  console.error("╔══════════════════════════════════════════════════════════╗");
  console.error("║  ❌  NO WORKING LLM PROVIDER                            ");
  console.error("╠══════════════════════════════════════════════════════════╣");
  if (triedProviders.length > 0) {
    console.error(`║  Tried: ${triedProviders.join(", ")}`);
    console.error("║  All API keys failed validation.                         ");
  } else {
    console.error("║  No API keys found in environment.                       ");
  }
  console.error("║                                                          ");
  console.error("║  Set one of these in your .env file:                     ");
  console.error("║    GROQ_API_KEY=gsk_...                                  ");
  console.error("║    OPENAI_API_KEY=sk-...                                 ");
  console.error("║    ANTHROPIC_API_KEY=sk-ant-...                          ");
  console.error("╚══════════════════════════════════════════════════════════╝");
  console.error("");

  return null;
}

// Database adapter
async function loadDatabasePlugin(): Promise<{
  plugin: Plugin;
  createAdapter: (config: { dataDir?: string }, agentId: UUID) => IDatabaseAdapter;
} | null> {
  try {
    const sqlModule = await import("@elizaos/plugin-sql");
    const plugin = sqlModule.default || sqlModule.plugin;
    const createDatabaseAdapter = sqlModule.createDatabaseAdapter;
    console.log("[RS-SDK] ✅ SQL/PGlite database plugin loaded");
    return { plugin, createAdapter: createDatabaseAdapter };
  } catch (error) {
    console.error("[RS-SDK] ❌ Failed to load @elizaos/plugin-sql:", error);
    return null;
  }
}

async function main(): Promise<void> {
  console.log("═══════════════════════════════════════════════════════════");
  console.log("  RS-SDK Bot Runner (ElizaOS Framework)");
  console.log("═══════════════════════════════════════════════════════════");
  console.log(`  Bot: ${BOT_NAME}`);
  console.log(`  Autonomy Interval: ${AUTONOMY_INTERVAL_MS}ms`);
  console.log(`  Max Steps: ${MAX_STEPS}`);
  console.log("═══════════════════════════════════════════════════════════\n");

  // Load LLM plugin (validates API keys with a smoke test before loading)
  const llmResult = await loadLLMPlugin();
  if (!llmResult) {
    process.exit(1);
  }

  // Load database plugin
  const dbResult = await loadDatabasePlugin();
  if (!dbResult) {
    console.error("❌ Database plugin not available");
    process.exit(1);
  }

  // Create character with appropriate settings
  const character: Character = {
    name: `RS-Bot-${BOT_NAME}`,
    bio: [
      `You are an autonomous RuneScape bot controller. You explore the game world, train skills, complete quests, and make strategic decisions.`,
      `You have 5 providers giving you context: RS_SDK_MAP_AREA (where you are + how to travel), RS_SDK_WORLD_KNOWLEDGE (nearest bank + training spots), RS_SDK_GOALS (what to do next), RS_SDK_BOT_STATE (current game state), RS_SDK_ACTIONS (available actions).`,
      `Follow the GOALS provider priorities. Handle IMMEDIATE goals first (low HP, full inventory), then SHORT_TERM (skill balancing), then MEDIUM_TERM (quests, new skills), then EXPLORE (new areas).`,
      `Talk to NPCs with unique names to discover quests. Navigate dialog with NAVIGATE_DIALOG. Travel to new areas using coordinates from the MAP_AREA provider. Don't stay in one area forever.`,
    ],
    settings: {
      // Bot configuration
      RS_SDK_BOT_NAME: BOT_NAME,
      // Disable response checks for autonomous operation
      CHECK_SHOULD_RESPOND: "false",
      secrets: {
        GROQ_API_KEY: process.env.GROQ_API_KEY,
        OPENAI_API_KEY: process.env.OPENAI_API_KEY,
        ANTHROPIC_API_KEY: process.env.ANTHROPIC_API_KEY,
      },
      // Groq model config - use Qwen3-32B for speed (preferred)
      GROQ_LARGE_MODEL: process.env.GROQ_LARGE_MODEL || "qwen/qwen3-32b",
      GROQ_SMALL_MODEL: process.env.GROQ_SMALL_MODEL || "qwen/qwen3-32b",
      // OpenAI model config (fallback)
      OPENAI_LARGE_MODEL: process.env.OPENAI_LARGE_MODEL || "gpt-5",
      OPENAI_SMALL_MODEL: process.env.OPENAI_SMALL_MODEL || "gpt-5-mini",
      // Anthropic (last fallback)
    },
    style: {
      all: [
        "You are a RuneScape player who explores, trains skills, and completes quests.",
        "Always follow the GOALS provider priorities. Handle IMMEDIATE goals first.",
        "Don't repeat the same action endlessly. If something fails 3+ times, try a different approach.",
        "Explore new areas regularly. Use MAP_AREA travel coordinates to visit new towns.",
        "Talk to named NPCs to discover quests. Use NAVIGATE_DIALOG to progress through quest conversations.",
        "Balance your skills - don't train only one thing forever.",
      ],
    },
  };

  // Create a custom ACTIONS provider that tells the LLM about available RS-SDK actions
  const rsActionsProvider: Provider = {
    name: "RS_SDK_ACTIONS",
    description: "Lists available RS-SDK bot control actions",
    dynamic: false,
    position: 5, // Early position so LLM sees actions first
    get: async (_runtime: IAgentRuntime, _message: Memory, _state?: State): Promise<ProviderResult> => {
      const actionDescriptions = rsSdkActions.map(action => {
        let paramDesc = "";
        if (action.parameters && action.parameters.length > 0) {
          const params = action.parameters.map(p => `${p.name}: ${p.description}`).join(", ");
          paramDesc = ` (Parameters: ${params})`;
        }
        return `- ${action.name}: ${action.description}${paramDesc}`;
      }).join("\n");

      const text = `# Available Bot Actions

You can execute these actions to control the RuneScape bot:

${actionDescriptions}

## How to Use Actions
When you want to perform an action, use:
<actions>ACTION_NAME</actions>

For actions with parameters, include a <params> block with the action name as wrapper:
<params>
  <ACTION_NAME>
    <paramName>value</paramName>
  </ACTION_NAME>
</params>

### Example: Walk to coordinates
<actions>WALK_TO</actions>
<params><WALK_TO><x>3200</x><z>3200</z></WALK_TO></params>

### Example: Attack an NPC
<actions>ATTACK_NPC</actions>
<params><ATTACK_NPC><npcName>Goblin</npcName></ATTACK_NPC></params>

## Rules
- Choose ONE action per response from the list above
- Do NOT use REPLY, IGNORE, or any action not in the list above
- ALWAYS include <params> when the action requires parameters
- The bot state (position, inventory, nearby entities) is shown in RS_SDK_BOT_STATE`;

      return {
        text,
        values: {
          actionNames: rsSdkActions.map(a => a.name).join(", "),
          availableActions: JSON.stringify(rsSdkActions.map(a => ({
            name: a.name,
            description: a.description,
            parameters: a.parameters,
          }))),
        },
      };
    },
  };

  // Create a plugin to hold our custom provider
  const rsActionsPlugin: Plugin = {
    name: "rs-sdk-actions-provider",
    description: "Provides RS-SDK actions list to LLM",
    providers: [rsActionsProvider],
  };

  // Load ElizaOS plugins by importing individual components (avoids Bun segfault on full plugin imports)
  const optionalPlugins: Plugin[] = [];
  const pluginsRoot = resolve(process.cwd(), "../plugins");

  // --- Scratchpad Plugin (file-based notes/memory) ---
  try {
    const { scratchpadProvider } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/providers/scratchpad.ts`);
    const { scratchpadWriteAction } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/actions/write.ts`);
    const { scratchpadReadAction } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/actions/read.ts`);
    const { scratchpadSearchAction } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/actions/search.ts`);
    const { scratchpadListAction } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/actions/list.ts`);
    const { scratchpadAppendAction } = await import(`${pluginsRoot}/plugin-scratchpad/typescript/actions/append.ts`);
    const scratchpadPlugin: Plugin = {
      name: "scratchpad",
      description: "File-based memory for persistent notes across sessions",
      providers: [scratchpadProvider],
      actions: [scratchpadWriteAction, scratchpadReadAction, scratchpadSearchAction, scratchpadListAction, scratchpadAppendAction],
    };
    optionalPlugins.push(scratchpadPlugin);
    console.log("[RS-SDK] ✅ Loaded plugin: Scratchpad (notes/memory)");
  } catch (e) {
    console.log(`[RS-SDK] ⚠️ Scratchpad plugin not available: ${e instanceof Error ? e.message : "unknown"}`);
  }

  // --- Experience Plugin (self-learning from past actions) ---
  try {
    const { experienceProvider } = await import(`${pluginsRoot}/plugin-experience/typescript/providers/experienceProvider.ts`);
    const { recordExperienceAction } = await import(`${pluginsRoot}/plugin-experience/typescript/actions/record-experience.ts`);
    const { experienceEvaluator } = await import(`${pluginsRoot}/plugin-experience/typescript/evaluators/experienceEvaluator.ts`);
    const experiencePlugin: Plugin = {
      name: "experience",
      description: "Records and recalls transferable agent learnings",
      providers: [experienceProvider],
      actions: [recordExperienceAction],
      evaluators: [experienceEvaluator],
    };
    optionalPlugins.push(experiencePlugin);
    console.log("[RS-SDK] ✅ Loaded plugin: Experience (self-learning)");
  } catch (e) {
    console.log(`[RS-SDK] ⚠️ Experience plugin not available: ${e instanceof Error ? e.message : "unknown"}`);
  }

  // --- Todo Plugin ---
  // The Todo plugin's schema migration has compatibility issues with PGlite in-memory DB.
  // Our built-in RS_SDK_GOALS provider handles goal/task tracking instead.
  // To enable Todo, run with a persistent Postgres database.
  console.log("[RS-SDK] ✅ Todo: using built-in RS_SDK_GOALS provider for task tracking");

  // --- Goals Plugin (requires spec files - use our custom goalsProvider instead) ---
  // The ElizaOS goals plugin crashes under Bun due to spec file resolution.
  // Our RS_SDK_GOALS provider already provides goal tracking functionality.
  console.log("[RS-SDK] ✅ Goals: using built-in RS_SDK_GOALS provider");

  // Assemble plugins
  const plugins: Plugin[] = [rsActionsPlugin, llmResult.plugin, dbResult.plugin, rsSdkPlugin, ...optionalPlugins];

  console.log("[RS-SDK] Creating AgentRuntime...");

  // Create runtime
  const runtime = new AgentRuntime({
    character,
    plugins,
    checkShouldRespond: false, // Always respond in autonomous mode
  });

  // Initialize database adapter manually (workaround for ElizaOS 1.7.x)
  console.log("[RS-SDK] Initializing database adapter...");
  const dbAdapter = dbResult.createAdapter({ dataDir: "memory://" }, runtime.agentId);
  runtime.registerDatabaseAdapter(dbAdapter);
  await dbAdapter.init();

  // Run migrations
  if (dbAdapter.runPluginMigrations) {
    const pluginsWithSchemas = plugins
      .filter((p) => p?.schema)
      .map((p) => ({
        name: p.name,
        schema: p.schema as Record<string, unknown>,
      }));
    if (dbResult.plugin?.schema) {
      pluginsWithSchemas.unshift({
        name: dbResult.plugin.name,
        schema: dbResult.plugin.schema as Record<string, unknown>,
      });
    }
    await dbAdapter.runPluginMigrations(pluginsWithSchemas, { verbose: false });
  }
  console.log("[RS-SDK] ✅ Database initialized");

  // Initialize runtime
  console.log("[RS-SDK] Initializing runtime...");
  await runtime.initialize({ skipMigrations: true });
  console.log("[RS-SDK] ✅ Runtime initialized");
  console.log(`[RS-SDK] LLM Provider: ${llmResult.providerName}`);
  if (llmResult.providerName.includes("Groq")) {
    console.log(`[RS-SDK] Model (large): ${runtime.getSetting("GROQ_LARGE_MODEL") || "(default)"}`);
    console.log(`[RS-SDK] Model (small): ${runtime.getSetting("GROQ_SMALL_MODEL") || "(default)"}`);
  } else if (llmResult.providerName.includes("OpenAI")) {
    console.log(`[RS-SDK] Model (large): ${runtime.getSetting("OPENAI_LARGE_MODEL") || "(default)"}`);
    console.log(`[RS-SDK] Model (small): ${runtime.getSetting("OPENAI_SMALL_MODEL") || "(default)"}`);
  } else {
    console.log(`[RS-SDK] Using default models for ${llmResult.providerName}`);
  }

  // Check if messageService is available
  if (!runtime.messageService) {
    console.error("[RS-SDK] ❌ messageService not available on runtime");
    process.exit(1);
  }
  console.log("[RS-SDK] ✅ messageService available");

  // Log registered actions
  const actionNames = runtime.actions.map((a) => a.name).join(", ");
  console.log(`[RS-SDK] Registered actions (${runtime.actions.length}): ${actionNames}`);

  // Log registered providers
  const providerNames = runtime.providers.map((p) => p.name).join(", ");
  console.log(`[RS-SDK] Registered providers (${runtime.providers.length}): ${providerNames}`);
  
  // Debug: check if ACTIONS provider exists
  const actionsProvider = runtime.providers.find(p => p.name === "ACTIONS");
  console.log(`[RS-SDK] ACTIONS provider: ${actionsProvider ? "found" : "NOT FOUND"}`);
  
  // Debug: list all providers with details
  console.log("[RS-SDK] Provider details:");
  for (const p of runtime.providers) {
    console.log(`  - ${p.name} (dynamic=${p.dynamic ?? false}, private=${p.private ?? false})`);
  }

  // Setup autonomous room and context
  const userId = uuidv4() as UUID;
  const autonomousRoomId = stringToUuid("rs-autonomous-room");
  const autonomousWorldId = stringToUuid("rs-autonomous-world");

  // Ensure the autonomous room exists
  await runtime.ensureConnection({
    entityId: userId,
    roomId: autonomousRoomId,
    worldId: autonomousWorldId,
    userName: "Autonomy",
    source: "autonomous-loop",
    channelId: "autonomous-room",
    serverId: "autonomous",
    type: ChannelType.DM,
  } as Parameters<typeof runtime.ensureConnection>[0]);

  // Intercept processActions to capture LLM response text before action handlers run.
  // This allows action handlers to read the LLM's response via getCurrentLlmResponse()
  // for parameter extraction (e.g., coordinates from "walk to (3214, 3245)").
  const originalProcessActions = runtime.processActions.bind(runtime);
  (runtime as Record<string, unknown>).processActions = async (
    msg: Memory,
    responses: Memory[],
    st?: State,
    cb?: (content: Content) => Promise<Memory[]>,
    opts?: { onStreamChunk?: (chunk: string, messageId?: string) => Promise<void> },
  ): Promise<void> => {
    // Extract LLM response text from the response messages
    for (const resp of responses) {
      const text = (resp.content as { text?: string })?.text;
      if (text && typeof text === "string") {
        setCurrentLlmResponse(text);
        break;
      }
    }
    return originalProcessActions(msg, responses, st, cb, opts);
  };

  console.log("\n[RS-SDK] 🚀 Starting autonomous loop!");
  console.log("[RS-SDK] Each cycle: providers (incl. RS_SDK_BOT_STATE with full event log) → LLM → actions → evaluators");
  console.log("[RS-SDK] Press Ctrl+C to stop.\n");

  // Graceful shutdown handler
  let running = true;
  process.on("SIGINT", async () => {
    console.log("\n[RS-SDK] Shutting down...");
    running = false;
    await runtime.stop();
    process.exit(0);
  });
  process.on("SIGTERM", async () => {
    console.log("\n[RS-SDK] Shutting down...");
    running = false;
    await runtime.stop();
    process.exit(0);
  });

  // Main autonomous loop
  for (let step = 1; step <= MAX_STEPS && running; step++) {
    const startTime = Date.now();

    try {
      // The autonomous prompt is intentionally concise.
      // All context is injected by providers:
      //   - RS_SDK_BOT_STATE: full world state, inventory, HP, alerts, event history
      //   - RS_SDK_ACTIONS: available actions with parameters
      const autonomyPrompt = `Autonomous step ${step}. Review ALL providers: GOALS (priorities), MAP_AREA (location + travel), WORLD_KNOWLEDGE (banks + training), BOT_STATE (current state + alerts). Follow goal priorities: IMMEDIATE first, then SHORT_TERM, MEDIUM_TERM, EXPLORE. If stuck doing the same thing, travel to a new area or talk to an NPC for a quest.`;

      // Create the message for the Eliza pipeline
      const message = createMessageMemory({
        id: uuidv4() as UUID,
        entityId: userId,
        roomId: autonomousRoomId,
        content: {
          text: autonomyPrompt,
          source: "autonomous-loop",
          metadata: {
            type: "autonomous-prompt",
            isAutonomous: true,
            channelId: "autonomous",
            timestamp: Date.now(),
          },
        },
      });

      // Process through messageService.handleMessage - full Eliza pipeline:
      // 1. Saves message to memory
      // 2. Composes state via providers (RS_SDK_BOT_STATE with world state + event log)
      // 3. LLM generates response with action selection
      // 4. Actions are validated and executed
      // 5. Evaluators run
      clearCurrentLlmResponse();
      let responseText = "";
      const executedActions: string[] = [];
      const actionResults: string[] = [];

      const result = await runtime.messageService.handleMessage(
        runtime,
        message,
        async (content: Content): Promise<Memory[]> => {
          if (content?.text) {
            responseText += content.text;
          }
          if (content?.action) {
            executedActions.push(content.action);
            // Log to the shared event log that the provider reads
            const isSuccess = !content.text?.toLowerCase().includes("failed");
            addEventLogEntry({
              step,
              timestamp: Date.now(),
              action: content.action,
              result: isSuccess ? "success" : "failure",
              message: (content.text || "").substring(0, 120),
            });
            actionResults.push(`${content.action}: ${content.text?.substring(0, 80) || "(no text)"}`);
          }
          return [];
        },
      );

      // Also capture pipeline-level actions
      if (result.responseContent?.actions?.length) {
        for (const action of result.responseContent.actions) {
          if (!executedActions.includes(action)) {
            addEventLogEntry({
              step,
              timestamp: Date.now(),
              action,
              result: "success",
              message: `Pipeline action: ${action}`,
            });
          }
        }
      }

      // Console logging
      console.log(`\n[Step ${step}] Mode: ${result.mode || "unknown"}, Responded: ${result.didRespond}`);
      
      if (executedActions.length > 0) {
        console.log(`         🎯 Actions: ${executedActions.join(", ")}`);
      }
      
      if (responseText) {
        const truncated = responseText.length > 200 
          ? responseText.substring(0, 200) + "..." 
          : responseText;
        console.log(`         💬 Response: ${truncated}`);
      }

      if (result.responseContent?.actions?.length) {
        console.log(`         ✅ Pipeline: ${result.responseContent.actions.join(", ")}`);
      }

      // No pause - immediately start calculating the next step

    } catch (error) {
      console.error(`[Step ${step}] ❌ Error:`, error);
      addEventLogEntry({
        step,
        timestamp: Date.now(),
        action: "ERROR",
        result: "failure",
        message: error instanceof Error ? error.message : "Unknown error",
      });
    }
  }

  console.log("\n[RS-SDK] ✅ Autonomous loop completed");
  await runtime.stop();
}

// Run
main().catch((error) => {
  console.error("Fatal error:", error);
  process.exit(1);
});
