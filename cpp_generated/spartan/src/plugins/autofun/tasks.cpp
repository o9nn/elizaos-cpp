#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/tasks.h"

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto worldId = undefined) mutable
{
    worldId = runtime->agentId;
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime->deleteTask(task->id); });
    }
    runtime->registerTaskWorker(object{
        object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_WALLET")}, 
        object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
        }
        }
    });
    runtime->createTask(object{
        object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_WALLET")}, 
        object::pair{std::string("description"), std::string("Sync wallet from Birdeye")}, 
        object::pair{std::string("worldId"), std::string("worldId")}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun"), std::string("immediate") }}
    });
    runtime->registerTaskWorker(object{
        object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_RAW_AUTOFUN_CHAT")}, 
        object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            auto chat = std::make_shared<Chat>(runtime);
            try
            {
                std::async([=]() { chat->syncChats(); });
            }
            catch (const any& error)
            {
                logger->debug(std::string("Failed to sync tokens"), error);
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_RAW_AUTOFUN_CHAT")}, 
        object::pair{std::string("description"), std::string("Check autofun chat rooms")}, 
        object::pair{std::string("worldId"), std::string("worldId")}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun"), std::string("immediate") }}
    });
    auto twitterService = runtime->getService(std::string("twitter"));
    if (twitterService) {
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std::string("twitter"));
                if (!twitterService) {
                    logger->debug(std::string("Twitter service not available, removing AUTOFUN_INTEL_SYNC_RAW_TWEETS task"));
                    auto tasks = std::async([=]() { runtime["getTasksByName"](std::string("AUTOFUN_INTEL_SYNC_RAW_TWEETS")); });
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
            object::pair{std::string("name"), std::string("AUTOFUN_INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std::string("description"), std::string("Sync raw tweets from Twitter")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 15}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun"), std::string("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std::string("name"), std::string("AUTOFUN_INTEL_INTEL_PARSE_TWEETS")}, 
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
            object::pair{std::string("name"), std::string("AUTOFUN_INTEL_INTEL_PARSE_TWEETS")}, 
            object::pair{std::string("description"), std::string("Parse tweets")}, 
            object::pair{std::string("worldId"), std::string("worldId")}, 
            object::pair{std::string("metadata"), object{
                object::pair{std::string("createdAt"), Date->now()}, 
                object::pair{std::string("updatedAt"), Date->now()}, 
                object::pair{std::string("updateInterval"), 1000 * 60 * 60 * 24}
            }}, 
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun"), std::string("immediate") }}
        });
    } else {
        logger->debug(std::string("WARNING: Twitter service not found, skipping creation of INTEL_SYNC_RAW_TWEETS task"));
    }
};

void Main(void)
{
}

MAIN
