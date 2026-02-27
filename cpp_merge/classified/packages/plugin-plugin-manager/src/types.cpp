#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/types.h"

namespace @elizaos/core {
}
object PluginManagerServiceType = as<std::shared_ptr<const>>(object{
    object::pair{std:("PLUGIN_MANAGER"), as<std::shared_ptr<ServiceTypeName>>(std:("plugin_manager"))}, 
    object::pair{std:("PLUGIN_CONFIGURATION"), as<std::shared_ptr<ServiceTypeName>>(std:("plugin_configuration"))}, 
    object::pair{std:("REGISTRY"), as<std::shared_ptr<ServiceTypeName>>(std:("registry"))}
});

void Main(void)
{
}

MAIN
