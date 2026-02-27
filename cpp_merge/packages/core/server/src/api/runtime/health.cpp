#include "health.hpp"

std::shared_ptr<express::Router> createHealthRouter(std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>> agents, std::shared_ptr<AgentServer> serverInstance)
{
    auto router = express->Router();
    router->get(std:("/ping"), [=](auto _req, auto res) mutable
    {
        res["json"](object{
            object::pair{std:("pong"), true}, 
            object::pair{std:("timestamp"), Date->now()}
        });
    }
    );
    router->get(std:("/hello"), [=](auto _req, auto res) mutable
    {
        logger->info(std:("Hello endpoint hit"));
        res["setHeader"](std:("Content-Type"), std:("application/json"));
        res["send"](JSON->stringify(object{
            object::pair{std:("message"), std:("Hello World!")}
        }));
    }
    );
    router->get(std:("/status"), [=](auto _req, auto res) mutable
    {
        logger->info(std:("Status endpoint hit"));
        res["setHeader"](std:("Content-Type"), std:("application/json"));
        res["send"](JSON->stringify(object{
            object::pair{std:("status"), std:("ok")}, 
            object::pair{std:("agentCount"), agents->size}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        }));
    }
    );
    router->get(std:("/health"), [=](auto _req, auto res) mutable
    {
        logger->log(object{
            object::pair{std:("apiRoute"), std:("/health")}
        }, std:("Health check route hit"));
        auto healthcheck = object{
            object::pair{std:("status"), std:("OK")}, 
            object::pair{std:("version"), OR((process->env->APP_VERSION), (std:("unknown")))}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("agents"), (agents->size > 0) ? std:("healthy") : std:("no_agents")}
            }}
        };
        auto statusCode = (healthcheck["dependencies"]["agents"] == std:("healthy")) ? 200 : 503;
        res["status"](statusCode)["json"](healthcheck);
    }
    );
    router->post(std:("/stop"), [=](auto _req, auto res) mutable
    {
        logger->log(object{
            object::pair{std:("apiRoute"), std:("/stop")}
        }, std:("Server stopping..."));
        serverInstance->stop();
        res["json"](object{
            object::pair{std:("message"), std:("Server stopping...")}
        });
    }
    );
    return router;
};


