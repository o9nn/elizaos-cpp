#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> heliusPlugin = object{
    object::pair{std::string("name"), std::string("helius")}, 
    object::pair{std::string("description"), std::string("Helius data plugin")}, 
    object::pair{std::string("actions"), array<any>()}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ heliusProvider }}
};

void Main(void)
{
}

MAIN
