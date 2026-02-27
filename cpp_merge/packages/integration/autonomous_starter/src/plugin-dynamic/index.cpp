#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> pluginDynamic = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-dynamic")}, 
    object::pair{std::string("description"), std::string("Dynamic plugin creation system with AI-powered code generation")}, 
    object::pair{std::string("actions"), array<any>{ createPluginAction, checkPluginCreationStatusAction, cancelPluginCreationAction, createPluginFromDescriptionAction }}, 
    object::pair{std::string("providers"), array<any>{ pluginCreationStatusProvider, pluginCreationCapabilitiesProvider }}, 
    object::pair{std::string("services"), array<PluginCreationService>{ PluginCreationService }}, 
    object::pair{std::string("evaluators"), array<any>()}
};

void Main(void)
{
}

MAIN
