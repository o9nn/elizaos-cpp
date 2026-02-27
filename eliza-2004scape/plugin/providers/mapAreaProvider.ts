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
import { findArea, findNearestArea } from "../data/areas";

const mapAreaProvider: Provider = {
  name: "RS_SDK_MAP_AREA",
  description: "Identifies the current map area, what is available here, and how to reach nearby areas",
  dynamic: false,
  position: 40, // Before bot state and world knowledge

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
      const area = findArea(worldX, worldZ);

      if (area) {
        const adjacentList = area.adjacentAreas.map(a =>
          `- **${a.name}** (${a.direction}) - WALK_TO (${a.walkCoords.x}, ${a.walkCoords.z}) - ${a.description}`
        ).join("\n");

        const text = `# Current Area: ${area.name}
${area.description}

**Available here:** ${area.features.join(", ")}
**Trainable skills:** ${area.trainableSkills.join(", ")}
**Key NPCs to talk to:** ${area.keyNpcs.join(", ")}

## Travel to Other Areas
${adjacentList}

Use WALK_TO with the coordinates above to travel to a new area. Exploring new areas unlocks new training methods, shops, and quests.`;

        return {
          text,
          values: {
            currentArea: area.name,
            currentAreaDescription: area.description,
          },
        };
      }

      // Not in a known area - find nearest
      const nearest = findNearestArea(worldX, worldZ);
      const text = `# Current Area: Unknown (between areas)
You are at (${worldX}, ${worldZ}), not in a recognized area.

**Nearest known area:** ${nearest.area.name} (~${nearest.distance} tiles away)
Walk toward ${nearest.area.name} for training and services.

## How to get to ${nearest.area.name}
${nearest.area.adjacentAreas.length > 0
  ? nearest.area.adjacentAreas.map(a => `- **${a.name}** (${a.direction}) - WALK_TO (${a.walkCoords.x}, ${a.walkCoords.z})`).join("\n")
  : `Walk toward (${Math.round((nearest.area.bounds.minX + nearest.area.bounds.maxX) / 2)}, ${Math.round((nearest.area.bounds.minZ + nearest.area.bounds.maxZ) / 2)})`
}`;

      return {
        text,
        values: { currentArea: "Unknown", nearestArea: nearest.area.name },
      };
    } catch (error) {
      logger.error({ error }, "Map area provider error");
      return { text: "", values: {} };
    }
  },
};

export default mapAreaProvider;
