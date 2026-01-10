#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/agents/lifecycle.h"

std::shared_ptr<express::Router> createAgentLifecycleRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    shared db = serverInstance->database;
    router->post(std::string("/:agentId/start"), [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        if (!db) {
            return sendError(res, 500, std::string("DB_ERROR"), std::string("Database not available"));
        }
        try
        {
            auto agent = std::async([=]() { db->getAgent(agentId); });
            if (!agent) {
                logger->debug(std::string("[AGENT START] Agent not found"));
                return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
            }
            auto isActive = !!agents->get(agentId);
            if (isActive) {
                logger->debug(std::string("[AGENT START] Agent ") + agentId + std::string(" is already running"));
                return sendSuccess(res, object{
                    object::pair{std::string("id"), agentId}, 
                    object::pair{std::string("name"), agent->name}, 
                    object::pair{std::string("status"), std::string("active")}
                });
            }
            std::async([=]() { serverInstance->startAgent(agent); });
            auto runtime = agents->get(agentId);
            if (!runtime) {
                throw any(std::make_shared<Error>(std::string("Failed to start agent")));
            }
            logger->debug(std::string("[AGENT START] Successfully started agent: ") + agent->name + string_empty);
            sendSuccess(res, object{
                object::pair{std::string("id"), agentId}, 
                object::pair{std::string("name"), agent->name}, 
                object::pair{std::string("status"), std::string("active")}
            });
        }
        catch (const any& error)
        {
            logger->error(std::string("[AGENT START] Error starting agent:"), error);
            sendError(res, 500, std::string("START_ERROR"), std::string("Error starting agent"), (is<Error>(error)) ? error->message : String(error));
        }
    }
    );
    router->post(std::string("/:agentId/stop"), [=](auto req, auto res) mutable
    {
        auto agentId = validateUuid(req["params"]["agentId"]);
        if (!agentId) {
            logger->debug(std::string("[AGENT STOP] Invalid agent ID format"));
            return sendError(res, 400, std::string("INVALID_ID"), std::string("Invalid agent ID format"));
        }
        auto runtime = agents->get(agentId);
        if (!runtime) {
            return sendError(res, 404, std::string("NOT_FOUND"), std::string("Agent not found"));
        }
        serverInstance->unregisterAgent(agentId);
        logger->debug(std::string("[AGENT STOP] Successfully stopped agent: ") + runtime->character->name + std::string(" (") + agentId + std::string(")"));
        sendSuccess(res, object{
            object::pair{std::string("message"), std::string("Agent stopped")}
        });
    }
    );
    return router;
};


