#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/index.h"

std::shared_ptr<Plugin> autonomyPlugin = object{
    object::pair{std:("name"), std:("autonomy")}, 
    object::pair{std:("description"), std:("Clean autonomous loop plugin with settings-based control")}, 
    object::pair{std:("services"), array<AutonomyService>{ AutonomyService }}, 
    object::pair{std:("providers"), array<any>{ adminChatProvider, autonomyStatusProvider }}, 
    object::pair{std:("actions"), array<any>{ sendToAdminAction }}, 
    object::pair{std:("routes"), autonomyRoutes}
};

void Main(void)
{
}

MAIN
