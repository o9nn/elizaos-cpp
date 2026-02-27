#include "index.hpp"

std::shared_ptr<Plugin> jupiterPlugin = object{
    object::pair{std:("name"), std:("jupiterOS")}, 
    object::pair{std:("description"), std:("jupiter plugin")}, 
    object::pair{std:("actions"), array<any>()}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("services"), array<JupiterService>{ JupiterService }}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std:("jupiter init"));
        auto asking = std:("jupiter");
        auto serviceType = std:("solana");
        auto solanaService = as<any>(runtime->getService(serviceType));
        while (!solanaService)
        {
            console->log(asking, std:("waiting for"), serviceType, std:("service..."));
            solanaService = as<any>(runtime->getService(serviceType));
            if (!solanaService) {
                std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                {
                    return setTimeout(waitResolve, 1000);
                }
                ); });
            } else {
                console->log(asking, std:("Acquired"), serviceType, std:("service..."));
            }
        }
        auto me = object{
            object::pair{std:("name"), std:("Jupiter DEX services")}
        };
        solanaService["registerExchange"](me);
        console->log(std:("jupiter init done"));
    }
    }
};

void Main(void)
{
}

MAIN
