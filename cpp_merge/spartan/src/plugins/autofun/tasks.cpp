#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/tasks.h"

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto worldId = undefined) mutable
{
    worldId = runtime->agentId;
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime->deleteTask(task->id); });
    }
    runtime->registerTaskWorker(object{
        object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_WALLET")}, 
        object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
        }
        }
    });
    runtime->createTask(object{
        object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_WALLET")}, 
        object::pair{std:("description"), std:("Sync wallet from Birdeye")}, 
        object::pair{std:("worldId"), std:("worldId")}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun"), std:("immediate") }}
    });
    runtime->registerTaskWorker(object{
        object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_RAW_AUTOFUN_CHAT")}, 
        object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            auto chat = std::make_shared<Chat>(runtime);
            try
            {
                std::async([=]() { chat->syncChats(); });
            }
            catch (const any& error)
            {
                logger->debug(std:("Failed to sync tokens"), error);
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_RAW_AUTOFUN_CHAT")}, 
        object::pair{std:("description"), std:("Check autofun chat rooms")}, 
        object::pair{std:("worldId"), std:("worldId")}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun"), std:("immediate") }}
    });
    auto twitterService = runtime->getService(std:("twitter"));
    if (twitterService) {
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
            {
                auto twitterService = runtime["getService"](std:("twitter"));
                if (!twitterService) {
                    logger->debug(std:("Twitter service not available, removing AUTOFUN_INTEL_SYNC_RAW_TWEETS task"));
                    auto tasks = std::async([=]() { runtime["getTasksByName"](std:("AUTOFUN_INTEL_SYNC_RAW_TWEETS")); });
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
            object::pair{std:("name"), std:("AUTOFUN_INTEL_SYNC_RAW_TWEETS")}, 
            object::pair{std:("description"), std:("Sync raw tweets from Twitter")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 15}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun"), std:("immediate") }}
        });
        runtime->registerTaskWorker(object{
            object::pair{std:("name"), std:("AUTOFUN_INTEL_INTEL_PARSE_TWEETS")}, 
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
            object::pair{std:("name"), std:("AUTOFUN_INTEL_INTEL_PARSE_TWEETS")}, 
            object::pair{std:("description"), std:("Parse tweets")}, 
            object::pair{std:("worldId"), std:("worldId")}, 
            object::pair{std:("metadata"), object{
                object::pair{std:("createdAt"), Date->now()}, 
                object::pair{std:("updatedAt"), Date->now()}, 
                object::pair{std:("updateInterval"), 1000 * 60 * 60 * 24}
            }}, 
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun"), std:("immediate") }}
        });
    } else {
        logger->debug(std:("WARNING: Twitter service not found, skipping creation of INTEL_SYNC_RAW_TWEETS task"));
    }
};

void Main(void)
{
}

MAIN
