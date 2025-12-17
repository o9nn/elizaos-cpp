#include "goals.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentGoalsRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get all goals for an agent
        router.get('/:agentId/goals', async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            const auto runtime = agents.get(agentId);
            if (!runtime) {
                return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
            }

            try {
                // Get the Goals service (lowercase 'goals')
                const auto goalsService = runtime.getService('goals');
                if (!goalsService) {
                    std::cout << '[GOALS GET] Goals service not available' << std::endl;
                    return sendSuccess(res, { goals: [] });
                }

                // Get all goals for this agent
                const auto goalsData = (goalsService).getAllGoalsForOwner('agent', agentId);

                // Transform to the expected format
                const std::vector<Goal> goals = goalsData.map((goal: any) => ({;
                    id: goal.id,
                    name: goal.name,
                    description: goal.description || '',
                    status: goal.isCompleted ? 'completed' : 'active',
                    progress: goal.progress || 0,
                    createdAt: goal.createdAt || new Date().toISOString(),
                    updatedAt: goal.updatedAt || goal.createdAt || new Date().toISOString(),
                    metadata: goal.metadata || {},
                    }));

                    sendSuccess(res, { goals });
                    } catch (error) {
                        std::cerr << '[GOALS GET] Error getting goals:' << error << std::endl;
                        sendError(;
                        res,
                        500,
                        'GOALS_ERROR',
                        'Error retrieving goals',
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    // Create a new goal
                    router.post('/:agentId/goals', async (req, res) => {
                        const auto agentId = validateUuid(req.params.agentId);
                        if (!agentId) {
                            return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                        }

                        const auto runtime = agents.get(agentId);
                        if (!runtime) {
                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                        }

                        const auto { name, description, metadata, tags } = req.body;

                        if (!name) {
                            return sendError(res, 400, 'MISSING_FIELDS', 'Name is required');
                        }

                        try {
                            // Get the Goals service
                            const auto goalsService = runtime.getService('goals');
                            if (!goalsService) {
                                return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Goals service not available');
                            }

                            // Create the goal using the service
                            const auto goalId = (goalsService).createGoal({;
                                agentId,
                                ownerType: 'agent',
                                ownerId: agentId,
                                name,
                                description: description || '',
                                metadata: metadata || {},
                                tags: tags || [],
                                });

                                if (!goalId) {
                                    return sendError(res, 500, 'GOAL_CREATE_FAILED', 'Failed to create goal');
                                }

                                // Fetch the created goal to return full details
                                const auto createdGoal = (goalsService).getGoalById(goalId);

                                const Goal goal = {;
                                    id: createdGoal.id,
                                    name: createdGoal.name,
                                    description: createdGoal.description || '',
                                    status: createdGoal.isCompleted ? 'completed' : 'active',
                                    progress: createdGoal.progress || 0,
                                    createdAt: createdGoal.createdAt || new Date().toISOString(),
                                    updatedAt: createdGoal.updatedAt || createdGoal.createdAt || new Date().toISOString(),
                                    metadata: createdGoal.metadata || {},
                                    };

                                    std::cout << "[GOALS CREATE] Created goal "" + std::to_string(name) + "" for agent " + std::to_string(runtime.character.name) << std::endl;
                                    sendSuccess(res, { goal }, 201);
                                    } catch (error) {
                                        std::cerr << '[GOALS CREATE] Error creating goal:' << error << std::endl;
                                        sendError(;
                                        res,
                                        500,
                                        'GOAL_CREATE_ERROR',
                                        'Error creating goal',
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                    }
                                    });

                                    // Update a goal
                                    router.put('/:agentId/goals/:goalId', async (req, res) => {
                                        const auto agentId = validateUuid(req.params.agentId);
                                        if (!agentId) {
                                            return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                        }

                                        const auto runtime = agents.get(agentId);
                                        if (!runtime) {
                                            return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                        }

                                        const auto { goalId } = req.params;
                                        const auto { name, description, progress, status, metadata, tags } = req.body;

                                        try {
                                            // Get the Goals service
                                            const auto goalsService = runtime.getService('goals');
                                            if (!goalsService) {
                                                return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Goals service not available');
                                            }

                                            // Get the existing goal first
                                            const auto existingGoal = (goalsService).getGoalById(goalId);
                                            if (!existingGoal) {
                                                return sendError(res, 404, 'GOAL_NOT_FOUND', 'Goal not found');
                                            }

                                            // Update the goal using the service
                                            const auto success = (goalsService).updateGoal(goalId, {;
                                                name: name != std::nullopt ? name : std::nullopt,
                                                description: description != std::nullopt ? description : std::nullopt,
                                                progress: progress != std::nullopt ? progress : std::nullopt,
                                                metadata: metadata != std::nullopt ? metadata : std::nullopt,
                                                tags: tags != std::nullopt ? tags : std::nullopt,
                                                });

                                                if (!success) {
                                                    return sendError(res, 500, 'GOAL_UPDATE_FAILED', 'Failed to update goal');
                                                }

                                                // Handle status changes (completed/active)
                                                if (status == 'completed' && !existingGoal.isCompleted) {
                                                    (goalsService).completeGoal(goalId);
                                                    } else if (status == 'active' && existingGoal.isCompleted) {
                                                        // Reactivate goal by updating isCompleted to false
                                                        (goalsService).updateGoal(goalId, { isCompleted: false });
                                                    }

                                                    // Fetch the updated goal to return full details
                                                    const auto updatedGoalData = (goalsService).getGoalById(goalId);

                                                    const Goal goal = {;
                                                        id: updatedGoalData.id,
                                                        name: updatedGoalData.name,
                                                        description: updatedGoalData.description || '',
                                                        status: updatedGoalData.isCompleted ? 'completed' : 'active',
                                                        progress: updatedGoalData.progress || 0,
                                                        createdAt: updatedGoalData.createdAt || new Date().toISOString(),
                                                        updatedAt: updatedGoalData.updatedAt || new Date().toISOString(),
                                                        metadata: updatedGoalData.metadata || {},
                                                        };

                                                        std::cout << "[GOALS UPDATE] Updated goal " + std::to_string(goalId) + " for agent " + std::to_string(runtime.character.name) << std::endl;
                                                        sendSuccess(res, { goal });
                                                        } catch (error) {
                                                            std::cerr << '[GOALS UPDATE] Error updating goal:' << error << std::endl;
                                                            sendError(;
                                                            res,
                                                            500,
                                                            'GOAL_UPDATE_ERROR',
                                                            'Error updating goal',
                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                            );
                                                        }
                                                        });

                                                        // Delete a goal
                                                        router.delete('/:agentId/goals/:goalId', async (req, res) => {
                                                            const auto agentId = validateUuid(req.params.agentId);
                                                            if (!agentId) {
                                                                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                                                            }

                                                            const auto runtime = agents.get(agentId);
                                                            if (!runtime) {
                                                                return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                                            }

                                                            const auto { goalId } = req.params;

                                                            try {
                                                                // Get the Goals service
                                                                const auto goalsService = runtime.getService('goals');
                                                                if (!goalsService) {
                                                                    return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Goals service not available');
                                                                }

                                                                // Check if goal exists
                                                                const auto existingGoal = (goalsService).getGoalById(goalId);
                                                                if (!existingGoal) {
                                                                    return sendError(res, 404, 'GOAL_NOT_FOUND', 'Goal not found');
                                                                }

                                                                // Delete the goal using the service (cancel it)
                                                                const auto success = (goalsService).cancelGoal(goalId);

                                                                if (!success) {
                                                                    return sendError(res, 500, 'GOAL_DELETE_FAILED', 'Failed to delete goal');
                                                                }

                                                                std::cout << "[GOALS DELETE] Deleted goal " + std::to_string(goalId) + " for agent " + std::to_string(runtime.character.name) << std::endl;
                                                                sendSuccess(res, { message: 'Goal deleted successfully', goalId });
                                                                } catch (error) {
                                                                    std::cerr << '[GOALS DELETE] Error deleting goal:' << error << std::endl;
                                                                    sendError(;
                                                                    res,
                                                                    500,
                                                                    'GOAL_DELETE_ERROR',
                                                                    'Error deleting goal',
                                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                                    );
                                                                }
                                                                });

                                                                return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
