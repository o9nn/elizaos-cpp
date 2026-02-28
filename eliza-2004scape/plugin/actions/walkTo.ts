import {
  type Action,
  type ActionExample,
  type ActionResult,
  type HandlerCallback,
  type HandlerOptions,
  type IAgentRuntime,
  logger,
  type Memory,
  type State,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { getCurrentLlmResponse } from "../shared-state";
import { BANK_LOCATIONS, findNearestBank } from "../data/banks";
import { GAME_AREAS } from "../data/areas";
import { SKILL_TRAINING } from "../data/training";

// ---------------------------------------------------------------------------
// Named-location resolver — maps natural-language destination text to coords
// ---------------------------------------------------------------------------

interface ResolvedLocation {
  x: number;
  z: number;
  label: string;
}

/** Try to resolve a destination name from free text to known coordinates. */
function resolveNamedDestination(text: string, botX?: number, botZ?: number): ResolvedLocation | null {
  const lower = text.toLowerCase();

  // --- Special keyword: "bank" / "nearest bank" ---
  if (/\bbank\b/.test(lower)) {
    // If we have a specific bank name, try exact match first
    for (const bank of BANK_LOCATIONS) {
      if (lower.includes(bank.name.toLowerCase()) || lower.includes(bank.area.toLowerCase() + " bank")) {
        return { x: bank.x, z: bank.z, label: bank.name };
      }
    }
    // Otherwise find nearest bank if we know the bot's position
    if (botX !== undefined && botZ !== undefined) {
      const { bank } = findNearestBank(botX, botZ);
      return { x: bank.x, z: bank.z, label: `${bank.name} (nearest)` };
    }
    // Default to Draynor Village Bank as a safe general-purpose fallback
    const draynor = BANK_LOCATIONS.find(b => b.area === "Draynor Village");
    if (draynor) return { x: draynor.x, z: draynor.z, label: draynor.name };
  }

  // --- Match game area names (e.g. "lumbridge", "varrock", "al kharid") ---
  for (const area of GAME_AREAS) {
    if (lower.includes(area.name.toLowerCase())) {
      // Use center of area bounds as destination
      const cx = Math.round((area.bounds.minX + area.bounds.maxX) / 2);
      const cz = Math.round((area.bounds.minZ + area.bounds.maxZ) / 2);
      return { x: cx, z: cz, label: area.name };
    }
  }

  // --- Match adjacent-area walk targets (e.g. "go to edgeville from varrock") ---
  for (const area of GAME_AREAS) {
    for (const adj of area.adjacentAreas) {
      if (lower.includes(adj.name.toLowerCase())) {
        return { x: adj.walkCoords.x, z: adj.walkCoords.z, label: adj.name };
      }
    }
  }

  // --- Match training spot keywords (e.g. "chickens", "cows", "willows") ---
  const trainingKeywords: { pattern: RegExp; skill: string; method: string }[] = [
    { pattern: /\bchicken/, skill: "Combat", method: "Chickens" },
    { pattern: /\bcow/, skill: "Combat", method: "Cows" },
    { pattern: /\bgoblin/, skill: "Combat", method: "Goblins" },
    { pattern: /\bhill giant/, skill: "Combat", method: "Hill Giants" },
    { pattern: /\bwillow/, skill: "Woodcutting", method: "Willow" },
    { pattern: /\boak/, skill: "Woodcutting", method: "Oak" },
    { pattern: /\byew/, skill: "Woodcutting", method: "Yew" },
    { pattern: /\blobster/, skill: "Fishing", method: "Lobster" },
    { pattern: /\btrout|salmon|fly fish/, skill: "Fishing", method: "Fly fishing" },
    { pattern: /\bshrimp|net fish/, skill: "Fishing", method: "Small net" },
    { pattern: /\bmine|mining|copper|tin|iron ore/, skill: "Mining", method: "" },
    { pattern: /\bfurnace|smelt/, skill: "Smithing", method: "Smelt" },
    { pattern: /\banvil|smith/, skill: "Smithing", method: "Smith" },
    { pattern: /\bpickpocket/, skill: "Thieving", method: "" },
  ];

  for (const kw of trainingKeywords) {
    if (kw.pattern.test(lower)) {
      for (const skillData of SKILL_TRAINING) {
        if (!skillData.skill.toLowerCase().includes(kw.skill.toLowerCase())) continue;
        for (const spot of skillData.spots) {
          if (kw.method === "" || spot.method.toLowerCase().includes(kw.method.toLowerCase())) {
            return { x: spot.coords.x, z: spot.coords.z, label: `${spot.method} at ${spot.location}` };
          }
        }
      }
    }
  }

  // --- Match specific bank-area names without the word "bank" (e.g. "al kharid") ---
  for (const bank of BANK_LOCATIONS) {
    if (lower.includes(bank.area.toLowerCase())) {
      // They said the area name — resolve to the area center, not the bank
      const area = GAME_AREAS.find(a => a.name.toLowerCase() === bank.area.toLowerCase());
      if (area) {
        const cx = Math.round((area.bounds.minX + area.bounds.maxX) / 2);
        const cz = Math.round((area.bounds.minZ + area.bounds.maxZ) / 2);
        return { x: cx, z: cz, label: area.name };
      }
    }
  }

  return null;
}

/** Build a help string listing known named destinations. */
function knownDestinationsHelp(): string {
  const areas = GAME_AREAS.map(a => a.name).join(", ");
  const banks = BANK_LOCATIONS.map(b => b.name).join(", ");
  return (
    `I can walk to named destinations or exact coordinates. Known locations:\n` +
    `- **Areas**: ${areas}\n` +
    `- **Banks**: ${banks}\n` +
    `- **Training spots**: chickens, cows, willows, oaks, yews, mine, furnace, anvil, shrimp, trout, lobster, pickpocket\n` +
    `- **Coordinates**: provide x,z like "walk to 3200, 3200"`
  );
}

// ---------------------------------------------------------------------------
// Action definition
// ---------------------------------------------------------------------------

const walkToAction: Action = {
  name: "WALK_TO",
  similes: ["MOVE_TO", "GO_TO", "WALK"],
  description:
    "Walk to specific world coordinates (x, z) or a named destination (e.g. 'bank', 'lumbridge', 'chickens', 'varrock west bank'). If coordinates are provided, uses them directly. If a location name is given instead, resolves it to known coordinates automatically. CRITICAL: You MUST use OPEN_DOOR to open any gates/doors BEFORE walking through fenced areas (cow field gate, chicken coop gate, etc.) - the pathfinder CANNOT handle closed gates and will get stuck! For long distances (>30 tiles), walk in segments using multiple WALK_TO calls with intermediate waypoints. WARNING: Avoid walking through (3220, 3220) area - aggressive Dark Wizards there will kill low-level characters.",
  parameters: [
    {
      name: "x",
      description: "The X coordinate to walk to (world coordinate) — optional if a named destination is given",
      required: false,
      schema: { type: "number" },
    },
    {
      name: "z",
      description: "The Z coordinate to walk to (world coordinate) — optional if a named destination is given",
      required: false,
      schema: { type: "number" },
    },
    {
      name: "destination",
      description: "Named destination (e.g. 'bank', 'lumbridge', 'chickens', 'varrock west bank') — optional if x,z are given",
      required: false,
      schema: { type: "string" },
    },
  ],
  examples: [
    [
      {
        name: "{{user1}}",
        content: { text: "Walk to coordinates 3200, 3200" },
      },
      {
        name: "{{agentName}}",
        content: {
          text: "Walking to coordinates (3200, 3200).",
          action: "WALK_TO",
        },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Go to the bank" },
      },
      {
        name: "{{agentName}}",
        content: {
          text: "Walking to the nearest bank.",
          action: "WALK_TO",
        },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Walk to Lumbridge" },
      },
      {
        name: "{{agentName}}",
        content: {
          text: "Heading to Lumbridge now.",
          action: "WALK_TO",
        },
      },
    ],
    [
      {
        name: "{{user1}}",
        content: { text: "Go to the chickens" },
      },
      {
        name: "{{agentName}}",
        content: {
          text: "Walking to the chicken area north of Lumbridge.",
          action: "WALK_TO",
        },
      },
    ],
  ] as ActionExample[][],

  validate: async (runtime: IAgentRuntime): Promise<boolean> => {
    const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
    return Boolean(service);
  },

  handler: async (
    runtime: IAgentRuntime,
    message: Memory,
    state: State | undefined,
    options?: HandlerOptions,
    callback?: HandlerCallback
  ): Promise<ActionResult | undefined> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.error("RS-SDK service not found");
        if (callback) {
          await callback({
            text: "Cannot walk - RS-SDK service not available.",
            action: "WALK_TO",
          });
        }
        return { success: false, error: "RS-SDK service not found" };
      }

      // Try to extract coordinates from multiple sources
      let x: number | undefined;
      let z: number | undefined;
      let resolvedLabel: string | undefined;

      // Try options.parameters first
      const params = options?.parameters as Record<string, string | number> | undefined;
      if (typeof params?.x === "number" && typeof params?.z === "number") {
        x = params.x;
        z = params.z;
      }

      // Gather all text for parsing
      const messageText = (message.content as { text?: string })?.text || "";
      const llmResponse = getCurrentLlmResponse();
      const allText = `${llmResponse} ${messageText}`;

      // Fallback: try to extract coordinates from the LLM's response text
      if (x === undefined || z === undefined) {
        // First try: parse <x> and <z> XML tags from <params> blocks (LLM often puts coords there)
        const xTag = allText.match(/<x>\s*(-?\d+(?:\.\d+)?)\s*<\/x>/i);
        const zTag = allText.match(/<z>\s*(-?\d+(?:\.\d+)?)\s*<\/z>/i);
        if (xTag?.[1] && zTag?.[1]) {
          x = parseFloat(xTag[1]);
          z = parseFloat(zTag[1]);
        }

        // Second try: regex patterns on natural language
        if (x === undefined || z === undefined) {
          const coordPatterns = [
            /(?:walk|go|move|head)\s+to\s+(?:coordinates?\s*)?(?:at\s+)?\(?(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\)?/i,
            /at\s+(?:coordinates?\s*)?\(?(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\)?/i,
            /coordinates?\s*\(?(-?\d+(?:\.\d+)?)\s*,\s*(-?\d+(?:\.\d+)?)\)?/i,
            /\((\d{3,4})\s*,\s*(\d{3,4})\)/i, // (3200, 3200) format
            /(\d{4})\s*,\s*(\d{4})/i, // 3200, 3200 bare format
          ];

          for (const pattern of coordPatterns) {
            const match = allText.match(pattern);
            if (match?.[1] && match?.[2]) {
              x = parseFloat(match[1]);
              z = parseFloat(match[2]);
              break;
            }
          }
        }
      }

      // Third fallback: resolve named destination from text
      if (typeof x !== "number" || typeof z !== "number" || Number.isNaN(x) || Number.isNaN(z)) {
        // Try the explicit "destination" parameter first
        const destParam = typeof params?.destination === "string" ? params.destination : "";
        const textToResolve = destParam || allText;

        // Get bot position for "nearest" lookups
        let botX: number | undefined;
        let botZ: number | undefined;
        try {
          const botState = await service.getBotState(runtime.agentId);
          if (botState) {
            botX = botState.player.worldX;
            botZ = botState.player.worldZ;
          }
        } catch {
          // Ignore — we'll resolve without position context
        }

        const resolved = resolveNamedDestination(textToResolve, botX, botZ);
        if (resolved) {
          x = resolved.x;
          z = resolved.z;
          resolvedLabel = resolved.label;
          logger.info({ destination: resolvedLabel, x, z }, "Resolved named destination to coordinates");
        }
      }

      // If we still have no coordinates, return a helpful message with known locations
      if (typeof x !== "number" || typeof z !== "number" || Number.isNaN(x) || Number.isNaN(z)) {
        const helpMsg = knownDestinationsHelp();
        logger.debug({ params, allText: allText.slice(0, 200) }, "WALK_TO could not resolve destination");
        if (callback) {
          await callback({ text: `I couldn't determine where to walk. ${helpMsg}`, action: "WALK_TO" });
        }
        return { success: false, error: "Could not resolve destination coordinates" };
      }

      const result = await service.executeAction(runtime.agentId, "walkTo", { x, z });

      const destDescription = resolvedLabel ? `${resolvedLabel} (${x}, ${z})` : `(${x}, ${z})`;
      logger.info({ action: "WALK_TO", destination: resolvedLabel, x, z, result }, "Executed walkTo action");

      if (callback) {
        await callback({
          text: result.success
            ? `Walking to ${destDescription}. ${result.message || ""}`
            : `Failed to walk to ${destDescription}: ${result.message}`,
          action: "WALK_TO",
        });
      }

      return {
        success: result.success,
        text: result.message || (result.success ? `Walking to ${destDescription}` : "Failed to walk"),
        data: { actionName: "walkTo", destination: resolvedLabel, x, z, result },
      };
    } catch (error) {
      logger.error({ error }, "Failed to execute WALK_TO action");
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      if (callback) {
        await callback({
          text: `Error walking: ${errorMessage}`,
          action: "WALK_TO",
        });
      }
      return { success: false, error: errorMessage };
    }
  },
};

export default walkToAction;
