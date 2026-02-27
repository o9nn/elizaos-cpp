#include "lifecycle.hpp"

std::shared_ptr<express::Router> createAgentLifecycleRouter(std::shared_ptr<ElizaOS> elizaOS, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    shared db = serverInstance->database;
    router->post(std:("/:agentId/start"), requireAuth, requireAdmin, [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        if (!db) {
            return sendError(res, 500, std:("DB_ERROR"), std:("Database not available"));
        }
        try
        {
            auto agent = std::async([=]() { db->getAgent(agentId); });
            if (!agent) {
                logger->debug(std:("[AGENT START] Agent not found"));
                return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
            }
            auto isActive = !!elizaOS->getAgent(agentId);
            if (isActive) {
                logger->debug(std:("[AGENT START] Agent ") + agentId + std:(" is already running"));
                return sendSuccess(res, object{
                    object::pair{std:("id"), agentId}, 
                    object::pair{std:("name"), agent->name}, 
                    object::pair{std:("status"), std:("active")}
                });
            }
            std::async([=]() { serverInstance->startAgents(array<any>{ agent }); });
            auto runtime = elizaOS->getAgent(agentId);
            if (!runtime) {
                throw any(std::make_shared<Error>(std:("Failed to start agent")));
            }
            logger->debug(std:("[AGENT START] Successfully started agent: ") + agent->name + string_empty);
            sendSuccess(res, object{
                object::pair{std:("id"), agentId}, 
                object::pair{std:("name"), agent->name}, 
                object::pair{std:("status"), std:("active")}
            });
        }
        catch (const any& error)
        {
            logger->error(std:("[AGENT START] Error starting agent:"), (is<Error>(error)) ? error->message : String(error));
            sendError(res, 500, std:("START_ERROR"), std:("Error starting agent"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->post(std:("/:agentId/stop"), requireAuth, requireAdmin, [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req->params->agentId);
        if (!agentId) {
            logger->debug(std:("[AGENT STOP] Invalid agent ID format"));
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto runtime = elizaOS->getAgent(agentId);
        if (!runtime) {
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
        }
        std::async([=]() { serverInstance->unregisterAgent(agentId); });
        logger->debug(std:("[AGENT STOP] Successfully stopped agent: ") + runtime->character->name + std:(" (") + agentId + std:(")"));
        sendSuccess(res, object{
            object::pair{std:("message"), std:("Agent stopped")}
        });
    }
    );
    return router;
};


