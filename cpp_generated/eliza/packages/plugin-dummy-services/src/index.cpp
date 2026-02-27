#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/index.h"

std::shared_ptr<Plugin> dummyServicesPlugin = object{
    object::pair{std::string("name"), std::string("dummy-services")}, 
    object::pair{std::string("description"), std::string("Load standard dummy services for testing purposes.")}, 
    object::pair{std::string("services"), array<DummyTokenDataService>{ DummyTokenDataService, DummyLpService, DummyWalletService }}, 
    object::pair{std::string("tests"), array<any>{ dummyServicesScenariosSuite }}, 
    object::pair{std::string("init"), [=](auto runtime) mutable
    {
        console->log(std::string("Dummy Services Plugin Initialized"));
    }
    }
};

void Main(void)
{
}

MAIN
