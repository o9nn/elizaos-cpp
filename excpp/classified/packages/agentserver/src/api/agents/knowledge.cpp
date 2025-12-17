#include "knowledge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentKnowledgeRouter(Map<UUID agents, auto IAgentRuntime>, AgentServer _serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get knowledge files for a specific agent
        router.get('/:agentId/knowledge', async (req, res) => {
            try {
                const auto agentId = validateUuid(req.params.agentId);
                const auto runtime = agents.get(agentId);

                if (!runtime) {
                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                }

                const auto knowledgeService = runtime.getService('knowledge') | nullptr;
                if (!knowledgeService) {
                    return sendSuccess(res, { knowledgeFiles: [] });
                }

                // Get documents from the knowledge service
                const auto documents = knowledgeService.getMemories({;
                    tableName: 'documents',
                    count: 100,
                    agentId: runtime.agentId,
                    });

                    // Format documents as knowledge files for consistent API response
                    const auto knowledgeFiles = documents.map((doc: KnowledgeItem) => ({;
                        id: doc.id,
                        name: doc.metadata.originalFilename || doc.metadata.title || 'Untitled',
                        title: doc.metadata.title || doc.metadata.originalFilename || 'Untitled',
                        originalFilename: doc.metadata.originalFilename || 'unknown',
                        path: doc.metadata.path || '',
                        size: doc.metadata.size || doc.metadata.fileSize || 0,
                        type: doc.metadata.contentType || doc.metadata.fileType || 'unknown',
                        contentType: doc.metadata.contentType || doc.metadata.fileType || 'unknown',
                        createdAt: new Date(doc.createdAt || doc.metadata.timestamp || Date.now()).toISOString(),
                        updatedAt: new Date(doc.createdAt || doc.metadata.timestamp || Date.now()).toISOString(),
                        fragmentCount: doc.metadata.fragmentCount || 0,
                        }));

                        sendSuccess(res, { knowledgeFiles });
                        } catch (error) {
                            std::cerr << '[KNOWLEDGE API] Error getting knowledge files:' << error << std::endl;
                            sendError(;
                            res,
                            500,
                            'KNOWLEDGE_ERROR',
                            'Error retrieving knowledge files',
                            true /* instanceof check */ ? error.message : std::to_string(error)
                            );
                        }
                        });

                        // Delete a knowledge file
                        router.delete('/:agentId/knowledge/:fileId', async (req, res) => {
                            try {
                                const auto agentId = validateUuid(req.params.agentId);
                                const auto fileId = req.params.fileId;

                                const auto runtime = agents.get(agentId);
                                if (!runtime) {
                                    return sendError(res, 404, 'NOT_FOUND', 'Agent not found or not running');
                                }

                                const auto knowledgeService = runtime.getService('knowledge') | nullptr;
                                if (!knowledgeService) {
                                    return sendError(res, 503, 'SERVICE_UNAVAILABLE', 'Knowledge service not available');
                                }

                                if (!knowledgeService.deleteMemory) {
                                    return sendError(res, 501, 'NOT_IMPLEMENTED', 'Delete operation not supported');
                                }

                                // Delete the knowledge document using the service
                                knowledgeService.deleteMemory(fileId);

                                logger.info(
                                "[KNOWLEDGE API] Deleted knowledge file " + std::to_string(fileId) + " for agent " + std::to_string(runtime.character.name);
                                );
                                "Knowledge file " + std::to_string(fileId) + " deleted successfully"
                                } catch (error) {
                                    std::cerr << '[KNOWLEDGE API] Error deleting knowledge file:' << error << std::endl;
                                    sendError(;
                                    res,
                                    500,
                                    'KNOWLEDGE_DELETE_ERROR',
                                    'Error deleting knowledge file',
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
