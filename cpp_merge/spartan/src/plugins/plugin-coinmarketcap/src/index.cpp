#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/index.h"

std::shared_ptr<Plugin> coinmarketcapPlugin = object{
    object::pair{std:("name"), std:("coinmarketcap")}, 
    object::pair{std:("description"), std:("CoinMarketCap Plugin for Eliza")}, 
    object::pair{std:("actions"), array<any>{ getPrice }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ trendingProvider }}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_coinmarketcap") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("COINMARKETCAP_SYNC_TRENDING")}, 
            object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
            {
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto cmc = std::make_shared<Coinmarketcap>(runtime);
                try
                {
                    std::async([=]() { cmc->syncTokens(); });
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
            object::pair{std:("name"), std:("COINMARKETCAP_SYNC_TRENDING")}, 
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
    }
};

void Main(void)
{
}

MAIN
