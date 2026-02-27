#include "index.hpp"

std::shared_ptr<Plugin> autofunTraderPlugin = object{
    object::pair{std:("name"), std:("Autofun Trader Plugin")}, 
    object::pair{std:("description"), std:("Autonomous trading agent plugin for automated trading strategies")}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("actions"), array<any>()}, 
    object::pair{std:("services"), array<DegenTradingService>{ DegenTradingService }}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun_trader") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        auto allowBuy = true;
        auto allowSell = true;
        if (allowBuy) {
            runtime->registerTaskWorker(object{
                object::pair{std:("name"), std:("AFTRADER_GOTO_MARKET")}, 
                object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto tradeService = runtime["getService"](ServiceTypes::AUTOFUN_TRADING);
                    try
                    {
                        tradeService["buyService"]["generateSignal"]();
                    }
                    catch (const any& error)
                    {
                        logger->error(std:("Failed to generate buy signal"), error);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std:("name"), std:("AFTRADER_GOTO_MARKET")}, 
                object::pair{std:("description"), std:("Generate a buy signal")}, 
                object::pair{std:("worldId"), std:("worldId")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("updatedAt"), Date->now()}, 
                    object::pair{std:("updateInterval"), 1000 * 60 * 5}
                }}, 
                object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun_trader"), std:("immediate") }}
            });
        }
        if (allowSell) {
            runtime->registerTaskWorker(object{
                object::pair{std:("name"), std:("AFTRADER_CHECK_POSITIONS")}, 
                object::pair{std:("validate"), [=](auto runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto tradeService = as<any>(runtime["getService"](ServiceTypes::AUTOFUN_TRADING));
                    try
                    {
                        tradeService->sellService->generateSignal();
                    }
                    catch (const any& error)
                    {
                        logger->error(std:("Failed to generate buy signal"), error);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std:("name"), std:("AFTRADER_CHECK_POSITIONS")}, 
                object::pair{std:("description"), std:("Generate a sell signal")}, 
                object::pair{std:("worldId"), std:("worldId")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("updatedAt"), Date->now()}, 
                    object::pair{std:("updateInterval"), 1000 * 60 * 5}
                }}, 
                object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("autofun_trader"), std:("immediate") }}
            });
        }
    }
    }
};

void Main(void)
{
}

MAIN
