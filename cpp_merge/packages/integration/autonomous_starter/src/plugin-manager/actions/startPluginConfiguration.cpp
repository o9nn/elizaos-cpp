#include "startPluginConfiguration.hpp"

std::shared_ptr<Promise<any>> extractPluginNameFromMessage(std::shared_ptr<IAgentRuntime> runtime, string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std:("configure\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std:("setup\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std:("(\w+)\s+plugin\s+config"))), (new RegExp(std:("set\s+up\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std:("configure\s+(\w+)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (AND((match), ((*const_(match))[1]))) {
            return (*const_(match))[1]->toLowerCase();
        }
    }
    try
    {
        auto prompt = std:("Extract the plugin name from this user message about plugin configuration: "") + text + std:(""\
\
If the user mentions a specific plugin name, return just the plugin name (lowercase, no spaces).\
If no specific plugin is mentioned, return "unknown".\
\
Examples:\
- "configure the openai plugin" → "openai"\
- "setup discord plugin" → "discord"\
- "I want to configure twitter" → "twitter"\
- "setup plugin environment variables" → "unknown"\
\
Plugin name:");
        auto result = std::async([=]() { runtime->useModel(std:("text"), object{
            object::pair{std:("prompt"), std:("prompt")}, 
            object::pair{std:("temperature"), 0.1}, 
            object::pair{std:("maxTokens"), 50}
        }); });
        auto extracted = result->trim()->toLowerCase();
        if (AND((AND((AND((extracted), (extracted != std:("unknown")))), (extracted->length > 0))), (extracted->length < 50))) {
            return extracted;
        }
    }
    catch (const any& error)
    {
        logger->warn(std:("[startPluginConfiguration] AI extraction failed:"), error);
    }
    return nullptr;
};


std::shared_ptr<Action> startPluginConfigurationAction = object{
    object::pair{std:("name"), std:("START_PLUGIN_CONFIGURATION")}, 
    object::pair{std:("similes"), array<string>{ std:("configure plugin"), std:("setup plugin"), std:("plugin configuration"), std:("setup environment variables"), std:("configure environment"), std:("plugin setup"), std:("set up plugin") }}, 
    object::pair{std:("description"), std:("Initiates configuration dialog for a plugin to collect required environment variables")}, 
    object::pair{std:("examples"), array<any>()}, 
    object::pair{std:("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto configService = runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]);
            if (!configService) {
                logger->warn(std:("[startPluginConfiguration] PluginConfigurationService not available"));
                return false;
            }
            auto interactionService = runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]);
            if (!interactionService) {
                logger->warn(std:("[startPluginConfiguration] PluginUserInteractionService not available"));
                return false;
            }
            shared text = message->content->text->toLowerCase();
            auto configKeywords = array<string>{ std:("configure"), std:("setup"), std:("config"), std:("environment"), std:("env var"), std:("environment variable"), std:("plugin config"), std:("set up") };
            return configKeywords->some([=](auto keyword) mutable
            {
                return text->includes(keyword);
            }
            );
        }
        catch (const any& error)
        {
            logger->error(std:("[startPluginConfiguration] Error in validation:"), error);
            return false;
        }
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            logger->info(std:("[startPluginConfiguration] Starting plugin configuration process"));
            auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
            auto interactionService = as<std::shared_ptr<PluginUserInteractionService>>(runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]));
            if (OR((!configService), (!interactionService))) {
                return std:("❌ Plugin configuration services are not available. Please check your setup.");
            }
            auto text = message->content->text->toLowerCase();
            auto pluginName = std::async([=]() { extractPluginNameFromMessage(runtime, text); });
            if (!pluginName) {
                return std:("🔧 **Plugin Configuration**\
\
To help you configure a plugin, I need to know which plugin you'd like to set up. Could you please specify the plugin name?\
\
For example: "configure the openai plugin" or "setup discord plugin"");
            }
            auto result = std::async([=]() { configService->parsePluginRequirements(std:("./plugins/") + pluginName + string_empty); });
            if (OR((!result), (result["requiredVars"]->get_length() == 0))) {
                return std:("ℹ️ The plugin "") + pluginName + std:("" doesn't require any configuration, or I couldn't find it. Please check the plugin name and try again.");
            }
            shared currentConfig = std::async([=]() { configService->getPluginConfiguration(pluginName); });
            shared missingVars = result["requiredVars"]->filter([=](auto varInfo) mutable
            {
                return !(*const_(currentConfig))[varInfo->name];
            }
            )->map([=](auto varInfo) mutable
            {
                return varInfo->name;
            }
            );
            if (missingVars->get_length() == 0) {
                return std:("✅ The plugin "") + pluginName + std:("" is already fully configured! All required environment variables are set.");
            }
            auto configRequest = object{
                object::pair{std:("pluginName"), std:("pluginName")}, 
                object::pair{std:("requiredVars"), result["requiredVars"]}, 
                object::pair{std:("missingVars"), std:("missingVars")}, 
                object::pair{std:("optionalVars"), result["optionalVars"]}
            };
            auto dialog = std::async([=]() { interactionService->initiateConfigurationDialog(configRequest, runtime->agentId); });
            if (missingVars->get_length() == 0) {
                return std:("✅ The plugin "") + pluginName + std:("" is already configured and ready to use!");
            }
            auto firstMissingVar = result["requiredVars"]->find([=](auto v) mutable
            {
                return missingVars->includes(v->name);
            }
            );
            if (!firstMissingVar) {
                return std:("❌ Error: Could not find configuration details for required variables.");
            }
            auto firstPrompt = interactionService->generatePromptForVariable(firstMissingVar);
            return std:("🎯 **Configuration Started**\
\
I'll help you configure the "") + pluginName + std:("" plugin step by step.\
\
**Progress**: 1 of ") + missingVars->get_length() + std:(" variables\
\
") + firstPrompt + string_empty;
        }
        catch (const any& error)
        {
            logger->error(std:("[startPluginConfiguration] Error in handler:"), error);
            return std:("❌ **Configuration Error**\
\
Sorry, I encountered an error while trying to start the plugin configuration. Please try again or check if the plugin exists.\
\
Error: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
