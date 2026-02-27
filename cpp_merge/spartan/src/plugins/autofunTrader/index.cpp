#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> autofunTraderPlugin = object{
    object::pair{std::string("name"), std::string("Autofun Trader Plugin")}, 
    object::pair{std::string("description"), std::string("Autonomous trading agent plugin for automated trading strategies")}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("actions"), array<any>()}, 
    object::pair{std::string("services"), array<DegenTradingService>{ DegenTradingService }}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        auto worldId = runtime->agentId;
        auto tasks = std::async([=]() { runtime->getTasks(object{
            object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun_trader") }}
        }); });
        for (auto& task : tasks)
        {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
        auto allowBuy = true;
        auto allowSell = true;
        if (allowBuy) {
            runtime->registerTaskWorker(object{
                object::pair{std::string("name"), std::string("AFTRADER_GOTO_MARKET")}, 
                object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto tradeService = runtime["getService"](ServiceTypes::AUTOFUN_TRADING);
                    try
                    {
                        tradeService["buyService"]["generateSignal"]();
                    }
                    catch (const any& error)
                    {
                        logger->error(std::string("Failed to generate buy signal"), error);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std::string("name"), std::string("AFTRADER_GOTO_MARKET")}, 
                object::pair{std::string("description"), std::string("Generate a buy signal")}, 
                object::pair{std::string("worldId"), std::string("worldId")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("updatedAt"), Date->now()}, 
                    object::pair{std::string("updateInterval"), 1000 * 60 * 5}
                }}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun_trader"), std::string("immediate") }}
            });
        }
        if (allowSell) {
            runtime->registerTaskWorker(object{
                object::pair{std::string("name"), std::string("AFTRADER_CHECK_POSITIONS")}, 
                object::pair{std::string("validate"), [=](auto runtime, auto _message, auto _state) mutable
                {
                    return true;
                }
                }, 
                object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
                {
                    auto tradeService = as<any>(runtime["getService"](ServiceTypes::AUTOFUN_TRADING));
                    try
                    {
                        tradeService->sellService->generateSignal();
                    }
                    catch (const any& error)
                    {
                        logger->error(std::string("Failed to generate buy signal"), error);
                    }
                }
                }
            });
            runtime->createTask(object{
                object::pair{std::string("name"), std::string("AFTRADER_CHECK_POSITIONS")}, 
                object::pair{std::string("description"), std::string("Generate a sell signal")}, 
                object::pair{std::string("worldId"), std::string("worldId")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("updatedAt"), Date->now()}, 
                    object::pair{std::string("updateInterval"), 1000 * 60 * 5}
                }}, 
                object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("autofun_trader"), std::string("immediate") }}
            });
        }
    }
    }
};

void Main(void)
{
}

MAIN
