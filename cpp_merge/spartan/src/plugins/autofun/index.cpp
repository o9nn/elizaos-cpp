#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/index.h"

std::shared_ptr<Plugin> autofunPlugin = object{
    object::pair{std:("name"), std:("autofun")}, 
    object::pair{std:("description"), std:("Autofun plugin")}, 
    object::pair{std:("routes"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ autofunProvider }}, 
    object::pair{std:("tests"), array<object>{ object{
        object::pair{std:("name"), std:("test suite for degen-intel")}, 
        object::pair{std:("tests"), array<object>{ object{
            object::pair{std:("name"), std:("test for degen-intel")}, 
            object::pair{std:("fn"), [=](auto runtime) mutable
            {
                logger->info(std:("test in degen-intel working"));
            }
            }
        } }}
    } }}, 
    object::pair{std:("services"), array<any>()}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        auto hasPluginTrader = true;
        if (hasPluginTrader) {
            std::make_shared<Promise<void>>([=](auto resolve) mutable
            {
                resolve();
                console->log(std:("autofunStartIn"));
                auto service = as<any>(runtime->getService(std:("TRADER_DATAPROVIDER")));
                while (!service)
                {
                    console->log(std:("autofun waiting for Trading info service..."));
                    service = as<any>(runtime->getService(std:("TRADER_DATAPROVIDER")));
                    if (!service) {
                        std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                        {
                            return setTimeout(waitResolve, 1000);
                        }
                        ); });
                    } else {
                        console->log(std:("autofun Acquired trading chain service..."));
                    }
                }
                auto me = object{
                    object::pair{std:("name"), std:("Autofun")}, 
                    object::pair{std:("trendingService"), std:("AUTOFUN")}
                };
                std::async([=]() { service["registerDataProvder"](me); });
                console->log(std:("autofunStart done"));
            }
            );
        }
    }
    }
};

void Main(void)
{
}

MAIN
