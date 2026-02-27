#include "types.hpp"
#include <string>

namespace @elizaos/core {
}
object PluginManagerServiceType = object{
    object::pair{std::string("PLUGIN_MANAGER"), as<std::shared_ptr<const>>(std::string("PLUGIN_MANAGER"))}, 
    object::pair{std::string("PLUGIN_CONFIGURATION"), as<std::shared_ptr<const>>(std::string("PLUGIN_CONFIGURATION"))}, 
    object::pair{std::string("PLUGIN_USER_INTERACTION"), as<std::shared_ptr<const>>(std::string("PLUGIN_USER_INTERACTION"))}
}, satisfies, Partial;
object EventType = as<std::shared_ptr<const>>(object{
    object::pair{std::string("PLUGIN_BUILDING"), std::string("PLUGIN_BUILDING")}, 
    object::pair{std::string("PLUGIN_READY"), std::string("PLUGIN_READY")}, 
    object::pair{std::string("PLUGIN_LOADED"), std::string("PLUGIN_LOADED")}, 
    object::pair{std::string("PLUGIN_UNLOADED"), std::string("PLUGIN_UNLOADED")}, 
    object::pair{std::string("PLUGIN_ERROR"), std::string("PLUGIN_ERROR")}, 
    object::pair{std::string("PLUGIN_ENV_MISSING"), std::string("PLUGIN_ENV_MISSING")}, 
    object::pair{std::string("PLUGIN_CONFIGURATION_REQUIRED"), std::string("PLUGIN_CONFIGURATION_REQUIRED")}, 
    object::pair{std::string("PLUGIN_CONFIGURATION_STARTED"), std::string("PLUGIN_CONFIGURATION_STARTED")}, 
    object::pair{std::string("PLUGIN_CONFIGURATION_COMPLETED"), std::string("PLUGIN_CONFIGURATION_COMPLETED")}
});

void Main(void)
{
    static_cast<any>();
}

MAIN
