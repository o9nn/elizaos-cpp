#include "plugin-utils.h"
#include <string>

any isValidPluginShape(any obj)
{
    if (OR((OR((!obj), (type_of(obj) != std::string("object")))), (!obj["name"]))) {
        return false;
    }
    return !!(OR((OR((OR((OR((OR((obj["init"]), (obj["services"]))), (obj["providers"]))), (obj["actions"]))), (obj["evaluators"]))), (obj["description"])));
};


std::shared_ptr<Promise<any>> loadAndPreparePlugin(string pluginName)
{
    auto version = getCliInstallTag();
    any pluginModule;
    auto context = detectPluginContext(pluginName);
    if (context->isLocalDevelopment) {
        try
        {
            pluginModule = std::async([=]() { loadPluginModule(pluginName); });
            if (!pluginModule) {
                logger->error(std::string("Failed to load local plugin ") + pluginName + std::string("."));
                provideLocalPluginGuidance(pluginName, context);
                return nullptr;
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Error loading local plugin ") + pluginName + std::string(": ") + error + string_empty);
            provideLocalPluginGuidance(pluginName, context);
            return nullptr;
        }
    } else {
        try
        {
            pluginModule = std::async([=]() { loadPluginModule(pluginName); });
            if (!pluginModule) {
                logger->info(std::string("Plugin ") + pluginName + std::string(" not available, installing..."));
                std::async([=]() { installPlugin(pluginName, process->cwd(), version); });
                pluginModule = std::async([=]() { loadPluginModule(pluginName); });
            }
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to process plugin ") + pluginName + std::string(": ") + error + string_empty);
            return nullptr;
        }
    }
    if (!pluginModule) {
        logger->error(std::string("Failed to load module for plugin ") + pluginName + std::string("."));
        return nullptr;
    }
    auto expectedFunctionName = string_empty + pluginName->replace((new RegExp(std::string("^@elizaos\/plugin"))), string_empty)->replace((new RegExp(std::string("^@elizaos\"))), string_empty)->replace((new RegExp(std::string("-."))), [=](auto match) mutable
    {
        return const_(match)[1]->toUpperCase();
    }
    ) + std::string("Plugin");
    auto exportsToCheck = array<any>{ const_(pluginModule)[expectedFunctionName], pluginModule["default"], Object->values(pluginModule) };
    for (auto& potentialPlugin : exportsToCheck)
    {
        if (isValidPluginShape(potentialPlugin)) {
            return as<std::shared_ptr<Plugin>>(potentialPlugin);
        }
    }
    logger->warn(std::string("Could not find a valid plugin in ") + pluginName + std::string("."));
    return nullptr;
};


std::shared_ptr<PluginValidation> validatePlugin(any plugin)
{
    if (!plugin) {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Plugin is null or std::nullopt")}
        };
    }
    if (!isValidPluginShape(plugin)) {
        return object{
            object::pair{std::string("isValid"), false}, 
            object::pair{std::string("error"), std::string("Plugin does not have valid shape")}
        };
    }
    return object{
        object::pair{std::string("isValid"), true}, 
        object::pair{std::string("plugin"), std::string("plugin")}
    };
};


