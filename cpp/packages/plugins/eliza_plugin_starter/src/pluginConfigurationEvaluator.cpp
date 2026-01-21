#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/evaluators/pluginConfigurationEvaluator.h"

std::shared_ptr<Evaluator> pluginConfigurationEvaluator = object{
    object::pair{std::string("name"), std::string("pluginConfigurationEvaluator")}, 
    object::pair{std::string("description"), std::string("Evaluates plugin configuration needs and suggests configuration when plugins require environment variables")}, 
    object::pair{std::string("examples"), array<any>()}, 
    object::pair{std::string("alwaysRun"), false}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto configService = runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]);
            auto interactionService = runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]);
            if (OR((!configService), (!interactionService))) {
                return false;
            }
            shared text = message->content->text->toLowerCase();
            auto pluginKeywords = array<string>{ std::string("plugin"), std::string("configure"), std::string("setup"), std::string("install"), std::string("environment"), std::string("env var"), std::string("api key"), std::string("token"), std::string("config") };
            return pluginKeywords->some([=](auto keyword) mutable
            {
                return text->includes(keyword);
            }
            );
        }
        catch (const any& error)
        {
            logger->error(std::string("[pluginConfigurationEvaluator] Error in validation:"), error);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
            auto interactionService = as<std::shared_ptr<PluginUserInteractionService>>(runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]));
            auto pluginManagerService = as<any>(runtime->getService(PluginManagerServiceType["PLUGIN_MANAGER"]));
            if (OR((OR((!configService), (!interactionService))), (!pluginManagerService))) {
                return string_empty;
            }
            auto unconfiguredPlugins = array<object>();
            try
            {
                auto allPlugins = pluginManagerService["getAllPlugins"]();
                auto& __array2567_3620 = allPlugins;
                for (auto __indx2567_3620 = 0_N; __indx2567_3620 < __array2567_3620->get_length(); __indx2567_3620++)
                {
                    auto& plugin = const_(__array2567_3620)[__indx2567_3620];
                    {
                        try
                        {
                            auto result = std::async([=]() { configService->parsePluginRequirements(std::string("./plugins/") + plugin["name"] + string_empty); });
                            if (AND((result), (result["requiredVars"]->get_length() > 0))) {
                                shared currentConfig = std::async([=]() { configService->getPluginConfiguration(plugin["name"]); });
                                auto missingVars = result["requiredVars"]->filter([=](auto varInfo) mutable
                                {
                                    return !(*const_(currentConfig))[varInfo->name];
                                }
                                )->map([=](auto varInfo) mutable
                                {
                                    return varInfo->name;
                                }
                                );
                                if (missingVars->get_length() > 0) {
                                    unconfiguredPlugins->push(object{
                                        object::pair{std::string("name"), plugin["name"]}, 
                                        object::pair{std::string("missingVars"), std::string("missingVars")}, 
                                        object::pair{std::string("requiredVars"), result["requiredVars"]}
                                    });
                                }
                            }
                        }
                        catch (const any& error)
                        {
                            logger->debug(std::string("[pluginConfigurationEvaluator] Failed to check plugin ") + plugin["name"] + std::string(":"), error);
                        }
                    }
                }
            }
            catch (const any& error)
            {
                logger->warn(std::string("[pluginConfigurationEvaluator] Failed to check plugins:"), error);
                return string_empty;
            }
            auto activeDialogs = interactionService->getActiveDialogs();
            auto evaluation = string_empty;
            shared text = message->content->text->toLowerCase();
            auto mentionedPlugins = unconfiguredPlugins->filter([=](auto plugin) mutable
            {
                return text->includes(plugin["name"]->toLowerCase());
            }
            );
            if (mentionedPlugins->get_length() > 0) {
                evaluation += std::string("\
💡 **Configuration Suggestion**: ");
                if (mentionedPlugins->get_length() == 1) {
                    auto plugin = const_(mentionedPlugins)[0];
                    evaluation += std::string("The ") + plugin["name"] + std::string(" plugin requires configuration before it can be used. ");
                    evaluation += std::string("It needs ") + plugin["missingVars"]->get_length() + std::string(" environment variable") + (plugin["missingVars"]->get_length() > 1) ? std::string("s") : string_empty + std::string(": ");
                    evaluation += plugin["missingVars"]->map([=](auto v) mutable
                    {
                        return std::string("**") + v + std::string("**");
                    }
                    )->join(std::string(", ")) + std::string(". ");
                    evaluation += std::string("Would you like me to help you configure it?");
                } else {
                    evaluation += std::string("I noticed you mentioned ") + mentionedPlugins->get_length() + std::string(" plugins that need configuration: ");
                    evaluation += mentionedPlugins->map([=](auto p) mutable
                    {
                        return p["name"];
                    }
                    )->join(std::string(", ")) + std::string(". ");
                    evaluation += std::string("Would you like me to help you configure them?");
                }
            } else if (AND((unconfiguredPlugins->get_length() > 0), (activeDialogs->get_length() == 0))) {
                if (OR((OR((text->includes(std::string("plugin"))), (text->includes(std::string("setup"))))), (text->includes(std::string("configure"))))) {
                    evaluation += std::string("\
📋 **Plugin Status**: ");
                    evaluation += std::string("You have ") + unconfiguredPlugins->get_length() + std::string(" plugin") + (unconfiguredPlugins->get_length() > 1) ? std::string("s") : string_empty + std::string(" that need") + (unconfiguredPlugins->get_length() == 1) ? std::string("s") : string_empty + std::string(" configuration: ");
                    evaluation += unconfiguredPlugins->map([=](auto p) mutable
                    {
                        return p["name"];
                    }
                    )->join(std::string(", ")) + std::string(". ");
                    evaluation += std::string("Say "configure [plugin name]" to set up any of these plugins.");
                }
            } else if (activeDialogs->get_length() > 0) {
                auto activePlugin = const_(activeDialogs)[0];
                if (activePlugin->status == std::string("in_progress")) {
                    evaluation += std::string("\
⏳ **Ongoing Configuration**: ");
                    evaluation += std::string("You have an active configuration dialog for the ") + activePlugin->pluginName + std::string(" plugin. ");
                    evaluation += std::string("We're currently collecting: **") + activePlugin->currentVariable + std::string("**. ");
                    evaluation += std::string("Please provide the required value to continue.");
                }
            }
            if (AND((text->includes(std::string("install"))), (text->includes(std::string("plugin"))))) {
                if (unconfiguredPlugins->get_length() > 0) {
                    evaluation += std::string("\
⚠️ **Post-Installation**: ");
                    evaluation += std::string("After installing plugins, you may need to configure them with API keys or other settings. ");
                    evaluation += std::string("Check the plugin configuration status to see what needs to be set up.");
                }
            }
            return evaluation;
        }
        catch (const any& error)
        {
            logger->error(std::string("[pluginConfigurationEvaluator] Error in handler:"), error);
            return string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
