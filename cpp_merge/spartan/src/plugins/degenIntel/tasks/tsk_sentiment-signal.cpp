#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks/tsk_sentiment-signal.h"

void setupSentimentGenerator(any runtime)
{
    worldId = runtime["agentId"];
    auto tasks = std::async([=]() { runtime["getTasks"](object{
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_trader") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime["deleteTask"](task["id"]); });
    }
    runtime["registerTaskWorker"](object{
        object::pair{std:("name"), std:("TRADER_SYNC_SENTIMENT")}, 
        object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            try
            {
                console->log(std:("PLUGIN_TRADER_SENTIMENT"));
                runtime["emitEvent"](std:("PLUGIN_TRADER_SENTIMENT"), object{});
            }
            catch (const any& error)
            {
                logger->error(std:("Failed to sync sentiment"), error);
            }
        }
        }
    });
    runtime["createTask"](object{
        object::pair{std:("name"), std:("TRADER_SYNC_SENTIMENT")}, 
        object::pair{std:("description"), std:("calculate sentiment")}, 
        object::pair{std:("worldId"), std:("worldId")}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("createdAt"), Date->now()}, 
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("plugin_trader"), std:("immediate") }}
    });
};


