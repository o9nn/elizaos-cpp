#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> autofunPlugin = object{
    object::pair{std::string("name"), std::string("autofun")}, 
    object::pair{std::string("description"), std::string("Autofun plugin")}, 
    object::pair{std::string("routes"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ autofunProvider }}, 
    object::pair{std::string("tests"), array<object>{ object{
        object::pair{std::string("name"), std::string("test suite for degen-intel")}, 
        object::pair{std::string("tests"), array<object>{ object{
            object::pair{std::string("name"), std::string("test for degen-intel")}, 
            object::pair{std::string("fn"), [=](auto runtime) mutable
            {
                logger->info(std::string("test in degen-intel working"));
            }
            }
        } }}
    } }}, 
    object::pair{std::string("services"), array<any>()}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        auto hasPluginTrader = true;
        if (hasPluginTrader) {
            std::make_shared<Promise<void>>([=](auto resolve) mutable
            {
                resolve();
                console->log(std::string("autofunStartIn"));
                auto service = as<any>(runtime->getService(std::string("TRADER_DATAPROVIDER")));
                while (!service)
                {
                    console->log(std::string("autofun waiting for Trading info service..."));
                    service = as<any>(runtime->getService(std::string("TRADER_DATAPROVIDER")));
                    if (!service) {
                        std::async([=]() { std::make_shared<Promise>([=](auto waitResolve) mutable
                        {
                            return setTimeout(waitResolve, 1000);
                        }
                        ); });
                    } else {
                        console->log(std::string("autofun Acquired trading chain service..."));
                    }
                }
                auto me = object{
                    object::pair{std::string("name"), std::string("Autofun")}, 
                    object::pair{std::string("trendingService"), std::string("AUTOFUN")}
                };
                std::async([=]() { service["registerDataProvder"](me); });
                console->log(std::string("autofunStart done"));
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
