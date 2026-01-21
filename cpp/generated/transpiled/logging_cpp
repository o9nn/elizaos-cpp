#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/runtime/logging.h"

std::shared_ptr<express::Router> createLoggingRouter()
{
    auto router = express->Router();
    auto logsHandler = [=](auto req, auto res) mutable
    {
        shared since = (req->query->since) ? Number(req->query->since) : Date->now() - 3600000;
        shared requestedLevel = as<LogLevel>((OR((req->query->level->toString()->toLowerCase()), (std::string("all")))));
        shared requestedAgentName = OR((req->query->agentName->toString()), (std::string("all")));
        shared requestedAgentId = OR((req->query->agentId->toString()), (std::string("all")));
        auto limit = Math->min(OR((Number(req->query->limit)), (100)), 1000);
        auto destination = const_((as<any>(logger)))[Symbol->for(std::string("pino-destination"))];
        if (!destination["recentLogs"]) {
            return res->status(500)->json(object{
                object::pair{std::string("error"), std::string("Logger destination not available")}, 
                object::pair{std::string("message"), std::string("The logger is not configured to maintain recent logs")}
            });
        }
        try
        {
            auto recentLogs = destination["recentLogs"]();
            shared requestedLevelValue = (requestedLevel == std::string("all")) ? any(0) : any(OR((const_(LOG_LEVELS)[as<any>(requestedLevel)]), (LOG_LEVELS["info"])));
            auto logsWithAgentNames = recentLogs->filter([=](auto l) mutable
            {
                return l->agentName;
            }
            )->get_length();
            auto logsWithAgentIds = recentLogs->filter([=](auto l) mutable
            {
                return l->agentId;
            }
            )->get_length();
            auto totalLogs = recentLogs->get_length();
            auto agentNamePopulationRate = (totalLogs > 0) ? any(logsWithAgentNames / totalLogs) : any(0);
            auto agentIdPopulationRate = (totalLogs > 0) ? any(logsWithAgentIds / totalLogs) : any(0);
            shared isAgentNameDataSparse = agentNamePopulationRate < 0.1;
            shared isAgentIdDataSparse = agentIdPopulationRate < 0.1;
            auto filtered = recentLogs->filter([=](auto log) mutable
            {
                auto timeMatch = log->time >= since;
                auto levelMatch = true;
                if (AND((requestedLevel), (requestedLevel != std::string("all")))) {
                    levelMatch = log->level == requestedLevelValue;
                }
                auto agentNameMatch = true;
                if (AND((requestedAgentName), (requestedAgentName != std::string("all")))) {
                    if (log->agentName) {
                        agentNameMatch = log->agentName == requestedAgentName;
                    } else {
                        agentNameMatch = isAgentNameDataSparse;
                    }
                }
                auto agentIdMatch = true;
                if (AND((requestedAgentId), (requestedAgentId != std::string("all")))) {
                    if (log->agentId) {
                        agentIdMatch = log->agentId == requestedAgentId;
                    } else {
                        agentIdMatch = isAgentIdDataSparse;
                    }
                }
                return AND((AND((AND((timeMatch), (levelMatch))), (agentNameMatch))), (agentIdMatch));
            }
            )->slice(-limit);
            logger->debug(std::string("Logs request processed"), object{
                object::pair{std::string("requestedLevel"), std::string("requestedLevel")}, 
                object::pair{std::string("requestedLevelValue"), std::string("requestedLevelValue")}, 
                object::pair{std::string("requestedAgentName"), std::string("requestedAgentName")}, 
                object::pair{std::string("requestedAgentId"), std::string("requestedAgentId")}, 
                object::pair{std::string("filteredCount"), filtered->get_length()}, 
                object::pair{std::string("totalLogs"), recentLogs->get_length()}, 
                object::pair{std::string("logsWithAgentNames"), std::string("logsWithAgentNames")}, 
                object::pair{std::string("logsWithAgentIds"), std::string("logsWithAgentIds")}, 
                object::pair{std::string("agentNamePopulationRate"), Math->round(agentNamePopulationRate * 100) + std::string("%")}, 
                object::pair{std::string("agentIdPopulationRate"), Math->round(agentIdPopulationRate * 100) + std::string("%")}, 
                object::pair{std::string("isAgentNameDataSparse"), std::string("isAgentNameDataSparse")}, 
                object::pair{std::string("isAgentIdDataSparse"), std::string("isAgentIdDataSparse")}, 
                object::pair{std::string("sampleLogAgentNames"), recentLogs->slice(0, 5)->map([=](auto log) mutable
                {
                    return log->agentName;
                }
                )}, 
                object::pair{std::string("uniqueAgentNamesInLogs"), (array<any>{ std::make_shared<Set>(recentLogs->map([=](auto log) mutable
                {
                    return log->agentName;
                }
                )) })->filter(Boolean)}, 
                object::pair{std::string("exactAgentNameMatches"), recentLogs->filter([=](auto log) mutable
                {
                    return log->agentName == requestedAgentName;
                }
                )->get_length()}
            });
            res->json(object{
                object::pair{std::string("logs"), filtered}, 
                object::pair{std::string("count"), filtered->get_length()}, 
                object::pair{std::string("total"), recentLogs->get_length()}, 
                object::pair{std::string("requestedLevel"), requestedLevel}, 
                object::pair{std::string("agentName"), requestedAgentName}, 
                object::pair{std::string("agentId"), requestedAgentId}, 
                object::pair{std::string("levels"), Object->keys(LOG_LEVELS)}
            });
        }
        catch (const any& error)
        {
            res->status(500)->json(object{
                object::pair{std::string("error"), std::string("Failed to retrieve logs")}, 
                object::pair{std::string("message"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
            });
        }
    };
    (as<any>(router))["get"](std::string("/logs"), logsHandler);
    (as<any>(router))["post"](std::string("/logs"), logsHandler);
    auto logsClearHandler = [=](auto _req, auto res) mutable
    {
        try
        {
            auto destination = const_((as<any>(logger)))[Symbol->for(std::string("pino-destination"))];
            if (!destination["clear"]) {
                return res->status(500)->json(object{
                    object::pair{std::string("error"), std::string("Logger clear method not available")}, 
                    object::pair{std::string("message"), std::string("The logger is not configured to clear logs")}
                });
            }
            destination["clear"]();
            logger->debug(std::string("Logs cleared via API endpoint"));
            res->json(object{
                object::pair{std::string("status"), std::string("success")}, 
                object::pair{std::string("message"), std::string("Logs cleared successfully")}
            });
        }
        catch (const any& error)
        {
            res->status(500)->json(object{
                object::pair{std::string("error"), std::string("Failed to clear logs")}, 
                object::pair{std::string("message"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
            });
        }
    };
    (as<any>(router))["delete"](std::string("/logs"), logsClearHandler);
    return router;
};


object LOG_LEVELS = as<std::shared_ptr<const>>(object{
    object::pair{std::string("fatal"), 60}, 
    object::pair{std::string("error"), 50}, 
    object::pair{std::string("warn"), 40}, 
    object::pair{std::string("info"), 30}, 
    object::pair{std::string("log"), 29}, 
    object::pair{std::string("progress"), 28}, 
    object::pair{std::string("success"), 27}, 
    object::pair{std::string("debug"), 20}, 
    object::pair{std::string("trace"), 10}
});

void Main(void)
{
}

MAIN
