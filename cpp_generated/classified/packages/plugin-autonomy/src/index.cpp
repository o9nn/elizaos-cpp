#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autonomy/src/index.h"

std::shared_ptr<Plugin> autonomyPlugin = object{
    object::pair{std::string("name"), std::string("autonomy")}, 
    object::pair{std::string("description"), std::string("Clean autonomous loop plugin with settings-based control")}, 
    object::pair{std::string("services"), array<AutonomyService>{ AutonomyService }}, 
    object::pair{std::string("providers"), array<any>{ adminChatProvider, autonomyStatusProvider }}, 
    object::pair{std::string("actions"), array<any>{ sendToAdminAction }}, 
    object::pair{std::string("routes"), autonomyRoutes}
};

void Main(void)
{
}

MAIN
