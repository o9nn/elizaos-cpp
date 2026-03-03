import {
  type IAgentRuntime,
  type Memory,
  type Provider,
  type ProviderResult,
  type State,
  logger,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME, type EventLogEntry } from "../types";

// Module-level event log shared across provider calls
const eventLog: EventLogEntry[] = [];
const MAX_EVENT_LOG = 30;

/** Add an event to the shared event log (called from run.ts) */
export function addEventLogEntry(entry: EventLogEntry): void {
  eventLog.push(entry);
  if (eventLog.length > MAX_EVENT_LOG * 2) {
    eventLog.splice(0, eventLog.length - MAX_EVENT_LOG);
  }
}

/** Get the event log (for external access) */
export function getEventLog(): readonly EventLogEntry[] {
  return eventLog;
}

const botStateProvider: Provider = {
  name: "RS_SDK_BOT_STATE",
  description: "Provides full bot state: position, inventory, equipment, skills, combat, nearby entities, game messages, and event history",
  dynamic: false,
  position: 50,
  get: async (
    runtime: IAgentRuntime,
    _message: Memory,
    _state?: State
  ): Promise<ProviderResult> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        logger.debug("RS-SDK service not found yet - bot state provider waiting for initialization");
        return {
          text: "# Bot State Unavailable\n\nThe RS-SDK service is not registered yet. Wait for it to initialize.",
          values: { error: "rs_sdk_service_not_found" },
        };
      }

      const botState = await service.getBotState(runtime.agentId);
      if (!botState) {
        return {
          text: "# Bot State Unavailable\n\nBot is not connected or not in-game yet. Waiting for connection...",
          values: { error: "bot_state_unavailable" },
        };
      }

      // --- Inventory analysis ---
      const invSlotsFilled = botState.inventory.length;
      const invCapacity = 28;
      const invPercent = Math.round((invSlotsFilled / invCapacity) * 100);
      const invFull = invSlotsFilled >= invCapacity;
      const invNearlyFull = invSlotsFilled >= 25;

      // --- HP analysis (Hitpoints skill) ---
      const hpSkill = botState.skills.find(s => s.name === "Hitpoints");
      const currentHp = hpSkill?.level ?? 10;
      const maxHp = hpSkill?.baseLevel ?? 10;
      const hpPercent = maxHp > 0 ? Math.round((currentHp / maxHp) * 100) : 100;
      const lowHp = hpPercent <= 30;

      // --- Food in inventory ---
      const knownFoods = ["shrimp", "anchovies", "bread", "cooked meat", "cooked chicken", "trout", "salmon", "tuna", "lobster", "swordfish", "shark", "cake", "meat pie", "potato"];
      const foodItems = botState.inventory.filter(item =>
        knownFoods.some(food => item.name.toLowerCase().includes(food))
      );

      // --- Combat state ---
      const inCombat = botState.player.combat.inCombat;

      // --- Slots remaining ---
      const slotsRemaining = invCapacity - invSlotsFilled;

      // --- Droppable items (non-tool items that can be dropped to free space) ---
      const toolNames = ["axe", "pickaxe", "tinderbox", "hammer", "chisel", "knife", "needle"];
      const droppableItems = botState.inventory.filter(item =>
        !toolNames.some(tool => item.name.toLowerCase().includes(tool))
      );

      // --- Alerts ---
      const alerts: string[] = [];
      if (invFull) {
        alerts.push(`INVENTORY FULL (28/28, 0 slots remaining) - You MUST drop items (use DROP_ITEM) or bank before gathering more!`);
        if (droppableItems.length > 0) {
          const droppableNames = [...new Set(droppableItems.map(i => i.name))].slice(0, 5).join(", ");
          alerts.push(`Droppable items: ${droppableNames}`);
        }
      } else if (invNearlyFull) {
        alerts.push(`INVENTORY NEARLY FULL (${invSlotsFilled}/28, ${slotsRemaining} slots remaining) - Consider banking or dropping items soon.`);
      }
      if (lowHp) alerts.push(`LOW HP (${currentHp}/${maxHp} = ${hpPercent}%) - Eat food or retreat!`);
      if (lowHp && foodItems.length === 0) alerts.push("NO FOOD AND LOW HP - Retreat immediately!");
      if (inCombat) alerts.push("Currently in combat!");

      // --- Format state text ---
      const sections: string[] = [];

      // Alerts section
      if (alerts.length > 0) {
        sections.push(`## ⚠️ ALERTS\n${alerts.map(a => `- ${a}`).join("\n")}`);
      }

      // Player status
      sections.push(`## Player Status
- **Name**: ${botState.player.name}
- **Position**: (${botState.player.worldX}, ${botState.player.worldZ}) level ${botState.player.level}
- **Combat Level**: ${botState.player.combatLevel}
- **HP**: ${currentHp}/${maxHp} (${hpPercent}%)
- **Run Energy**: ${botState.player.runEnergy}%
- **In Combat**: ${inCombat ? "YES" : "No"}
- **Game Tick**: ${botState.tick}`);

      // Skills (compact format - only show non-1 skills or all)
      const relevantSkills = botState.skills.filter(s => s.level > 1 || s.name === "Hitpoints" || s.name === "Attack" || s.name === "Strength" || s.name === "Defence");
      const otherSkills = botState.skills.filter(s => !relevantSkills.includes(s));
      let skillsText = relevantSkills.map(s => `- ${s.name}: ${s.level}/${s.baseLevel} (${s.experience} XP)`).join("\n");
      if (otherSkills.length > 0) {
        const level1Skills = otherSkills.filter(s => s.level === 1).map(s => s.name).join(", ");
        if (level1Skills) {
          skillsText += `\n- Level 1: ${level1Skills}`;
        }
      }
      sections.push(`## Skills\n${skillsText}`);

      // Inventory
      const invHeader = invFull
        ? `## Inventory (FULL ${invSlotsFilled}/${invCapacity} - 0 slots free - DROP or BANK items!)`
        : `## Inventory (${invSlotsFilled}/${invCapacity} slots used, ${slotsRemaining} free)`;
      const invItems = botState.inventory.length > 0
        ? botState.inventory.map(item => `- [${item.slot}] ${item.name} x${item.count}`).join("\n")
        : "- Empty";
      sections.push(`${invHeader}\n${invItems}`);

      // Equipment
      if (botState.equipment.length > 0) {
        const eqItems = botState.equipment.map(item => `- ${item.name}`).join("\n");
        sections.push(`## Equipment\n${eqItems}`);
      }

      // Nearby NPCs
      if (botState.nearbyNpcs.length > 0) {
        const npcs = botState.nearbyNpcs.slice(0, 10).map(npc => {
          const opts = npc.options.length > 0 ? ` [${npc.options.join(", ")}]` : "";
          return `- ${npc.name} (Level ${npc.combatLevel}) dist=${npc.distance.toFixed(1)}${opts}`;
        }).join("\n");
        sections.push(`## Nearby NPCs (${botState.nearbyNpcs.length})\n${npcs}`);
      } else {
        sections.push("## Nearby NPCs\n- None");
      }

      // Nearby Locations/Objects
      if (botState.nearbyLocs.length > 0) {
        const locs = botState.nearbyLocs.slice(0, 10).map(loc => {
          const opts = loc.options.length > 0 ? ` [${loc.options.join(", ")}]` : "";
          return `- ${loc.name} at (${loc.x}, ${loc.z}) dist=${loc.distance.toFixed(1)}${opts}`;
        }).join("\n");
        sections.push(`## Nearby Objects (${botState.nearbyLocs.length})\n${locs}`);
      }

      // Ground Items
      if (botState.groundItems.length > 0) {
        const items = botState.groundItems.slice(0, 8).map(item =>
          `- ${item.name} x${item.count} at (${item.x}, ${item.z}) dist=${item.distance.toFixed(1)}`
        ).join("\n");
        sections.push(`## Ground Items (${botState.groundItems.length})\n${items}`);
      }

      // Game Messages (recent)
      if (botState.gameMessages.length > 0) {
        const msgs = botState.gameMessages.slice(-8).map(msg =>
          msg.sender ? `- [${msg.sender}]: ${msg.text}` : `- ${msg.text}`
        ).join("\n");
        sections.push(`## Recent Game Messages\n${msgs}`);
      }

      // Combat Events (recent)
      if (botState.combatEvents.length > 0) {
        const events = botState.combatEvents.slice(-5).map(evt =>
          `- ${evt.type}: ${evt.damage} damage (tick ${evt.tick})`
        ).join("\n");
        sections.push(`## Recent Combat Events\n${events}`);
      }

      // Event Log (our action history)
      const recentEvents = eventLog.slice(-MAX_EVENT_LOG);
      if (recentEvents.length > 0) {
        const evtText = recentEvents.map(e =>
          `- [Step ${e.step}] ${e.action}: ${e.result} - ${e.message}`
        ).join("\n");
        sections.push(`## Action History (last ${recentEvents.length} actions)\n${evtText}`);
      }

      const stateText = `# Bot State\n\n${sections.join("\n\n")}`;

      return {
        text: stateText,
        values: {
          botState: JSON.stringify(botState),
          inventoryFull: String(invFull),
          inventoryPercent: String(invPercent),
          slotsRemaining: String(slotsRemaining),
          slotsUsed: String(invSlotsFilled),
          hpPercent: String(hpPercent),
          inCombat: String(inCombat),
          hasAlerts: String(alerts.length > 0),
        },
      };
    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : "Unknown error";
      logger.error({ error, agentId: runtime.agentId }, "Failed to get bot state");
      return {
        text: `# Bot State Error\n\nFailed to retrieve bot state: ${errorMessage}`,
        values: { error: "bot_state_error", errorMessage },
      };
    }
  },
};

export default botStateProvider;
