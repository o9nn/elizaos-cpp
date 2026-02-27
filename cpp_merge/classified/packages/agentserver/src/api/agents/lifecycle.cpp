#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/agents/lifecycle.h"

std::shared_ptr<express::Router> createAgentLifecycleRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    shared db = serverInstance->database;
    router->post(std:("/:agentId/start"), [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req["params"]["agentId"]);
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
            auto isActive = !!agents->get(agentId);
            if (isActive) {
                logger->debug(std:("[AGENT START] Agent ") + agentId + std:(" is already running"));
                return sendSuccess(res, object{
                    object::pair{std:("id"), agentId}, 
                    object::pair{std:("name"), agent->name}, 
                    object::pair{std:("status"), std:("active")}
                });
            }
            std::async([=]() { serverInstance->startAgent(agent); });
            auto runtime = agents->get(agentId);
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
            logger->error(std:("[AGENT START] Error starting agent:"), error);
            sendError(res, 500, std:("START_ERROR"), std:("Error starting agent"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->post(std:("/:agentId/stop"), [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            logger->debug(std:("[AGENT STOP] Invalid agent ID format"));
            return sendError(res, 400, std:("INVALID_ID"), std:("Invalid agent ID format"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            return sendError(res, 404, std:("NOT_FOUND"), std:("Agent not found"));
        }
        serverInstance->unregisterAgent(agentId);
        logger->debug(std:("[AGENT STOP] Successfully stopped agent: ") + runtime->character->name + std:(" (") + agentId + std:(")"));
        sendSuccess(res, object{
            object::pair{std:("message"), std:("Agent stopped")}
        });
    }
    );
    return router;
};


