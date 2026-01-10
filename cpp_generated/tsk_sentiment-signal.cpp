#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks/tsk_sentiment-signal.h"

void setupSentimentGenerator(any runtime)
{
    worldId = runtime["agentId"];
    auto tasks = std::async([=]() { runtime["getTasks"](object{
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_trader") }}
    }); });
    for (auto& task : tasks)
    {
        std::async([=]() { runtime["deleteTask"](task["id"]); });
    }
    runtime["registerTaskWorker"](object{
        object::pair{std::string("name"), std::string("TRADER_SYNC_SENTIMENT")}, 
        object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            try
            {
                console->log(std::string("PLUGIN_TRADER_SENTIMENT"));
                runtime["emitEvent"](std::string("PLUGIN_TRADER_SENTIMENT"), object{});
            }
            catch (const any& error)
            {
                logger->error(std::string("Failed to sync sentiment"), error);
            }
        }
        }
    });
    runtime["createTask"](object{
        object::pair{std::string("name"), std::string("TRADER_SYNC_SENTIMENT")}, 
        object::pair{std::string("description"), std::string("calculate sentiment")}, 
        object::pair{std::string("worldId"), std::string("worldId")}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("createdAt"), Date->now()}, 
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("updateInterval"), 1000 * 60 * 5}
        }}, 
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("plugin_trader"), std::string("immediate") }}
    });
};


