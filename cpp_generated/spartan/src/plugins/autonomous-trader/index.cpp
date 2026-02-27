#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/index.h"

std::shared_ptr<Plugin> autonomousTraderPlugin = object{
    object::pair{std::string("name"), std::string("autonomous-trader")}, 
    object::pair{std::string("description"), std::string("Spartan Autonomous trading agent plugin")}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("actions"), array<any>{ userRegistration, checkRegistrationCode, checkRegistration, deleteRegistration, servicesMenu, walletCreate, setStrategy, userMetawalletList, devFix }}, 
    object::pair{std::string("services"), array<any>()}, 
    object::pair{std::string("init"), [=](auto _, auto runtime) mutable
    {
        console->log(std::string("autonomous-trader init"));
        llmStrategy(runtime);
        copyStrategy(runtime);
    }
    }
};

void Main(void)
{
}

MAIN
