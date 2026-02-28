import {
  type IAgentRuntime,
  type Memory,
  type Provider,
  type ProviderResult,
  type State,
  logger,
} from "@elizaos/core";
import type { RsSdkService } from "../services/RsSdkService";
import { RS_SDK_SERVICE_NAME } from "../types";
import { findArea } from "../data/areas";
import { findNearestBank } from "../data/banks";
import { getEventLog } from "./botStateProvider";

interface Goal {
  priority: "IMMEDIATE" | "SHORT_TERM" | "MEDIUM_TERM" | "EXPLORE";
  description: string;
  action: string; // suggested action
}

const goalsProvider: Provider = {
  name: "RS_SDK_GOALS",
  description: "Provides strategic goals and suggestions based on current state, skill levels, and recent activity",
  dynamic: false,
  position: 48, // After world knowledge, before bot state

  get: async (
    runtime: IAgentRuntime,
    _message: Memory,
    _state?: State
  ): Promise<ProviderResult> => {
    try {
      const service = runtime.getService<RsSdkService>(RS_SDK_SERVICE_NAME);
      if (!service) {
        return { text: "", values: {} };
      }

      const botState = await service.getBotState(runtime.agentId);
      if (!botState) {
        return { text: "", values: {} };
      }

      const goals: Goal[] = [];
      const { worldX, worldZ } = botState.player;
      const area = findArea(worldX, worldZ);

      // --- Skill analysis ---
      const getLevel = (name: string): number => botState.skills.find(s => s.name === name)?.level ?? 1;
      const atk = getLevel("Attack");
      const str = getLevel("Strength");
      const def = getLevel("Defence");
      const wc = getLevel("Woodcutting");
      const mining = getLevel("Mining");
      const fishing = getLevel("Fishing");
      const cooking = getLevel("Cooking");
      const fm = getLevel("Firemaking");
      const prayer = getLevel("Prayer");

      // --- Inventory analysis ---
      const invFull = botState.inventory.length >= 28;
      const invNearlyFull = botState.inventory.length >= 25;
      const { bank, distance: bankDist } = findNearestBank(worldX, worldZ);

      // --- HP analysis ---
      const hpSkill = botState.skills.find(s => s.name === "Hitpoints");
      const currentHp = hpSkill?.level ?? 10;
      const maxHp = hpSkill?.baseLevel ?? 10;
      const lowHp = maxHp > 0 && currentHp / maxHp < 0.4;

      // --- Recent activity analysis ---
      const eventLog = getEventLog();
      const recentActions = eventLog.slice(-20);
      const recentActionCounts = new Map<string, number>();
      for (const entry of recentActions) {
        recentActionCounts.set(entry.action, (recentActionCounts.get(entry.action) ?? 0) + 1);
      }

      // Check for repetitive behavior
      const mostCommonAction = [...recentActionCounts.entries()].sort((a, b) => b[1] - a[1])[0];
      const isStuck = mostCommonAction && mostCommonAction[1] >= 15;

      // === IMMEDIATE GOALS ===

      if (lowHp) {
        const hasFood = botState.inventory.some(i => /bread|shrimp|cooked|trout|salmon|lobster|cake/i.test(i.name));
        if (hasFood) {
          goals.push({ priority: "IMMEDIATE", description: "HP is low - eat food to heal before doing anything else", action: "EAT_FOOD" });
        } else {
          goals.push({ priority: "IMMEDIATE", description: "HP is low and no food! Walk to safety and get food", action: "WALK_TO (bank or safe area)" });
        }
      }

      if (invFull) {
        if (bankDist < 50) {
          goals.push({ priority: "IMMEDIATE", description: `Inventory full - bank is ${bankDist} tiles away at ${bank.name} (${bank.x}, ${bank.z}). Walk there and deposit items.`, action: "WALK_TO then OPEN_BANK then DEPOSIT_ITEM" });
        } else {
          goals.push({ priority: "IMMEDIATE", description: "Inventory full - drop low-value items (Logs, Bones) to make space, or walk to nearest bank", action: "DROP_ITEM" });
        }
      }

      // === SHORT-TERM GOALS (balance skills) ===

      // Balance combat stats
      if (Math.abs(atk - str) > 10) {
        const weak = atk < str ? "Attack" : "Strength";
        const style = atk < str ? 0 : 1;
        goals.push({ priority: "SHORT_TERM", description: `${weak} (${weak === "Attack" ? atk : str}) is much lower than ${weak === "Attack" ? "Strength" : "Attack"} (${weak === "Attack" ? str : atk}). Train ${weak} to balance combat stats.`, action: `SET_COMBAT_STYLE ${style} then ATTACK_NPC` });
      }

      if (def < Math.min(atk, str) - 15) {
        goals.push({ priority: "SHORT_TERM", description: `Defence (${def}) is much lower than Attack/Strength. Train Defence for survivability.`, action: "SET_COMBAT_STYLE 2 then ATTACK_NPC" });
      }

      // Low prayer - bury bones
      if (prayer < 15 && botState.inventory.some(i => /bones/i.test(i.name))) {
        goals.push({ priority: "SHORT_TERM", description: `Prayer is only ${prayer}. Bury the bones in your inventory for Prayer XP.`, action: "USE_ITEM (Bones)" });
      }

      // === MEDIUM-TERM GOALS (skill progression) ===

      // Suggest training lagging skills
      const allLevels = [
        { name: "Mining", level: mining },
        { name: "Fishing", level: fishing },
        { name: "Cooking", level: cooking },
      ];
      const lowestSkill = allLevels.sort((a, b) => a.level - b.level)[0];
      if (lowestSkill && lowestSkill.level < 15) {
        goals.push({ priority: "MEDIUM_TERM", description: `${lowestSkill.name} is only level ${lowestSkill.level}. Consider training it for a more well-rounded character.`, action: `Train ${lowestSkill.name}` });
      }

      // Quest encouragement
      if (area?.keyNpcs && area.keyNpcs.some(n => n.includes("quest"))) {
        goals.push({ priority: "MEDIUM_TERM", description: `There are quest NPCs in ${area?.name || "this area"}. Talk to them to start quests for XP rewards and unlocks.`, action: "TALK_TO_NPC" });
      }

      // === EXPLORE GOALS ===

      if (isStuck) {
        goals.push({ priority: "EXPLORE", description: `You've been doing ${mostCommonAction[0]} ${mostCommonAction[1]} times in a row. Try something different! Explore a new area or train a different skill.`, action: "WALK_TO a new area" });
      }

      // Suggest visiting areas the agent hasn't been to
      if (area) {
        const unexploredSuggestion = area.adjacentAreas[Math.floor(Math.random() * area.adjacentAreas.length)];
        if (unexploredSuggestion) {
          goals.push({ priority: "EXPLORE", description: `Consider visiting ${unexploredSuggestion.name} (${unexploredSuggestion.direction}) - ${unexploredSuggestion.description}. WALK_TO (${unexploredSuggestion.walkCoords.x}, ${unexploredSuggestion.walkCoords.z})`, action: `WALK_TO (${unexploredSuggestion.walkCoords.x}, ${unexploredSuggestion.walkCoords.z})` });
        }
      }

      // Always have at least one goal
      if (goals.length === 0) {
        goals.push({ priority: "MEDIUM_TERM", description: "No urgent goals. Continue training your lowest skill or explore a new area.", action: "Train or explore" });
      }

      // Format output
      const goalsByPriority = ["IMMEDIATE", "SHORT_TERM", "MEDIUM_TERM", "EXPLORE"] as const;
      const goalLines: string[] = [];
      let num = 1;
      for (const priority of goalsByPriority) {
        const matching = goals.filter(g => g.priority === priority);
        for (const goal of matching) {
          goalLines.push(`${num}. [${priority}] ${goal.description}`);
          num++;
        }
      }

      // Suggested next move
      const topGoal = goals[0];
      const suggestedMove = topGoal
        ? `Based on priorities: ${topGoal.description} (${topGoal.action})`
        : "Continue what you are doing.";

      const text = `# Strategic Goals

## Current Goals (priority order)
${goalLines.join("\n")}

## Suggested Next Move
${suggestedMove}

## Tips
- If you've been in one area too long, WALK_TO a new area for variety
- Talk to NPCs with unique names - they may have quests
- Balance your combat stats (Attack, Strength, Defence) for better fighting
- Always bank or drop items when inventory is full before gathering more`;

      return {
        text,
        values: {
          topGoalPriority: topGoal?.priority || "NONE",
          topGoalAction: topGoal?.action || "",
          goalCount: String(goals.length),
          isStuck: String(Boolean(isStuck)),
        },
      };
    } catch (error) {
      logger.error({ error }, "Goals provider error");
      return { text: "", values: {} };
    }
  },
};

export default goalsProvider;
