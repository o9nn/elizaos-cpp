#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/index.h"

std::shared_ptr<Plugin> autonomousTraderPlugin = object{
    object::pair{std:("name"), std:("autonomous-trader")}, 
    object::pair{std:("description"), std:("Spartan Autonomous trading agent plugin")}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("actions"), array<any>{ userRegistration, checkRegistrationCode, checkRegistration, deleteRegistration, servicesMenu, walletCreate, setStrategy, userMetawalletList, devFix }}, 
    object::pair{std:("services"), array<any>()}, 
    object::pair{std:("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std:("autonomous-trader init"));
        llmStrategy(runtime);
        copyStrategy(runtime);
    }
    }
};

void Main(void)
{
}

MAIN
