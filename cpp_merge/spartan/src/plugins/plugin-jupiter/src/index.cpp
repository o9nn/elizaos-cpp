#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> jupiterPlugin = object{
    object::pair{std::string("name"), std::string("jupiterOS")}, 
    object::pair{std::string("description"), std::string("jupiter plugin")}, 
    object::pair{std::string("actions"), array<any>()}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("services"), array<JupiterService>{ JupiterService }}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std::string("jupiter init"));
        auto asking = std::string("jupiter");
        auto serviceType = std::string("solana");
        auto solanaService = as<any>(runtime->getService(serviceType));
        while (!solanaService)
        {
            console->log(asking, std::string("waiting for"), serviceType, std::string("service..."));
            solanaService = as<any>(runtime->getService(serviceType));
            if (!solanaService) {
                std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                {
                    return setTimeout(waitResolve, 1000);
                }
                ); });
            } else {
                console->log(asking, std::string("Acquired"), serviceType, std::string("service..."));
            }
        }
        auto me = object{
            object::pair{std::string("name"), std::string("Jupiter DEX services")}
        };
        solanaService["registerExchange"](me);
        console->log(std::string("jupiter init done"));
    }
    }
};

void Main(void)
{
}

MAIN
