#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/start/utils/plugin-utils.h"

any isValidPluginShape(any obj)
{
    if (OR((OR((!obj), (type_of(obj) != std:("object")))), (!obj["name"]))) {
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
                logger->error(std:("Failed to load local plugin ") + pluginName + std:("."));
                provideLocalPluginGuidance(pluginName, context);
                return nullptr;
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Error loading local plugin ") + pluginName + std:(": ") + error + string_empty);
            provideLocalPluginGuidance(pluginName, context);
            return nullptr;
        }
    } else {
        try
        {
            pluginModule = std::async([=]() { loadPluginModule(pluginName); });
            if (!pluginModule) {
                logger->info(std:("Plugin ") + pluginName + std:(" not available, installing..."));
                std::async([=]() { installPlugin(pluginName, process->cwd(), version); });
                pluginModule = std::async([=]() { loadPluginModule(pluginName); });
            }
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to process plugin ") + pluginName + std:(": ") + error + string_empty);
            return nullptr;
        }
    }
    if (!pluginModule) {
        logger->error(std:("Failed to load module for plugin ") + pluginName + std:("."));
        return nullptr;
    }
    auto expectedFunctionName = string_empty + pluginName->replace((new RegExp(std:("^@elizaos\/plugin"))), string_empty)->replace((new RegExp(std:("^@elizaos\"))), string_empty)->replace((new RegExp(std:("-."))), [=](auto match) mutable
    {
        return const_(match)[1]->toUpperCase();
    }
    ) + std:("Plugin");
    auto exportsToCheck = array<any>{ const_(pluginModule)[expectedFunctionName], pluginModule["default"], Object->values(pluginModule) };
    for (auto& potentialPlugin : exportsToCheck)
    {
        if (isValidPluginShape(potentialPlugin)) {
            return as<std::shared_ptr<Plugin>>(potentialPlugin);
        }
    }
    logger->warn(std:("Could not find a valid plugin in ") + pluginName + std:("."));
    return nullptr;
};


std::shared_ptr<PluginValidation> validatePlugin(any plugin)
{
    if (!plugin) {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Plugin is null or undefined")}
        };
    }
    if (!isValidPluginShape(plugin)) {
        return object{
            object::pair{std:("isValid"), false}, 
            object::pair{std:("error"), std:("Plugin does not have valid shape")}
        };
    }
    return object{
        object::pair{std:("isValid"), true}, 
        object::pair{std:("plugin"), std:("plugin")}
    };
};


