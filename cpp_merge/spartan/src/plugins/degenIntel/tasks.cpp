#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks.h"

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto worldId = undefined) mutable
{
    worldId = runtime->agentId;
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime->deleteTask(task->id); });
    }
    runtime->registerTaskWorker(object{
        object::pair{std:("name"), std:("INTEL_SYNC_WALLET")}, 
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
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std:("name"), std:("INTEL_SYNC_WALLET")}, 
        object::pair{std:("description"), std:("Sync wallet from Birdeye")}, 
        object::pair{std:("worldId"), std:("worldId")}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel"), std:("immediate") }}
    });
    auto plugins = runtime->plugins->map([=](auto p) mutable
    {
        return p["name"];
    }
    );
    if (plugins->indexOf(std:("twitter")) != -1) {
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std:("twitter"));
                if (!twitterService) {
                    logger->debug(std:("Twitter service not available, removing INTEL_SYNC_RAW_TWEETS task"));
                    auto tasks = std::async([=]() { runtime["getTasksByName"](std:("INTEL_SYNC_RAW_TWEETS")); });
                    for (auto& task : tasks)
                    {
                        std::async([=]() { runtime["deleteTask"](task["id"]); });
                    }
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                try
                {
                    auto twitter = std::make_shared<Twitter>(runtime);
                    std::async([=]() { twitter->syncRawTweets(); });
                }
                catch (const any& error)
                {
                    logger->error(std:("Failed to sync raw tweets"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std:("name"), std:("INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std:("description"), std:("Sync raw tweets from Twitter")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 15}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel"), std:("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("INTEL_PARSE_TWEETS")}, 
            object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std:("twitter"));
                if (!twitterService) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto twitterParser = std::make_shared<TwitterParser>(runtime);
                try
                {
                    std::async([=]() { twitterParser->parseTweets(); });
                }
                catch (const any& error)
                {
                    logger->error(std:("Failed to parse tweets"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std:("name"), std:("INTEL_PARSE_TWEETS")}, 
            object::pair{std:("description"), std:("Parse tweets")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 60 * 24}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel"), std:("immediate") }}
        });
    } else {
        console->log(std:("intel:tasks - plugins"), runtime->plugins->map([=](auto p) mutable
        {
            return p["name"];
        }
        ));
        logger->debug(std:("WARNING: Twitter plugin not found, skipping creation of INTEL_SYNC_RAW_TWEETS task"));
    }
    auto tradeService = as<any>(runtime->getService(std:("degen_trader")));
    if (tradeService) {
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("INTEL_GENERATE_BUY_SIGNAL")}, 
            object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto sentimentsData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<Sentiment>>>(std:("sentiments")); }))), (array<any>()));
                if (sentimentsData["length"] == 0) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto signal = std::make_shared<BuySignal>(runtime);
                try
                {
                    std::async([=]() { signal->generateSignal(); });
                }
                catch (const any& error)
                {
                    logger->error(std:("Failed to generate buy signal"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std:("name"), std:("INTEL_GENERATE_BUY_SIGNAL")}, 
            object::pair{std:("description"), std:("Generate a buy signal")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel"), std:("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("INTEL_GENERATE_SELL_SIGNAL")}, 
            object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto sentimentsData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<Sentiment>>>(std:("sentiments")); }))), (array<any>()));
                if (sentimentsData["length"] == 0) {
                    return false;
                }
                return true;
            }
            }, 
            object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
            {
                auto signal = std::make_shared<SellSignal>(runtime);
                try
                {
                    std::async([=]() { signal->generateSignal(); });
                }
                catch (const any& error)
                {
                    logger->error(std:("Failed to generate buy signal"), error);
                }
            }
            }
        });
        runtime->createTask(object{
            object::pair{std:("name"), std:("INTEL_GENERATE_SELL_SIGNAL")}, 
            object::pair{std:("description"), std:("Generate a sell signal")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 5}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("degen_intel"), std:("immediate") }}
        });
    } else {
        logger->debug(std:("WARNING: Trader service not found, skipping creation of INTEL_GENERATE_*_SIGNAL task"));
    }
};

void Main(void)
{
}

MAIN
