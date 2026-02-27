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
import { findNearestBank } from "../data/banks";
import { getTrainingRecommendation } from "../data/training";

const worldKnowledgeProvider: Provider = {
  name: "RS_SDK_WORLD_KNOWLEDGE",
  description: "Provides nearest bank location and skill training recommendations based on current levels",
  dynamic: false,
  position: 45, // After map area, before bot state

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

      const { worldX, worldZ } = botState.player;
      const sections: string[] = [];

      // --- Nearest Bank ---
      const { bank, distance } = findNearestBank(worldX, worldZ);
      sections.push(`## Nearest Bank
**${bank.name}** at (${bank.x}, ${bank.z}) - ${distance} tiles away (${bank.area})
${bank.notes}
${distance > 50 ? "WARNING: Bank is far away. Consider walking there if inventory is nearly full." : "Bank is close enough to visit."}`);

      // --- Skill Training Recommendations ---
      const trainableSkills = [
        "Woodcutting", "Mining", "Fishing", "Cooking", "Firemaking",
        "Smithing", "Crafting", "Fletching", "Prayer", "Thieving",
        "Ranged", "Magic",
      ];
      const combatSkills = ["Attack", "Strength", "Defence"];

      const recommendations: string[] = [];

      // Combat recommendation
      const atkLevel = botState.skills.find(s => s.name === "Attack")?.level ?? 1;
      const strLevel = botState.skills.find(s => s.name === "Strength")?.level ?? 1;
      const defLevel = botState.skills.find(s => s.name === "Defence")?.level ?? 1;
      const combatRec = getTrainingRecommendation("Combat", Math.min(atkLevel, strLevel, defLevel));
      if (combatRec) {
        const weakest = atkLevel <= strLevel && atkLevel <= defLevel ? "Attack" : strLevel <= defLevel ? "Strength" : "Defence";
        recommendations.push(`- **Combat** (Atk:${atkLevel}/Str:${strLevel}/Def:${defLevel}): ${combatRec.method} at ${combatRec.location} (${combatRec.coords.x}, ${combatRec.coords.z}). Focus on ${weakest} with SET_COMBAT_STYLE.`);
      }

      // Gathering/production skills
      for (const skillName of trainableSkills) {
        const skill = botState.skills.find(s => s.name === skillName);
        const level = skill?.level ?? 1;
        const rec = getTrainingRecommendation(skillName, level);
        if (rec) {
          recommendations.push(`- **${skillName}** (${level}): ${rec.method} at ${rec.location} (${rec.coords.x}, ${rec.coords.z}). ${rec.requirements}. ${rec.notes}`);
        }
      }

      if (recommendations.length > 0) {
        sections.push(`## Skill Training Recommendations\nBased on your current levels:\n${recommendations.join("\n")}`);
      }

      // --- Critical Warnings (from proven learnings) ---
      sections.push(`## Critical Warnings
- **NO BANK IN LUMBRIDGE** (2004scape): Nearest banks are Draynor Village (3092, 3243), Al Kharid (3269, 3167, requires 10gp toll), or Varrock West (3185, 3436).
- **OPEN GATES/DOORS FIRST**: Always use OPEN_DOOR before WALK_TO through fenced areas (cow field, chicken coop). The pathfinder CANNOT handle closed gates and will get stuck.
- **DARK WIZARDS DANGER ZONE**: Aggressive Dark Wizards at ~(3220, 3220) between Lumbridge and Draynor. They WILL kill characters under combat level 20. Stay north of z=3240 when walking west from Lumbridge.
- **GENERAL STORES PAY 0 GP WHEN OVERSTOCKED**: Don't sell lots of the same item at general stores. Use specialized shops or make money via Thieving instead.
- **LUMBRIDGE HAS NO LEVEL-1 FISHING SPOTS**: The swamp area only has fly fishing (level 20+). Go to Draynor Village (3087, 3230) for shrimp fishing.
- **LUMBRIDGE SWAMP MINE IS UNRELIABLE**: Mining interactions fail silently. Use SE Varrock mine (3285, 3365) for copper/tin instead.
- **FISHING SPOTS ARE NPCs THAT MOVE**: They appear in the Nearby NPCs list as "Fishing spot", not in Nearby Objects. They drift over time.
- **KEBAB SELLER KARIM uses dialog**: At (3273, 3180) in Al Kharid. Use TALK_TO_NPC then NAVIGATE_DIALOG to buy (1gp each), NOT OPEN_SHOP.
- **DIALOG INDEX 0 = CONTINUE**: When navigating dialogs, option 0 means "click here to continue". Options 1-5 are actual choice buttons.
- **THIEVING REQUIRES NO TOOLS**: Great for making money from level 1 with nothing in inventory. Men give 3GP, Farmers give 9GP per pickpocket.`);

      // --- Quest Hints ---
      sections.push(`## Quests
Talk to NPCs to discover quests. Look for NPCs with unique names (not generic 'Man' or 'Woman'). Key quest NPCs in early areas:
- **Cook** in Lumbridge Castle - has a quest about making a cake
- **Duke Horacio** in Lumbridge Castle upstairs - has a quest about rune essence
- **Morgan** in Draynor Village - vampire problem
- **Romeo** in Varrock square - lost his love
- **Aubury** in Varrock rune shop - mysterious portal

Use TALK_TO_NPC to start conversations, then NAVIGATE_DIALOG to progress through quest dialog.`);

      const text = `# World Knowledge\n\n${sections.join("\n\n")}`;

      return {
        text,
        values: {
          nearestBankName: bank.name,
          nearestBankDistance: String(distance),
          nearestBankCoords: `${bank.x},${bank.z}`,
        },
      };
    } catch (error) {
      logger.error({ error }, "World knowledge provider error");
      return { text: "", values: {} };
    }
  },
};

export default worldKnowledgeProvider;
