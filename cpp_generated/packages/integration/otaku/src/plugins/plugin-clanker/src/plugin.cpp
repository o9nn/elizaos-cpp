#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-clanker/src/plugin.h"

std::shared_ptr<Plugin> clankerPlugin = object{
    object::pair{std::string("name"), std::string("plugin-clanker")}, 
    object::pair{std::string("description"), std::string("Clanker protocol integration for token deployment and trading on Base L2")}, 
    , 
    object::pair{std::string("services"), array<ClankerService>{ ClankerService }}, 
    object::pair{std::string("actions"), array<any>{ tokenDeployAction }}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("evaluators"), array<any>()}
};

void Main(void)
{
}

MAIN
