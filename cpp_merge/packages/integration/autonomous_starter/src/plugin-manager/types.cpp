#include "types.hpp"

namespace @elizaos/core {
}
object PluginManagerServiceType = object{
    object::pair{std:("PLUGIN_MANAGER"), as<std::shared_ptr<const>>(std:("PLUGIN_MANAGER"))}, 
    object::pair{std:("PLUGIN_CONFIGURATION"), as<std::shared_ptr<const>>(std:("PLUGIN_CONFIGURATION"))}, 
    object::pair{std:("PLUGIN_USER_INTERACTION"), as<std::shared_ptr<const>>(std:("PLUGIN_USER_INTERACTION"))}
}, satisfies, Partial;
object EventType = as<std::shared_ptr<const>>(object{
    object::pair{std:("PLUGIN_BUILDING"), std:("PLUGIN_BUILDING")}, 
    object::pair{std:("PLUGIN_READY"), std:("PLUGIN_READY")}, 
    object::pair{std:("PLUGIN_LOADED"), std:("PLUGIN_LOADED")}, 
    object::pair{std:("PLUGIN_UNLOADED"), std:("PLUGIN_UNLOADED")}, 
    object::pair{std:("PLUGIN_ERROR"), std:("PLUGIN_ERROR")}, 
    object::pair{std:("PLUGIN_ENV_MISSING"), std:("PLUGIN_ENV_MISSING")}, 
    object::pair{std:("PLUGIN_CONFIGURATION_REQUIRED"), std:("PLUGIN_CONFIGURATION_REQUIRED")}, 
    object::pair{std:("PLUGIN_CONFIGURATION_STARTED"), std:("PLUGIN_CONFIGURATION_STARTED")}, 
    object::pair{std:("PLUGIN_CONFIGURATION_COMPLETED"), std:("PLUGIN_CONFIGURATION_COMPLETED")}
});

void Main(void)
{
    static_cast<any>();
}

MAIN
