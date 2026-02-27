#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> communityManagerPlugin = object{
    object::pair{std::string("name"), std::string("community-manager")}, 
    object::pair{std::string("description"), std::string("Community Manager Plugin for Eliza")}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("providers"), array<any>{ timeoutUserProvider }}, 
    object::pair{std::string("actions"), array<any>{ timeoutUser }}, 
    object::pair{std::string("services"), array<CommunityManagerService>{ CommunityManagerService }}
};

void Main(void)
{
}

MAIN
