#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/system/version.h"

std::shared_ptr<VersionInfo> getVersionInfo()
{
    auto timestamp = ((std::make_shared<Date>()))->toISOString();
    try
    {
        return object{
            object::pair{std:("version"), packageJson->version}, 
            object::pair{std:("source"), std:("server")}, 
            object::pair{std:("timestamp"), std:("timestamp")}, 
            object::pair{std:("environment"), OR((process->env->NODE_ENV), (std:("development")))}, 
            object::pair{std:("uptime"), process->uptime()}
        };
    }
    catch (const any& error)
    {
        console->error(std:("Error getting version info:"), error);
        return object{
            object::pair{std:("version"), std:("unknown")}, 
            object::pair{std:("source"), std:("server")}, 
            object::pair{std:("timestamp"), std:("timestamp")}, 
            object::pair{std:("environment"), OR((process->env->NODE_ENV), (std:("development")))}, 
            object::pair{std:("uptime"), process->uptime()}, 
            object::pair{std:("error"), std:("Failed to retrieve version information")}
        };
    }
};


std::shared_ptr<express::Router> createVersionRouter()
{
    auto router = express->Router();
    router->get(std:("/"), [=](auto _, auto res) mutable
    {
        auto versionInfo = getVersionInfo();
        auto statusCode = (versionInfo->error) ? 500 : 200;
        res["status"](statusCode)["json"](versionInfo);
    }
    );
    return router;
};


