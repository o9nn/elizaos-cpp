#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-morpho/src/plugin.h"

std::shared_ptr<Plugin> morphoPlugin = object{
    object::pair{std:("name"), std:("plugin-morpho")}, 
    object::pair{std:("description"), std:("Morpho protocol integration for optimized lending and borrowing through peer-to-peer matching")}, 
    , 
    object::pair{std:("services"), array<MorphoService>{ MorphoService }}, 
    object::pair{std:("actions"), array<any>{ marketInfoAction, marketPositionsAction, marketTransferAction, vaultPositionsAction, vaultInfoAction, vaultTransferAction }}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("evaluators"), array<any>()}
};

void Main(void)
{
}

MAIN
