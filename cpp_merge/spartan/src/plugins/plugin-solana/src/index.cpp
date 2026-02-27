#include "index.hpp"

std::shared_ptr<Plugin> solanaPlugin = object{
    object::pair{std:("name"), SOLANA_SERVICE_NAME}, 
    object::pair{std:("description"), std:("Solana Plugin for Eliza")}, 
    object::pair{std:("actions"), array<any>{ transferToken, executeSwap }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ walletProvider }}, 
    object::pair{std:("services"), array<SolanaService>{ SolanaService }}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std:("solana init"));
        auto asking = std:("solana");
        auto serviceType = std:("TRADER_CHAIN");
        auto traderChainService = as<any>(runtime->getService(serviceType));
        while (!traderChainService)
        {
            console->log(asking, std:("waiting for"), serviceType, std:("service..."));
            traderChainService = as<any>(runtime->getService(serviceType));
            if (!traderChainService) {
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
            object::pair{std:("name"), std:("Solana services")}
        };
        traderChainService["registerChain"](me);
        console->log(std:("jupiter init done"));
    }
    }
};

void Main(void)
{
}

MAIN
