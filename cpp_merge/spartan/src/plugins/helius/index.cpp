#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/helius/index.h"

std::shared_ptr<Plugin> heliusPlugin = object{
    object::pair{std:("name"), std:("helius")}, 
    object::pair{std:("description"), std:("Helius data plugin")}, 
    object::pair{std:("actions"), array<any>()}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ heliusProvider }}
};

void Main(void)
{
}

MAIN
