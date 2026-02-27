#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> solanaPlugin = object{
    object::pair{std::string("name"), SOLANA_SERVICE_NAME}, 
    object::pair{std::string("description"), std::string("Solana Plugin for Eliza")}, 
    object::pair{std::string("actions"), array<any>{ transferToken, executeSwap }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ walletProvider }}, 
    object::pair{std::string("services"), array<SolanaService>{ SolanaService }}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std::string("solana init"));
        auto asking = std::string("solana");
        auto serviceType = std::string("TRADER_CHAIN");
        auto traderChainService = as<any>(runtime->getService(serviceType));
        while (!traderChainService)
        {
            console->log(asking, std::string("waiting for"), serviceType, std::string("service..."));
            traderChainService = as<any>(runtime->getService(serviceType));
            if (!traderChainService) {
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
            object::pair{std::string("name"), std::string("Solana services")}
        };
        traderChainService["registerChain"](me);
        console->log(std::string("jupiter init done"));
    }
    }
};

void Main(void)
{
}

MAIN
