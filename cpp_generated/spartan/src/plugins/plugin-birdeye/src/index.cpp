#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/index.h"

std::shared_ptr<Plugin> birdeyePlugin = object{
    object::pair{std::string("name"), std::string("birdeye")}, 
    object::pair{std::string("description"), std::string("birdeye plugin")}, 
    object::pair{std::string("routes"), array<any>()}, 
    object::pair{std::string("actions"), array<any>{ tokenSearchAddressAction, tokenSearchSymbolAction, walletSearchAddressAction }}, 
    object::pair{std::string("providers"), array<any>{ agentPortfolioProvider, trendingProvider, tradePortfolioProvider }}, 
    object::pair{std::string("services"), array<BirdeyeService>{ BirdeyeService }}, 
    object::pair{std::string("tests"), array<any>()}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        auto hasPluginTrader = true;
        if (hasPluginTrader) {
            std::make_shared<Promise<void>>([=](auto resolve) mutable
            {
                resolve();
                console->log(std::string("birdeyeStartIn"));
                auto service = as<any>(runtime->getService(std::string("TRADER_DATAPROVIDER")));
                while (!service)
                {
                    console->log(std::string("birdeye waiting for Trading info service..."));
                    service = as<any>(runtime->getService(std::string("TRADER_DATAPROVIDER")));
                    if (!service) {
                        std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                        {
                            return setTimeout(waitResolve, 1000);
                        }
                        ); });
                    } else {
                        console->log(std::string("birdeye Acquired trading chain service..."));
                    }
                }
                auto me = object{
                    object::pair{std::string("name"), std::string("Birdeye")}, 
                    object::pair{std::string("trendingService"), BIRDEYE_SERVICE_NAME}
                };
                std::async([=]() { service["registerDataProvder"](me); });
                console->log(std::string("birdeyeStart done"));
            }
            );
        }
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_birdeye") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        if (hasPluginTrader) {
            runtime->registerTaskWorker(object{
                object::pair{std::string("name"), std::string("BIRDEYE_SYNC_TRENDING")}, 
                object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto birdeye = std::make_shared<Birdeye>(runtime);
                    try
                    {
                        std::async([=]() { birdeye->syncTrendingTokens(std::string("solana")); });
                    }
                    catch (const any& error)
                    {
                        logger->error(std::string("Failed to sync trending tokens"), error);
                        runtime["deleteTask"](task["id"]);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std::string("name"), std::string("BIRDEYE_SYNC_TRENDING")}, 
                object::pair{std::string("description"), std::string("Sync trending tokens from Birdeye")}, 
                object::pair{std::string("worldId"), std::string("worldId")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("updatedAt"), Date->now()}, 
                    object::pair{std::string("updateInterval"), 1000 * 60 * 60}
                }}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_birdeye"), std::string("immediate") }}
            });
        }
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("BIRDEYE_SYNC_WALLET")}, 
            object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
            {
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto birdeye = std::make_shared<Birdeye>(runtime);
                try
                {
                    std::async([=]() { birdeye->syncWallet(); });
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to sync wallet"), error);
                    std::async([=]() { runtime["deleteTask"](task["id"]); });
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std::string("name"), std::string("BIRDEYE_SYNC_WALLET")}, 
            object::pair{std::string("description"), std::string("Sync wallet from Birdeye")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_birdeye"), std::string("immediate") }}
        });
    }
    }
};

void Main(void)
{
}

MAIN
