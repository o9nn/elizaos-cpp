#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/agents/knowledge.h"

std::shared_ptr<express::Router> createAgentKnowledgeRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> _serverInstance)
{
    auto router = express->Router();
    router->get(std::string("/:agentId/knowledge"), [=](auto req, auto res) mutable
    {
        try
        {
            auto agentId = validateUuid(req["params"]["agentId"]);
            auto runtime = agents->get(agentId);
            if (!runtime) {
                return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found or not running"));
            }
            auto knowledgeService = as<any>(runtime->getService(std::string("knowledge")));
            if (!knowledgeService) {
                return sendSuccess(res, object{
                    object::pair{std::string("knowledgeFiles"), array<any>()}
                });
            }
            auto documents = std::async([=]() { knowledgeService["getMemories"](object{
                object::pair{std::string("tableName"), std::string("documents")}, 
                object::pair{std::string("count"), 100}, 
                object::pair{std::string("agentId"), runtime->agentId}
            }); });
            auto knowledgeFiles = documents["map"]([=](auto doc) mutable
            {
                return (object{
                    object::pair{std::string("id"), doc->id}, 
                    object::pair{std::string("name"), OR((OR((doc->metadata->originalFilename), (doc->metadata->title))), (std::string("Untitled")))}, 
                    object::pair{std::string("title"), OR((OR((doc->metadata->title), (doc->metadata->originalFilename))), (std::string("Untitled")))}, 
                    object::pair{std::string("originalFilename"), OR((doc->metadata->originalFilename), (std::string("unknown")))}, 
                    object::pair{std::string("path"), OR((doc->metadata->path), (string_empty))}, 
                    object::pair{std::string("size"), OR((OR((doc->metadata->size), (doc->metadata->fileSize))), (0))}, 
                    object::pair{std::string("type"), OR((OR((doc->metadata->contentType), (doc->metadata->fileType))), (std::string("unknown")))}, 
                    object::pair{std::string("contentType"), OR((OR((doc->metadata->contentType), (doc->metadata->fileType))), (std::string("unknown")))}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>(OR((OR((doc->createdAt), (doc->metadata->timestamp))), (Date->now())))))->toISOString()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>(OR((OR((doc->createdAt), (doc->metadata->timestamp))), (Date->now())))))->toISOString()}, 
                    object::pair{std::string("fragmentCount"), OR((doc->metadata->fragmentCount), (0))}
                });
            }
            );
            sendSuccess(res, object{
                object::pair{std::string("knowledgeFiles"), std::string("knowledgeFiles")}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[KNOWLEDGE API] Error getting knowledge files:"), error);
            sendError(res, 500, std::string("KNOWLEDGE_ERROR"), std::string("Error retrieving knowledge files"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->delete(std::string("/:agentId/knowledge/:fileId"), [=](auto req, auto res) mutable
    {
        try
        {
            auto agentId = validateUuid(req["params"]["agentId"]);
            auto fileId = req["params"]["fileId"];
            auto runtime = agents->get(agentId);
            if (!runtime) {
                return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found or not running"));
            }
            auto knowledgeService = as<any>(runtime->getService(std::string("knowledge")));
            if (!knowledgeService) {
                return sendError(res, 503, std::string("SERVICE_UNAVAILABLE"), std::string("Knowledge service not available"));
            }
            if (!knowledgeService["deleteMemory"]) {
                return sendError(res, 501, std::string("NOT_IMPLEMENTED"), std::string("Delete operation not supported"));
            }
            std::async([=]() { knowledgeService["deleteMemory"](fileId); });
            logger->info(std::string("[KNOWLEDGE API] Deleted knowledge file ") + fileId + std::string(" for agent ") + runtime->character->name + string_empty);
            sendSuccess(res, object{
                object::pair{std::string("message"), std::string("Knowledge file ") + fileId + std::string(" deleted successfully")}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[KNOWLEDGE API] Error deleting knowledge file:"), error);
            sendError(res, 500, std::string("KNOWLEDGE_DELETE_ERROR"), std::string("Error deleting knowledge file"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    return router;
};


