#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/evaluators/pluginConfigurationEvaluator.h"

std::shared_ptr<Evaluator> pluginConfigurationEvaluator = object{
    object::pair{std:("name"), std:("pluginConfigurationEvaluator")}, 
    object::pair{std:("description"), std:("Evaluates plugin configuration needs and suggests configuration when plugins require environment variables")}, 
    object::pair{std:("examples"), array<any>()}, 
    object::pair{std:("alwaysRun"), false}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto configService = runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]);
            auto interactionService = runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]);
            if (OR((!configService), (!interactionService))) {
                return false;
            }
            shared text = message->content->text->toLowerCase();
            auto pluginKeywords = array<string>{ std:("plugin"), std:("configure"), std:("setup"), std:("install"), std:("environment"), std:("env var"), std:("api key"), std:("token"), std:("config") };
            return pluginKeywords->some([=](auto keyword) mutable
            {
                return text->includes(keyword);
            }
            );
        }
        catch (const any& error)
        {
            logger->error(std:("[pluginConfigurationEvaluator] Error in validation:"), error);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined) mutable
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
                            auto result = std::async([=]() { configService->parsePluginRequirements(std:("./plugins/") + plugin["name"] + string_empty); });
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
                                        object::pair{std:("name"), plugin["name"]}, 
                                        object::pair{std:("missingVars"), std:("missingVars")}, 
                                        object::pair{std:("requiredVars"), result["requiredVars"]}
                                    });
                                }
                            }
                        }
                        catch (const any& error)
                        {
                            logger->debug(std:("[pluginConfigurationEvaluator] Failed to check plugin ") + plugin["name"] + std:(":"), error);
                        }
                    }
                }
            }
            catch (const any& error)
            {
                logger->warn(std:("[pluginConfigurationEvaluator] Failed to check plugins:"), error);
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
                evaluation += std:("\
💡 **Configuration Suggestion**: ");
                if (mentionedPlugins->get_length() == 1) {
                    auto plugin = const_(mentionedPlugins)[0];
                    evaluation += std:("The ") + plugin["name"] + std:(" plugin requires configuration before it can be used. ");
                    evaluation += std:("It needs ") + plugin["missingVars"]->get_length() + std:(" environment variable") + (plugin["missingVars"]->get_length() > 1) ? std:("s") : string_empty + std:(": ");
                    evaluation += plugin["missingVars"]->map([=](auto v) mutable
                    {
                        return std:("**") + v + std:("**");
                    }
                    )->join(std:(", ")) + std:(". ");
                    evaluation += std:("Would you like me to help you configure it?");
                } else {
                    evaluation += std:("I noticed you mentioned ") + mentionedPlugins->get_length() + std:(" plugins that need configuration: ");
                    evaluation += mentionedPlugins->map([=](auto p) mutable
                    {
                        return p["name"];
                    }
                    )->join(std:(", ")) + std:(". ");
                    evaluation += std:("Would you like me to help you configure them?");
                }
            } else if (AND((unconfiguredPlugins->get_length() > 0), (activeDialogs->get_length() == 0))) {
                if (OR((OR((text->includes(std:("plugin"))), (text->includes(std:("setup"))))), (text->includes(std:("configure"))))) {
                    evaluation += std:("\
📋 **Plugin Status**: ");
                    evaluation += std:("You have ") + unconfiguredPlugins->get_length() + std:(" plugin") + (unconfiguredPlugins->get_length() > 1) ? std:("s") : string_empty + std:(" that need") + (unconfiguredPlugins->get_length() == 1) ? std:("s") : string_empty + std:(" configuration: ");
                    evaluation += unconfiguredPlugins->map([=](auto p) mutable
                    {
                        return p["name"];
                    }
                    )->join(std:(", ")) + std:(". ");
                    evaluation += std:("Say "configure [plugin name]" to set up any of these plugins.");
                }
            } else if (activeDialogs->get_length() > 0) {
                auto activePlugin = const_(activeDialogs)[0];
                if (activePlugin->status == std:("in_progress")) {
                    evaluation += std:("\
⏳ **Ongoing Configuration**: ");
                    evaluation += std:("You have an active configuration dialog for the ") + activePlugin->pluginName + std:(" plugin. ");
                    evaluation += std:("We're currently collecting: **") + activePlugin->currentVariable + std:("**. ");
                    evaluation += std:("Please provide the required value to continue.");
                }
            }
            if (AND((text->includes(std:("install"))), (text->includes(std:("plugin"))))) {
                if (unconfiguredPlugins->get_length() > 0) {
                    evaluation += std:("\
⚠️ **Post-Installation**: ");
                    evaluation += std:("After installing plugins, you may need to configure them with API keys or other settings. ");
                    evaluation += std:("Check the plugin configuration status to see what needs to be set up.");
                }
            }
            return evaluation;
        }
        catch (const any& error)
        {
            logger->error(std:("[pluginConfigurationEvaluator] Error in handler:"), error);
            return string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
