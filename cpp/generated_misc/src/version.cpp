#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/api/system/version.h"

std::shared_ptr<VersionInfo> getVersionInfo()
{
    auto timestamp = ((std::make_shared<Date>()))->toISOString();
    try
    {
        return object{
            object::pair{std::string("version"), packageJson->version}, 
            object::pair{std::string("source"), std::string("server")}, 
            object::pair{std::string("timestamp"), std::string("timestamp")}, 
            object::pair{std::string("environment"), OR((process->env->NODE_ENV), (std::string("development")))}, 
            object::pair{std::string("uptime"), process->uptime()}
        };
    }
    catch (const any& error)
    {
        console->error(std::string("Error getting version info:"), error);
        return object{
            object::pair{std::string("version"), std::string("unknown")}, 
            object::pair{std::string("source"), std::string("server")}, 
            object::pair{std::string("timestamp"), std::string("timestamp")}, 
            object::pair{std::string("environment"), OR((process->env->NODE_ENV), (std::string("development")))}, 
            object::pair{std::string("uptime"), process->uptime()}, 
            object::pair{std::string("error"), std::string("Failed to retrieve version information")}
        };
    }
};


std::shared_ptr<express::Router> createVersionRouter()
{
    auto router = express->Router();
    router->get(std::string("/"), [=](auto _, auto res) mutable
    {
        auto versionInfo = getVersionInfo();
        auto statusCode = (versionInfo->error) ? 500 : 200;
        res["status"](statusCode)["json"](versionInfo);
    }
    );
    return router;
};


