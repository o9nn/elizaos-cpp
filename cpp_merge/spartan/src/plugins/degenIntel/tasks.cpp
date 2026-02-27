#include "tasks.hpp"
#include <string>

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto worldId = std::nullopt) mutable
{
    worldId = runtime->agentId;
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime->deleteTask(task->id); });
    }
    runtime->registerTaskWorker(object{
        object::pair{std::string("name"), std::string("INTEL_SYNC_WALLET")}, 
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
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std::string("name"), std::string("INTEL_SYNC_WALLET")}, 
        object::pair{std::string("description"), std::string("Sync wallet from Birdeye")}, 
        object::pair{std::string("worldId"), std::string("worldId")}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel"), std::string("immediate") }}
    });
    auto plugins = runtime->plugins->map([=](auto p) mutable
    {
        return p["name"];
    }
    );
    if (plugins->indexOf(std::string("twitter")) != -1) {
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std::string("twitter"));
                if (!twitterService) {
                    logger->debug(std::string("Twitter service not available, removing INTEL_SYNC_RAW_TWEETS task"));
                    auto tasks = std::async([=]() { runtime["getTasksByName"](std::string("INTEL_SYNC_RAW_TWEETS")); });
                    for (auto& task : tasks)
                    {
                        std::async([=]() { runtime["deleteTask"](task["id"]); });
                    }
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                try
                {
                    auto twitter = std::make_shared<Twitter>(runtime);
                    std::async([=]() { twitter->syncRawTweets(); });
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to sync raw tweets"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std::string("name"), std::string("INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std::string("description"), std::string("Sync raw tweets from Twitter")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 15}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel"), std::string("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("INTEL_PARSE_TWEETS")}, 
            object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std::string("twitter"));
                if (!twitterService) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto twitterParser = std::make_shared<TwitterParser>(runtime);
                try
                {
                    std::async([=]() { twitterParser->parseTweets(); });
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to parse tweets"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std::string("name"), std::string("INTEL_PARSE_TWEETS")}, 
            object::pair{std::string("description"), std::string("Parse tweets")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 60 * 24}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel"), std::string("immediate") }}
        });
    } else {
        console->log(std::string("intel:tasks - plugins"), runtime->plugins->map([=](auto p) mutable
        {
            return p["name"];
        }
        ));
        logger->debug(std::string("WARNING: Twitter plugin not found, skipping creation of INTEL_SYNC_RAW_TWEETS task"));
    }
    auto tradeService = as<any>(runtime->getService(std::string("degen_trader")));
    if (tradeService) {
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("INTEL_GENERATE_BUY_SIGNAL")}, 
            object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto sentimentsData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
                if (sentimentsData["length"] == 0) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto signal = std::make_shared<BuySignal>(runtime);
                try
                {
                    std::async([=]() { signal->generateSignal(); });
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to generate buy signal"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std::string("name"), std::string("INTEL_GENERATE_BUY_SIGNAL")}, 
            object::pair{std::string("description"), std::string("Generate a buy signal")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel"), std::string("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("INTEL_GENERATE_SELL_SIGNAL")}, 
            object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto sentimentsData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
                if (sentimentsData["length"] == 0) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto signal = std::make_shared<SellSignal>(runtime);
                try
                {
                    std::async([=]() { signal->generateSignal(); });
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to generate buy signal"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std::string("name"), std::string("INTEL_GENERATE_SELL_SIGNAL")}, 
            object::pair{std::string("description"), std::string("Generate a sell signal")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("degen_intel"), std::string("immediate") }}
        });
    } else {
        logger->debug(std::string("WARNING: Trader service not found, skipping creation of INTEL_GENERATE_*_SIGNAL task"));
    }
};

void Main(void)
{
}

MAIN
