#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/api/runtime/health.h"

std::shared_ptr<express::Router> createHealthRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->get(std::string("/ping"), [=](auto _req, auto res) mutable
    {
        res["json"](object{
            object::pair{std::string("pong"), true}, 
            object::pair{std::string("timestamp"), Date->now()}
        });
    }
    );
    router->get(std::string("/hello"), [=](auto _req, auto res) mutable
    {
        logger->info(std::string("Hello endpoint hit"));
        res["setHeader"](std::string("Content-Type"), std::string("application/json"));
        res["send"](JSON->stringify(object{
            object::pair{std::string("message"), std::string("Hello World!")}
        }));
    }
    );
    router->get(std::string("/status"), [=](auto _req, auto res) mutable
    {
        logger->info(std::string("Status endpoint hit"));
        res["setHeader"](std::string("Content-Type"), std::string("application/json"));
        res["send"](JSON->stringify(object{
            object::pair{std::string("status"), std::string("ok")}, 
            object::pair{std::string("agentCount"), agents->size}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        }));
    }
    );
    router->get(std::string("/health"), [=](auto _req, auto res) mutable
    {
        logger->log(object{
            object::pair{std::string("apiRoute"), std::string("/health")}
        }, std::string("Health check route hit"));
        auto healthcheck = object{
            object::pair{std::string("status"), std::string("OK")}, 
            object::pair{std::string("version"), OR((process->env->APP_VERSION), (std::string("unknown")))}, 
            object::pair{std::string("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("agents"), (agents->size > 0) ? std::string("healthy") : std::string("no_agents")}
            }}
        };
        auto statusCode = (healthcheck["dependencies"]["agents"] == std::string("healthy")) ? 200 : 503;
        res["status"](statusCode)["json"](healthcheck);
    }
    );
    router->post(std::string("/stop"), [=](auto _req, auto res) mutable
    {
        logger->log(object{
            object::pair{std::string("apiRoute"), std::string("/stop")}
        }, std::string("Server stopping..."));
        serverInstance->stop();
        res["json"](object{
            object::pair{std::string("message"), std::string("Server stopping...")}
        });
    }
    );
    return router;
};


