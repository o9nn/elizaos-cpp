#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-morpho/src/plugin.h"

std::shared_ptr<Plugin> morphoPlugin = object{
    object::pair{std::string("name"), std::string("plugin-morpho")}, 
    object::pair{std::string("description"), std::string("Morpho protocol integration for optimized lending and borrowing through peer-to-peer matching")}, 
    , 
    object::pair{std::string("services"), array<MorphoService>{ MorphoService }}, 
    object::pair{std::string("actions"), array<any>{ marketInfoAction, marketPositionsAction, marketTransferAction, vaultPositionsAction, vaultInfoAction, vaultTransferAction }}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("evaluators"), array<any>()}
};

void Main(void)
{
}

MAIN
