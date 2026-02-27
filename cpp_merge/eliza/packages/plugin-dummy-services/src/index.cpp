#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-dummy-services/src/index.h"

std::shared_ptr<Plugin> dummyServicesPlugin = object{
    object::pair{std:("name"), std:("dummy-services")}, 
    object::pair{std:("description"), std:("Load standard dummy services for testing purposes.")}, 
    object::pair{std:("services"), array<DummyTokenDataService>{ DummyTokenDataService, DummyLpService, DummyWalletService }}, 
    object::pair{std:("tests"), array<any>{ dummyServicesScenariosSuite }}, 
    object::pair{std:("init"), [=](auto runtime) mutable
    {
        console->log(std:("Dummy Services Plugin Initialized"));
    }
    }
};

void Main(void)
{
}

MAIN
