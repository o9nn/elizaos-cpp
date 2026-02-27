#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/types.h"

namespace @elizaos/core {
}
object PluginManagerServiceType = as<std::shared_ptr<const>>(object{
    object::pair{std::string("PLUGIN_MANAGER"), as<std::shared_ptr<ServiceTypeName>>(std::string("plugin_manager"))}, 
    object::pair{std::string("PLUGIN_CONFIGURATION"), as<std::shared_ptr<ServiceTypeName>>(std::string("plugin_configuration"))}, 
    object::pair{std::string("REGISTRY"), as<std::shared_ptr<ServiceTypeName>>(std::string("registry"))}
});

void Main(void)
{
}

MAIN
