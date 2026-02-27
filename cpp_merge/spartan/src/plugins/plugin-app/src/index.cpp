#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/index.h"

std::shared_ptr<Plugin> appPlugin = object{
    object::pair{std::string("name"), std::string("AppDev")}, 
    object::pair{std::string("description"), std::string("application development framework for ElizaOS")}, 
    object::pair{std::string("actions"), array<any>{ userRegistration, checkRegistrationCode, checkRegistration, deleteRegistration, servicesMenu, walletCreate, setStrategy, userMetawalletList, devFix }}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
