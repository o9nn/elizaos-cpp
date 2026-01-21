#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/actions/startPluginConfiguration.h"

std::shared_ptr<Promise<any>> extractPluginNameFromMessage(std::shared_ptr<IAgentRuntime> runtime, string text)
{
    auto patterns = array<std::shared_ptr<RegExp>>{ (new RegExp(std::string("configure\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std::string("setup\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std::string("(\w+)\s+plugin\s+config"))), (new RegExp(std::string("set\s+up\s+(?:the\s+)?(\w+)\s+plugin"))), (new RegExp(std::string("configure\s+(\w+)"))) };
    for (auto& pattern : patterns)
    {
        auto match = text->match(pattern);
        if (AND((match), ((*const_(match))[1]))) {
            return (*const_(match))[1]->toLowerCase();
        }
    }
    try
    {
        auto prompt = std::string("Extract the plugin name from this user message about plugin configuration: "") + text + std::string(""\
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
        auto result = std::async([=]() { runtime->useModel(std::string("text"), object{
            object::pair{std::string("prompt"), std::string("prompt")}, 
            object::pair{std::string("temperature"), 0.1}, 
            object::pair{std::string("maxTokens"), 50}
        }); });
        auto extracted = result->trim()->toLowerCase();
        if (AND((AND((AND((extracted), (extracted != std::string("unknown")))), (extracted->length > 0))), (extracted->length < 50))) {
            return extracted;
        }
    }
    catch (const any& error)
    {
        logger->warn(std::string("[startPluginConfiguration] AI extraction failed:"), error);
    }
    return nullptr;
};


std::shared_ptr<Action> startPluginConfigurationAction = object{
    object::pair{std::string("name"), std::string("START_PLUGIN_CONFIGURATION")}, 
    object::pair{std::string("similes"), array<string>{ std::string("configure plugin"), std::string("setup plugin"), std::string("plugin configuration"), std::string("setup environment variables"), std::string("configure environment"), std::string("plugin setup"), std::string("set up plugin") }}, 
    object::pair{std::string("description"), std::string("Initiates configuration dialog for a plugin to collect required environment variables")}, 
    object::pair{std::string("examples"), array<any>()}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto configService = runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]);
            if (!configService) {
                logger->warn(std::string("[startPluginConfiguration] PluginConfigurationService not available"));
                return false;
            }
            auto interactionService = runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]);
            if (!interactionService) {
                logger->warn(std::string("[startPluginConfiguration] PluginUserInteractionService not available"));
                return false;
            }
            shared text = message->content->text->toLowerCase();
            auto configKeywords = array<string>{ std::string("configure"), std::string("setup"), std::string("config"), std::string("environment"), std::string("env var"), std::string("environment variable"), std::string("plugin config"), std::string("set up") };
            return configKeywords->some([=](auto keyword) mutable
            {
                return text->includes(keyword);
            }
            );
        }
        catch (const any& error)
        {
            logger->error(std::string("[startPluginConfiguration] Error in validation:"), error);
            return false;
        }
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            logger->info(std::string("[startPluginConfiguration] Starting plugin configuration process"));
            auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
            auto interactionService = as<std::shared_ptr<PluginUserInteractionService>>(runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]));
            if (OR((!configService), (!interactionService))) {
                return std::string("❌ Plugin configuration services are not available. Please check your setup.");
            }
            auto text = message->content->text->toLowerCase();
            auto pluginName = std::async([=]() { extractPluginNameFromMessage(runtime, text); });
            if (!pluginName) {
                return std::string("🔧 **Plugin Configuration**\
\
To help you configure a plugin, I need to know which plugin you'd like to set up. Could you please specify the plugin name?\
\
For example: "configure the openai plugin" or "setup discord plugin"");
            }
            auto result = std::async([=]() { configService->parsePluginRequirements(std::string("./plugins/") + pluginName + string_empty); });
            if (OR((!result), (result["requiredVars"]->get_length() == 0))) {
                return std::string("ℹ️ The plugin "") + pluginName + std::string("" doesn't require any configuration, or I couldn't find it. Please check the plugin name and try again.");
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
                return std::string("✅ The plugin "") + pluginName + std::string("" is already fully configured! All required environment variables are set.");
            }
            auto configRequest = object{
                object::pair{std::string("pluginName"), std::string("pluginName")}, 
                object::pair{std::string("requiredVars"), result["requiredVars"]}, 
                object::pair{std::string("missingVars"), std::string("missingVars")}, 
                object::pair{std::string("optionalVars"), result["optionalVars"]}
            };
            auto dialog = std::async([=]() { interactionService->initiateConfigurationDialog(configRequest, runtime->agentId); });
            if (missingVars->get_length() == 0) {
                return std::string("✅ The plugin "") + pluginName + std::string("" is already configured and ready to use!");
            }
            auto firstMissingVar = result["requiredVars"]->find([=](auto v) mutable
            {
                return missingVars->includes(v->name);
            }
            );
            if (!firstMissingVar) {
                return std::string("❌ Error: Could not find configuration details for required variables.");
            }
            auto firstPrompt = interactionService->generatePromptForVariable(firstMissingVar);
            return std::string("🎯 **Configuration Started**\
\
I'll help you configure the "") + pluginName + std::string("" plugin step by step.\
\
**Progress**: 1 of ") + missingVars->get_length() + std::string(" variables\
\
") + firstPrompt + string_empty;
        }
        catch (const any& error)
        {
            logger->error(std::string("[startPluginConfiguration] Error in handler:"), error);
            return std::string("❌ **Configuration Error**\
\
Sorry, I encountered an error while trying to start the plugin configuration. Please try again or check if the plugin exists.\
\
Error: ") + (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error")) + string_empty;
        }
    }
    }
};

void Main(void)
{
}

MAIN
