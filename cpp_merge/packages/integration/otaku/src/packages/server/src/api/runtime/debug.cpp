#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/src/api/runtime/debug.h"

std::shared_ptr<express::Router> createDebugRouter(std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->use(requireAuth, requireAdmin);
    router->get(std:("/servers"), [=](auto _req, auto res) mutable
    {
        try
        {
            auto servers = std::async([=]() { serverInstance->getServers(); });
            res["json"](object{
                object::pair{std:("success"), true}, 
                object::pair{std:("servers"), OR((servers), (array<any>()))}, 
                object::pair{std:("count"), OR((servers->get_length()), (0))}
            });
        }
        catch (const any& error)
        {
            res["status"](500)["json"](object{
                object::pair{std:("success"), false}, 
                object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
            });
        }
    }
    );
    return router;
};


