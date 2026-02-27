#include "logging.hpp"

std::shared_ptr<express::Router> createLoggingRouter()
{
    auto router = express->Router();
    auto logsHandler = [=](auto req, auto res) mutable
    {
        shared since = (req->query->since) ? Number(req->query->since) : Date->now() - 3600000;
        shared requestedLevel = as<LogLevel>((OR((req->query->level->toString()->toLowerCase()), (std:("all")))));
        shared requestedAgentName = OR((req->query->agentName->toString()), (std:("all")));
        shared requestedAgentId = OR((req->query->agentId->toString()), (std:("all")));
        auto limit = Math->min(OR((Number(req->query->limit)), (100)), 1000);
        auto destination = const_((as<any>(logger)))[Symbol->for(std:("pino-destination"))];
        if (!destination["recentLogs"]) {
            return res->status(500)->json(object{
                object::pair{std:("error"), std:("Logger destination not available")}, 
                object::pair{std:("message"), std:("The logger is not configured to maintain recent logs")}
            });
        }
        try
        {
            auto recentLogs = destination["recentLogs"]();
            shared requestedLevelValue = (requestedLevel == std:("all")) ? any(0) (OR((const_(LOG_LEVELS)[as<any>(requestedLevel)]), (LOG_LEVELS["info"])));
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
            auto agentNamePopulationRate = (totalLogs > 0) ? any(logsWithAgentNames / totalLogs) (0);
            auto agentIdPopulationRate = (totalLogs > 0) ? any(logsWithAgentIds / totalLogs) (0);
            shared isAgentNameDataSparse = agentNamePopulationRate < 0.1;
            shared isAgentIdDataSparse = agentIdPopulationRate < 0.1;
            auto filtered = recentLogs->filter([=](auto log) mutable
            {
                auto timeMatch = log->time >= since;
                auto levelMatch = true;
                if (AND((requestedLevel), (requestedLevel != std:("all")))) {
                    auto logLevel = (type_of(log->level) == std:("number")) ? any(log->level) (OR((const_(LOG_LEVELS)[as<any>(log->level)]), (30)));
                    levelMatch = logLevel == requestedLevelValue;
                }
                auto agentNameMatch = true;
                if (AND((requestedAgentName), (requestedAgentName != std:("all")))) {
                    if (log->agentName) {
                        agentNameMatch = log->agentName == requestedAgentName;
                    } else {
                        agentNameMatch = isAgentNameDataSparse;
                    }
                }
                auto agentIdMatch = true;
                if (AND((requestedAgentId), (requestedAgentId != std:("all")))) {
                    if (log->agentId) {
                        agentIdMatch = log->agentId == requestedAgentId;
                    } else {
                        agentIdMatch = isAgentIdDataSparse;
                    }
                }
                return AND((AND((AND((timeMatch), (levelMatch))), (agentNameMatch))), (agentIdMatch));
            }
            )->slice(-limit);
            logger->debug(std:("Logs request processed"), object{
                object::pair{std:("requestedLevel"), std:("requestedLevel")}, 
                object::pair{std:("requestedLevelValue"), std:("requestedLevelValue")}, 
                object::pair{std:("requestedAgentName"), std:("requestedAgentName")}, 
                object::pair{std:("requestedAgentId"), std:("requestedAgentId")}, 
                object::pair{std:("filteredCount"), filtered->get_length()}, 
                object::pair{std:("totalLogs"), recentLogs->get_length()}, 
                object::pair{std:("logsWithAgentNames"), std:("logsWithAgentNames")}, 
                object::pair{std:("logsWithAgentIds"), std:("logsWithAgentIds")}, 
                object::pair{std:("agentNamePopulationRate"), string_empty + Math->round(agentNamePopulationRate * 100) + std:("%")}, 
                object::pair{std:("agentIdPopulationRate"), string_empty + Math->round(agentIdPopulationRate * 100) + std:("%")}, 
                object::pair{std:("isAgentNameDataSparse"), std:("isAgentNameDataSparse")}, 
                object::pair{std:("isAgentIdDataSparse"), std:("isAgentIdDataSparse")}, 
                object::pair{std:("sampleLogAgentNames"), recentLogs->slice(0, 5)->map([=](auto log) mutable
                {
                    return log->agentName;
                }
                )}, 
                object::pair{std:("uniqueAgentNamesInLogs"), (array<any>{ std::make_shared<Set>(recentLogs->map([=](auto log) mutable
                {
                    return log->agentName;
                }
                )) })->filter(Boolean)}, 
                object::pair{std:("exactAgentNameMatches"), recentLogs->filter([=](auto log) mutable
                {
                    return log->agentName == requestedAgentName;
                }
                )->get_length()}
            });
            res->json(object{
                object::pair{std:("logs"), filtered}, 
                object::pair{std:("count"), filtered->get_length()}, 
                object::pair{std:("total"), recentLogs->get_length()}, 
                object::pair{std:("requestedLevel"), std:("requestedLevel")}, 
                object::pair{std:("agentName"), requestedAgentName}, 
                object::pair{std:("agentId"), requestedAgentId}, 
                object::pair{std:("levels"), Object->keys(LOG_LEVELS)}
            });
        }
        catch (const any& error)
        {
            res->status(500)->json(object{
                object::pair{std:("error"), std:("Failed to retrieve logs")}, 
                object::pair{std:("message"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
            });
        }
    };
    (as<any>(router))["get"](std:("/logs"), logsHandler);
    (as<any>(router))["post"](std:("/logs"), logsHandler);
    auto logsClearHandler = [=](auto _req, auto res) mutable
    {
        try
        {
            auto destination = const_((as<any>(logger)))[Symbol->for(std:("pino-destination"))];
            if (!destination["clear"]) {
                return res->status(500)->json(object{
                    object::pair{std:("error"), std:("Logger clear method not available")}, 
                    object::pair{std:("message"), std:("The logger is not configured to clear logs")}
                });
            }
            destination["clear"]();
            logger->debug(std:("Logs cleared via API endpoint"));
            res->json(object{
                object::pair{std:("status"), std:("success")}, 
                object::pair{std:("message"), std:("Logs cleared successfully")}
            });
        }
        catch (const any& error)
        {
            res->status(500)->json(object{
                object::pair{std:("error"), std:("Failed to clear logs")}, 
                object::pair{std:("message"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
            });
        }
    };
    (as<any>(router))["delete"](std:("/logs"), logsClearHandler);
    return router;
};


object LOG_LEVELS = as<std::shared_ptr<const>>(object{
    object::pair{std:("fatal"), 60}, 
    object::pair{std:("error"), 50}, 
    object::pair{std:("warn"), 40}, 
    object::pair{std:("info"), 30}, 
    object::pair{std:("log"), 29}, 
    object::pair{std:("progress"), 28}, 
    object::pair{std:("success"), 27}, 
    object::pair{std:("debug"), 20}, 
    object::pair{std:("trace"), 10}, 
    object::pair{std:("verbose"), 0}
});

void Main(void)
{
}

MAIN
