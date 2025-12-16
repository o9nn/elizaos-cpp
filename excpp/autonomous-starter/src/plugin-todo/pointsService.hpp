#include "elizaos/core.hpp"
#include "services/componentService.hpp"
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

;
;

// Define the structure for the points component data
struct UserPointsData {
    double currentPoints;
    std::optional<std::string> lastPointUpdateReason;
    std::optional<{ timestamp: number; points: number; reason: string }[]; // Optional history> history;
};


// Define the component structure
interface UserPointsComponent extends Component {
  type: "userPoints";
  data: UserPointsData;
}

const POINT_COMPONENT_TYPE = "userPoints";
const MAX_HISTORY = 10; // Keep last 10 point updates

/**
 * Calculates points based on task type and completion status.
 * Needs refinement based on specific point values.
 */
. Defaulting to 4.`,
    );
    // priority = 4;
  }

  switch (completionStatus) {
    case "onTime":
      // Higher points for higher priority (lower number) and urgent tasks
      points = (5 - priority) * 10; // P1=40, P2=30, P3=20, P4=10
      if (task.tags?.includes("urgent")) {
        points += 10;
      }
      break;
    case "late":
      points = 5; // Flat small points for late completion
      break;
    case "daily":
      points = 10; // Standard points for daily tasks
      break;
    case "streakBonus":
      const streak =
        typeof task.metadata?.streak === "number" ? task.metadata.streak : 0;
      points = Math.min(streak * 5, 50); // Bonus points for streak, capped
      break;
  }
  logger.debug(
    `Calculated points: ${points} for task ${task.name} (${completionStatus})`,
  );
  return points;
}

/**
 * Retrieves the current points for a given entity.
 * Creates the component if it doesn't exist.
 */
std::future<double> getPoints(IAgentRuntime runtime, UUID entityId, UUID roomId, UUID worldId);, creating.`,
      );
      // Create component if it doesn't exist
      const newComponentData: UserPointsData = {
        currentPoints: 0,
        history: [],
      };

      // Use the provided parameters directly
      if (!roomId || !worldId) {
        logger.error(
          `Cannot create points component for entity ${entityId}: Invalid roomId or worldId provided.`,
        );
        return 0;
      }

      logger.debug(
        `Creating points component for entity ${entityId} using provided roomId ${roomId} and worldId ${worldId}.`,
      );

      await componentService.createComponent({
        entityId: entityId,
        agentId: runtime.agentId,
        type: POINT_COMPONENT_TYPE,
        data: newComponentData,
        roomId: roomId,
        worldId: worldId,
        sourceEntityId: runtime.agentId,
      });
      return 0;
    }
    return component.data.currentPoints;
  } catch (error) {
    logger.error(`Error getting points for entity ${entityId}:`, error);
    return 0; // Return 0 in case of error
  }
}

/**
 * Adds points to a user's total and updates the component.
 */
std::future<bool> addPoints(IAgentRuntime runtime, UUID entityId, double pointsToAdd, const std::string& reason, UUID roomId, UUID worldId); during addPoints, creating.`,
      );
      currentData = { currentPoints: 0, history: [] };

      // Use the provided parameters directly
      if (!roomId || !worldId) {
        logger.error(
          `Cannot create points component for entity ${entityId} during addPoints: Invalid roomId or worldId provided.`,
        );
        return false;
      }

      logger.debug(
        `Creating points component for entity ${entityId} using provided roomId ${roomId} and worldId ${worldId}.`,
      );

      // Create component before updating
      const createdComponentId = await componentService.createComponent({
        entityId: entityId,
        agentId: runtime.agentId,
        type: POINT_COMPONENT_TYPE,
        data: currentData,
        roomId: roomId,
        worldId: worldId,
        sourceEntityId: runtime.agentId,
      });

      if (!createdComponentId) {
        logger.error(
          `Failed to create points component for entity ${entityId}.`,
        );
        return false;
      }
      // Since createComponent returns boolean in current core types, we have to refetch.
      // Ideally, it would return the component or ID.
      component = (await componentService.getComponent(
        entityId,
        POINT_COMPONENT_TYPE,
      )) as UserPointsComponent | null;

      if (!component) {
        logger.error(
          `Failed to retrieve points component for entity ${entityId} after creation.`,
        );
        return false;
      }
      componentIdToUpdate = component.id!;
      componentRoomId = component.roomId;
      componentWorldId = component.worldId;
      componentAgentId = component.agentId;
      componentSourceEntityId = component.sourceEntityId;
    } else {
      currentData = component.data;
      componentIdToUpdate = component.id!;
      componentRoomId = component.roomId;
      componentWorldId = component.worldId;
      componentAgentId = component.agentId;
      componentSourceEntityId = component.sourceEntityId;
    }

    const newPoints = currentData.currentPoints + pointsToAdd;
    const newHistoryEntry = {
      timestamp: Date.now(),
      points: pointsToAdd,
      reason,
    };

    const updatedHistory = [...(currentData.history ?? [])];
    updatedHistory.push(newHistoryEntry);
    if (updatedHistory.length > MAX_HISTORY) {
      updatedHistory.shift(); // Remove oldest entry
    }

    const updatedData: UserPointsData = {
      currentPoints: newPoints,
      lastPointUpdateReason: reason,
      history: updatedHistory,
    };

    logger.debug(
      `Adding ${pointsToAdd} points to entity ${entityId}. New total: ${newPoints}. Reason: ${reason}`,
    );

    // Update the component
    await componentService.updateComponent({
      id: componentIdToUpdate,
      entityId: entityId,
      agentId: componentAgentId,
      roomId: componentRoomId,
      worldId: componentWorldId,
      sourceEntityId: componentSourceEntityId,
      type: POINT_COMPONENT_TYPE,
      data: updatedData,
    });

    return true;
  } catch (error) {
    logger.error(`Error adding points for entity ${entityId}:`, error);
    return false;
  }
}

} // namespace elizaos
