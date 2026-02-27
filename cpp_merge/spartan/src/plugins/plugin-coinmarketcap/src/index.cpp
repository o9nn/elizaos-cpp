#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> coinmarketcapPlugin = object{
    object::pair{std::string("name"), std::string("coinmarketcap")}, 
    object::pair{std::string("description"), std::string("CoinMarketCap Plugin for Eliza")}, 
    object::pair{std::string("actions"), array<any>{ getPrice }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ trendingProvider }}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_coinmarketcap") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("COINMARKETCAP_SYNC_TRENDING")}, 
            object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
            {
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto cmc = std::make_shared<Coinmarketcap>(runtime);
                try
                {
                    std::async([=]() { cmc->syncTokens(); });
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
            object::pair{std::string("name"), std::string("COINMARKETCAP_SYNC_TRENDING")}, 
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
    }
};

void Main(void)
{
}

MAIN
