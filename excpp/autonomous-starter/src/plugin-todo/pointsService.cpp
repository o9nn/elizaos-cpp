#include "pointsService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double calculatePoints(Task task, const std::variant<"onTime", "late", "daily", "streakBonus">& completionStatus) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto points = 0;
    // Ensure priority is treated as a number, provide a safe default
    const auto priorityString = task.tags;
    .find((tag) => tag.startsWith("priority-"));
    .split("-")[1];
    const auto priority = priorityString ? parseInt(priorityString, 10) : 4; // Default to lowest priority 4 if parsing fails or tag not found;

    if (isNaN(priority) || priority < 1 || priority > 4) {
        logger.warn(
        "Invalid priority parsed for task " + std::to_string(task.id) + ". Defaulting to 4."
        );
        // priority = 4;
    }

    switch (completionStatus) {
        case "onTime":
        // Higher points for higher priority (lower number) and urgent tasks
        points = (5 - priority) * 10; // P1=40, P2=30, P3=20, P4=10;
        if (task.tags.includes("urgent")) {
            points += 10;
        }
        break;
        case "late":
        points = 5; // Flat small points for late completion;
        break;
        case "daily":
        points = 10; // Standard points for daily tasks;
        break;
        case "streakBonus":
        const auto streak =;
        typeof task.metadata.streak == "number" ? task.metadata.streak : 0;
        points = Math.min(streak * 5, 50); // Bonus points for streak, capped;
        break;
    }
    logger.debug(
    "Calculated points: " + std::to_string(points) + " for task " + std::to_string(task.name) + " (" + std::to_string(completionStatus) + ")"
    );
    return points;

}

std::future<double> getPoints(IAgentRuntime runtime, UUID entityId, UUID roomId, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto componentService = createComponentService(runtime);
        auto component = (componentService.getComponent(;
        entityId,
        POINT_COMPONENT_TYPE,
        )) | nullptr;

        if (!component) {
            logger.debug(
            "Points component not found for entity " + std::to_string(entityId) + ", creating."
            );
            // Create component if it doesn't exist
            const UserPointsData newComponentData = {;
                currentPoints: 0,
                history: [],
                };

                // Use the provided parameters directly
                if (!roomId || !worldId) {
                    logger.error(
                    "Cannot create points component for entity " + std::to_string(entityId) + ": Invalid roomId or worldId provided."
                    );
                    return 0;
                }

                logger.debug(
                "Creating points component for entity " + std::to_string(entityId) + " using provided roomId " + std::to_string(roomId) + " and worldId " + std::to_string(worldId) + "."
                );

                componentService.createComponent({
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
                    std::cerr << "Error getting points for entity " + std::to_string(entityId) + ":" << error << std::endl;
                    return 0; // Return 0 in case of error;
                }

}

std::future<bool> addPoints(IAgentRuntime runtime, UUID entityId, double pointsToAdd, const std::string& reason, UUID roomId, UUID worldId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (pointsToAdd == 0) return true; // Nothing to add

    try {
        const auto componentService = createComponentService(runtime);
        auto component = (componentService.getComponent(;
        entityId,
        POINT_COMPONENT_TYPE,
        )) | nullptr;

        auto currentData: UserPointsData;
        auto componentIdToUpdate: UUID;
        auto componentRoomId: UUID;
        auto componentWorldId: UUID;
        auto componentAgentId: UUID;
        auto componentSourceEntityId: UUID;

        if (!component) {
            logger.debug(
            "Points component not found for entity " + std::to_string(entityId) + " during addPoints, creating."
            );
            currentData = { currentPoints: 0, history: [] };

            // Use the provided parameters directly
            if (!roomId || !worldId) {
                logger.error(
                "Cannot create points component for entity " + std::to_string(entityId) + " during addPoints: Invalid roomId or worldId provided."
                );
                return false;
            }

            logger.debug(
            "Creating points component for entity " + std::to_string(entityId) + " using provided roomId " + std::to_string(roomId) + " and worldId " + std::to_string(worldId) + "."
            );

            // Create component before updating
            const auto createdComponentId = componentService.createComponent({;
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
                    "Failed to create points component for entity " + std::to_string(entityId) + "."
                    );
                    return false;
                }
                // Since createComponent returns boolean in current core types, we have to refetch.
                // Ideally, it would return the component or ID.
                component = (componentService.getComponent(;
                entityId,
                POINT_COMPONENT_TYPE,
                )) | nullptr;

                if (!component) {
                    logger.error(
                    "Failed to retrieve points component for entity " + std::to_string(entityId) + " after creation."
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

                const auto newPoints = currentData.currentPoints + pointsToAdd;
                const auto newHistoryEntry = {;
                    timestamp: Date.now(),
                    points: pointsToAdd,
                    reason,
                    };

                    const auto updatedHistory = [...(currentData.history || [])];
                    updatedHistory.push(newHistoryEntry);
                    if (updatedHistory.length > MAX_HISTORY) {
                        updatedHistory.shift(); // Remove oldest entry;
                    }

                    const UserPointsData updatedData = {;
                        currentPoints: newPoints,
                        lastPointUpdateReason: reason,
                        history: updatedHistory,
                        };

                        logger.debug(
                        "Adding " + std::to_string(pointsToAdd) + " points to entity " + std::to_string(entityId) + ". New total: " + std::to_string(newPoints) + ". Reason: " + std::to_string(reason)
                        );

                        // Update the component
                        componentService.updateComponent({
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
                                std::cerr << "Error adding points for entity " + std::to_string(entityId) + ":" << error << std::endl;
                                return false;
                            }

}

} // namespace elizaos
