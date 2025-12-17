#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router entitiesRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // GET /entities/:entityId - Get entity by ID
        // SECURITY: Users can only get their own entity unless they're admin
        router.get('/:entityId', requireAuth, async (req: AuthenticatedRequest, res) => {
            const auto entityId = validateUuid(req.params.entityId);
            if (!entityId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid entity ID format');
            }

            if (!db) {
                return sendError(res, 500, 'DB_ERROR', 'Database not available');
            }

            // SECURITY: Check if user is requesting their own entity or is admin
            if (!req.isAdmin && entityId != req.userId) {
                std::cout << "[ENTITY GET] User " + std::to_string(req.userId) + " attempted to access entity " + std::to_string(entityId) << std::endl;
                return sendError(res, 403, 'FORBIDDEN', 'You can only access your own entity');
            }

            try {
                const auto entities = db.getEntitiesByIds([entityId]);

                if (!entities || entities.length == 0) {
                    return sendError(res, 404, 'NOT_FOUND', 'Entity not found');
                }

                sendSuccess(res, { entity: entities[0] });
                } catch (error) {
                    logger.error(
                    '[ENTITY GET] Error retrieving entity:',
                    true /* instanceof check */ ? error.message : std::to_string(error)
                    );
                    sendError(;
                    res,
                    500,
                    'INTERNAL_ERROR',
                    'Error retrieving entity',
                    true /* instanceof check */ ? error.message : std::to_string(error)
                    );
                }
                });

                // POST /entities - Create a new entity
                // SECURITY: Users can only create entities for themselves unless they're admin
                router.post('/', requireAuth, async (req: AuthenticatedRequest, res) => {
                    const auto { id, agentId, names, metadata } = req.body;

                    if (!id) {
                        return sendError(res, 400, 'INVALID_REQUEST', 'Entity ID is required');
                    }

                    const auto entityId = validateUuid(id);
                    if (!entityId) {
                        return sendError(res, 400, 'INVALID_ID', 'Invalid entity ID format');
                    }

                    // SECURITY: Check if user is creating their own entity or is admin
                    if (!req.isAdmin && entityId != req.userId) {
                        std::cout << "[ENTITY CREATE] User " + std::to_string(req.userId) + " attempted to create entity " + std::to_string(entityId) << std::endl;
                        return sendError(res, 403, 'FORBIDDEN', 'You can only create your own entity');
                    }

                    if (!agentId) {
                        return sendError(res, 400, 'INVALID_REQUEST', 'Agent ID is required');
                    }

                    const auto validAgentId = validateUuid(agentId);
                    if (!validAgentId) {
                        return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
                    }

                    if (!db) {
                        return sendError(res, 500, 'DB_ERROR', 'Database not available');
                    }

                    try {
                        const Entity entity = {;
                            id: entityId,
                            agentId: validAgentId,
                            names: names || [],
                            metadata: metadata || {},
                            };

                            const auto result = db.createEntities([entity]);

                            if (!result) {
                                return sendError(res, 500, 'CREATE_FAILED', 'Failed to create entity');
                            }

                            sendSuccess(res, { entity }, 201);
                            } catch (error) {
                                logger.error(
                                '[ENTITY CREATE] Error creating entity:',
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                                sendError(;
                                res,
                                500,
                                'INTERNAL_ERROR',
                                'Error creating entity',
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                            }
                            });

                            // PATCH /entities/:entityId - Update an entity
                            // SECURITY: Users can only update their own entity unless they're admin
                            router.patch('/:entityId', requireAuth, async (req: AuthenticatedRequest, res) => {
                                const auto entityId = validateUuid(req.params.entityId);
                                if (!entityId) {
                                    return sendError(res, 400, 'INVALID_ID', 'Invalid entity ID format');
                                }

                                // SECURITY: Check if user is updating their own entity or is admin
                                if (!req.isAdmin && entityId != req.userId) {
                                    std::cout << "[ENTITY UPDATE] User " + std::to_string(req.userId) + " attempted to update entity " + std::to_string(entityId) << std::endl;
                                    return sendError(res, 403, 'FORBIDDEN', 'You can only update your own entity');
                                }

                                if (!db) {
                                    return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                }

                                try {
                                    // First, check if entity exists
                                    const auto existing = db.getEntitiesByIds([entityId]);

                                    if (!existing || existing.length == 0) {
                                        return sendError(res, 404, 'NOT_FOUND', 'Entity not found');
                                    }

                                    const auto existingEntity = existing[0];

                                    // Merge updates with existing entity
                                    const Entity updatedEntity = {;
                                        ...existingEntity,
                                        ...req.body,
                                        id: entityId, // Ensure ID doesn't change
                                        };

                                        db.updateEntity(updatedEntity);

                                        // Fetch updated entity
                                        const auto updated = db.getEntitiesByIds([entityId]);

                                        sendSuccess(res, { entity: updated.[0] });
                                        } catch (error) {
                                            logger.error(
                                            '[ENTITY UPDATE] Error updating entity:',
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                            sendError(;
                                            res,
                                            500,
                                            'INTERNAL_ERROR',
                                            'Error updating entity',
                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                            );
                                        }
                                        });

                                        // DELETE /entities/:entityId - Delete an entity
                                        // TODO: Uncomment when deleteEntity is added to DatabaseAdapter interface
                                        /*
                                        router.delete('/:entityId', async (req, res) => {
                                            const auto entityId = validateUuid(req.params.entityId);
                                            if (!entityId) {
                                                return sendError(res, 400, 'INVALID_ID', 'Invalid entity ID format');
                                            }

                                            if (!db) {
                                                return sendError(res, 500, 'DB_ERROR', 'Database not available');
                                            }

                                            try {
                                                db.deleteEntity(entityId);
                                                sendSuccess(res, { success: true });
                                                } catch (error) {
                                                    logger.error(
                                                    '[ENTITY DELETE] Error deleting entity:',
                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                    );
                                                    sendError(;
                                                    res,
                                                    500,
                                                    'INTERNAL_ERROR',
                                                    'Error deleting entity',
                                                    true /* instanceof check */ ? error.message : std::to_string(error)
                                                    );
                                                }
                                                });
                                                */

                                                return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
