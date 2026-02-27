#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/index.h"

std::shared_ptr<Plugin> birdeyePlugin = object{
    object::pair{std:("name"), std:("birdeye")}, 
    object::pair{std:("description"), std:("birdeye plugin")}, 
    object::pair{std:("routes"), array<any>()}, 
    object::pair{std:("actions"), array<any>{ tokenSearchAddressAction, tokenSearchSymbolAction, walletSearchAddressAction }}, 
    object::pair{std:("providers"), array<any>{ agentPortfolioProvider, trendingProvider, tradePortfolioProvider }}, 
    object::pair{std:("services"), array<BirdeyeService>{ BirdeyeService }}, 
    object::pair{std:("tests"), array<any>()}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        auto hasPluginTrader = true;
        if (hasPluginTrader) {
            std::make_shared<Promise<void>>([=](auto resolve) mutable
            {
                resolve();
                console->log(std:("birdeyeStartIn"));
                auto service = as<any>(runtime->getService(std:("TRADER_DATAPROVIDER")));
                while (!service)
                {
                    console->log(std:("birdeye waiting for Trading info service..."));
                    service = as<any>(runtime->getService(std:("TRADER_DATAPROVIDER")));
                    if (!service) {
                        std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                        {
                            return setTimeout(waitResolve, 1000);
                        }
                        ); });
                    } else {
                        console->log(std:("birdeye Acquired trading chain service..."));
                    }
                }
                auto me = object{
                    object::pair{std:("name"), std:("Birdeye")}, 
                    object::pair{std:("trendingService"), BIRDEYE_SERVICE_NAME}
                };
                std::async([=]() { service["registerDataProvder"](me); });
                console->log(std:("birdeyeStart done"));
            }
            );
        }
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_birdeye") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        if (hasPluginTrader) {
            runtime->registerTaskWorker(object{
                object::pair{std:("name"), std:("BIRDEYE_SYNC_TRENDING")}, 
                object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto birdeye = std::make_shared<Birdeye>(runtime);
                    try
                    {
                        std::async([=]() { birdeye->syncTrendingTokens(std:("solana")); });
                    }
                    catch (const any& error)
                    {
                        logger->error(std:("Failed to sync trending tokens"), error);
                        runtime["deleteTask"](task["id"]);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std:("name"), std:("BIRDEYE_SYNC_TRENDING")}, 
                object::pair{std:("description"), std:("Sync trending tokens from Birdeye")}, 
                object::pair{std:("worldId"), std:("worldId")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("updatedAt"), Date->now()}, 
                    object::pair{std:("updateInterval"), 1000 * 60 * 60}
                }}, 
                object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_birdeye"), std:("immediate") }}
            });
        }
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("BIRDEYE_SYNC_WALLET")}, 
            object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
            {
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto birdeye = std::make_shared<Birdeye>(runtime);
                try
                {
                    std::async([=]() { birdeye->syncWallet(); });
                }
                catch (const any& error)
                {
                    logger->error(std:("Failed to sync wallet"), error);
                    std::async([=]() { runtime["deleteTask"](task["id"]); });
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std:("name"), std:("BIRDEYE_SYNC_WALLET")}, 
            object::pair{std:("description"), std:("Sync wallet from Birdeye")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_birdeye"), std:("immediate") }}
        });
    }
    }
};

void Main(void)
{
}

MAIN
