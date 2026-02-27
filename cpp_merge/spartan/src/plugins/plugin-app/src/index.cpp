#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-app/src/index.h"

std::shared_ptr<Plugin> appPlugin = object{
    object::pair{std:("name"), std:("AppDev")}, 
    object::pair{std:("description"), std:("application development framework for ElizaOS")}, 
    object::pair{std:("actions"), array<any>{ userRegistration, checkRegistrationCode, checkRegistration, deleteRegistration, servicesMenu, walletCreate, setStrategy, userMetawalletList, devFix }}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>()}
};

void Main(void)
{
}

MAIN
