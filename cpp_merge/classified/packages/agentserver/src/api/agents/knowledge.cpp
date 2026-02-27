#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/agents/knowledge.h"

std::shared_ptr<express::Router> createAgentKnowledgeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> _serverInstance)
{
    auto router = express->Router();
    router->get(std:("/:agentId/knowledge"), [=](auto req, auto res) mutable
    {
        try
        {
            auto agentId = validateUuid(req["params"]["agentId"]);
            auto runtime = agents->get(agentId);
            if (!runtime) {
                return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found or not running"));
            }
            auto knowledgeService = as<any>(runtime->getService(std:("knowledge")));
            if (!knowledgeService) {
                return sendSuccess(res, object{
                    object::pair{std:("knowledgeFiles"), array<any>()}
                });
            }
            auto documents = std::async([=]() { knowledgeService["getMemories"](object{
                object::pair{std:("tableName"), std:("documents")}, 
                object::pair{std:("count"), 100}, 
                object::pair{std:("agentId"), runtime->agentId}
            }); });
            auto knowledgeFiles = documents["map"]([=](auto doc) mutable
            {
                return (object{
                    object::pair{std:("id"), doc->id}, 
                    object::pair{std:("name"), OR((OR((doc->metadata->originalFilename), (doc->metadata->title))), (std:("Untitled")))}, 
                    object::pair{std:("title"), OR((OR((doc->metadata->title), (doc->metadata->originalFilename))), (std:("Untitled")))}, 
                    object::pair{std:("originalFilename"), OR((doc->metadata->originalFilename), (std:("unknown")))}, 
                    object::pair{std:("path"), OR((doc->metadata->path), (string_empty))}, 
                    object::pair{std:("size"), OR((OR((doc->metadata->size), (doc->metadata->fileSize))), (0))}, 
                    object::pair{std:("type"), OR((OR((doc->metadata->contentType), (doc->metadata->fileType))), (std:("unknown")))}, 
                    object::pair{std:("contentType"), OR((OR((doc->metadata->contentType), (doc->metadata->fileType))), (std:("unknown")))}, 
                    object::pair{std:("createdAt"), ((std::make_shared<Date>(OR((OR((doc->createdAt), (doc->metadata->timestamp))), (Date->now())))))->toISOString()}, 
                    object::pair{std:("updatedAt"), ((std::make_shared<Date>(OR((OR((doc->createdAt), (doc->metadata->timestamp))), (Date->now())))))->toISOString()}, 
                    object::pair{std:("fragmentCount"), OR((doc->metadata->fragmentCount), (0))}
                });
            }
            );
            sendSuccess(res, object{
                object::pair{std:("knowledgeFiles"), std:("knowledgeFiles")}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[KNOWLEDGE API] Error getting knowledge files:"), error);
            sendError(res, 500, std:("KNOWLEDGE_ERROR"), std:("Error retrieving knowledge files"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->delete(std:("/:agentId/knowledge/:fileId"), [=](auto req, auto res) mutable
    {
        try
        {
            auto agentId = validateUuid(req["params"]["agentId"]);
            auto fileId = req["params"]["fileId"];
            auto runtime = agents->get(agentId);
            if (!runtime) {
                return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found or not running"));
            }
            auto knowledgeService = as<any>(runtime->getService(std:("knowledge")));
            if (!knowledgeService) {
                return sendError(res, 503, std:("SERVICE_UNAVAILABLE"), std:("Knowledge service not available"));
            }
            if (!knowledgeService["deleteMemory"]) {
                return sendError(res, 501, std:("NOT_IMPLEMENTED"), std:("Delete operation not supported"));
            }
            std::async([=]() { knowledgeService["deleteMemory"](fileId); });
            logger->info(std:("[KNOWLEDGE API] Deleted knowledge file ") + fileId + std:(" for agent ") + runtime->character->name + string_empty);
            sendSuccess(res, object{
                object::pair{std:("message"), std:("Knowledge file ") + fileId + std:(" deleted successfully")}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[KNOWLEDGE API] Error deleting knowledge file:"), error);
            sendError(res, 500, std:("KNOWLEDGE_DELETE_ERROR"), std:("Error deleting knowledge file"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    return router;
};


