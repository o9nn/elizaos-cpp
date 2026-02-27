#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/runtime/debug.h"

std::shared_ptr<express::Router> createDebugRouter(std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->get(std::string("/servers"), [=](auto _req, auto res) mutable
    {
        try
        {
            auto servers = std::async([=]() { serverInstance->getServers(); });
            res["json"](object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("servers"), OR((servers), (array<any>()))}, 
                object::pair{std::string("count"), OR((servers->get_length()), (0))}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
            });
        }
    }
    );
    return router;
};


