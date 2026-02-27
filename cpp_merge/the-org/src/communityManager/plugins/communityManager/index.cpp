#include "index.hpp"

std::shared_ptr<Plugin> communityManagerPlugin = object{
    object::pair{std:("name"), std:("community-manager")}, 
    object::pair{std:("description"), std:("Community Manager Plugin for Eliza")}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("providers"), array<any>{ timeoutUserProvider }}, 
    object::pair{std:("actions"), array<any>{ timeoutUser }}, 
    object::pair{std:("services"), array<CommunityManagerService>{ CommunityManagerService }}
};

void Main(void)
{
}

MAIN
