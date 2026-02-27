#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-clanker/src/plugin.h"

std::shared_ptr<Plugin> clankerPlugin = object{
    object::pair{std:("name"), std:("plugin-clanker")}, 
    object::pair{std:("description"), std:("Clanker protocol integration for token deployment and trading on Base L2")}, 
    , 
    object::pair{std:("services"), array<ClankerService>{ ClankerService }}, 
    object::pair{std:("actions"), array<any>{ tokenDeployAction }}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("evaluators"), array<any>()}
};

void Main(void)
{
}

MAIN
