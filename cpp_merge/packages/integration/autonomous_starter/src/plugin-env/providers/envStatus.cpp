#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/providers/envStatus.h"

string formatEnvVarStatus(std::shared_ptr<EnvVarConfig> config, boolean showValues)
{
    auto statusIcon = const_(object{
        object::pair{std:("missing"), std:("❌")}, 
        object::pair{std:("generating"), std:("⏳")}, 
        object::pair{std:("validating"), std:("🔍")}, 
        object::pair{std:("invalid"), std:("⚠️")}, 
        object::pair{std:("valid"), std:("✅")}
    })[config->status];
    auto typeLabel = config->type->replace(std:("_"), std:(" "))->toUpperCase();
    auto requiredLabel = (config->required) ? std:("Required") : std:("Optional");
    auto valueDisplay = std:("Not set");
    if (config->value) {
        if (OR((OR((config->type == std:("api_key")), (config->type == std:("secret")))), (config->type == std:("private_key")))) {
            valueDisplay = (showValues) ? any(config->value) (std:("****************"));
        } else {
            valueDisplay = config->value;
        }
    }
    auto statusText = string_empty + statusIcon + std:(" **") + (OR((config->description), (std:("Environment Variable")))) + std:("**\
");
    statusText += std:("   Type: ") + typeLabel + std:(" (") + requiredLabel + std:(")\
");
    statusText += std:("   Status: ") + config->status->toUpperCase() + std:("\
");
    statusText += std:("   Value: ") + valueDisplay + std:("\
");
    if (AND((config->canGenerate), (config->status == std:("missing")))) {
        statusText += std:("   🤖 Can be auto-generated\
");
    }
    if (config->lastError) {
        statusText += std:("   Error: ") + config->lastError + std:("\
");
    }
    return statusText;
};


string generateEnvStatusMessage(std::shared_ptr<EnvVarMetadata> envVars, boolean showValues)
{
    auto plugins = Object->keys(envVars);
    if (plugins->get_length() == 0) {
        return std:("No environment variables configured yet.");
    }
    auto totalVars = 0;
    auto missingRequired = 0;
    auto generatable = 0;
    auto needsUserInput = 0;
    auto validVars = 0;
    for (auto& plugin : Object->values(envVars))
    {
        for (auto& config : Object->values(plugin))
        {
            totalVars++;
            if (config->status == std:("valid")) {
                validVars++;
            } else if (AND((config->required), (config->status == std:("missing")))) {
                missingRequired++;
                if (config->canGenerate) {
                    generatable++;
                } else {
                    needsUserInput++;
                }
            }
        }
    }
    auto statusMessage = std:("# Environment Variables Status\
\
");
    statusMessage += std:("**Summary:** ") + validVars + std:("/") + totalVars + std:(" variables configured\
");
    if (missingRequired > 0) {
        statusMessage += std:("**Missing Required:** ") + missingRequired + std:(" variables\
");
        if (generatable > 0) {
            statusMessage += std:("**Auto-generatable:** ") + generatable + std:(" variables\
");
        }
        if (needsUserInput > 0) {
            statusMessage += std:("**Needs User Input:** ") + needsUserInput + std:(" variables\
");
        }
    }
    statusMessage += std:("\
");
    for (auto& [pluginName, plugin] : Object->entries(envVars))
    {
        statusMessage += std:("## ") + (pluginName->charAt(0)->toUpperCase() + pluginName->slice(1)) + std:(" Plugin\
\
");
        for (auto& [varName, config] : Object->entries(plugin))
        {
            statusMessage += std:("### ") + varName + std:("\
");
            statusMessage += formatEnvVarStatus(config, showValues);
            statusMessage += std:("\
");
        }
    }
    if (missingRequired > 0) {
        statusMessage += std:("\
## Recommended Actions\
\
");
        if (generatable > 0) {
            statusMessage += std:("1. **Generate Variables**: I can automatically generate ") + generatable + std:(" variables for you.\
");
        }
        if (needsUserInput > 0) {
            statusMessage += std:("2. **User Input Required**: ") + needsUserInput + std:(" variables need to be provided by you.\
");
        }
        statusMessage += std:("\
Use the SET_ENV_VAR action to configure variables or GENERATE_ENV_VAR to auto-generate them.\
");
    }
    return statusMessage;
};


std::shared_ptr<Provider> envStatusProvider = object{
    object::pair{std:("name"), std:("ENV_STATUS")}, 
    object::pair{std:("description"), std:("Current status of environment variables for all plugins")}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto worldId = as<std::shared_ptr<UUID>>(runtime->getSetting(std:("WORLD_ID")));
            if (!worldId) {
                logger->debug(std:("[EnvStatus] No WORLD_ID found, skipping env status provider"));
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("envVars"), object{}}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("envStatus"), std:("No world configuration found.")}, 
                        object::pair{std:("hasMissing"), false}, 
                        object::pair{std:("hasGeneratable"), false}, 
                        object::pair{std:("needsUserInput"), false}
                    }}, 
                    object::pair{std:("text"), std:("No world configuration found.")}
                };
            }
            auto world = std::async([=]() { runtime->getWorld(worldId); });
            if (!world->metadata->envVars) {
                logger->debug(std:("[EnvStatus] No environment variables configured yet"));
                return object{
                    object::pair{std:("data"), object{
                        object::pair{std:("envVars"), object{}}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("envStatus"), std:("No environment variables configured yet.")}, 
                        object::pair{std:("hasMissing"), false}, 
                        object::pair{std:("hasGeneratable"), false}, 
                        object::pair{std:("needsUserInput"), false}
                    }}, 
                    object::pair{std:("text"), std:("No environment variables configured yet.")}
                };
            }
            auto envVars = as<std::shared_ptr<EnvVarMetadata>>(world->metadata->envVars);
            auto hasMissing = false;
            auto hasGeneratable = false;
            auto needsUserInput = false;
            for (auto& plugin : Object->values(envVars))
            {
                for (auto& config : Object->values(plugin))
                {
                    if (AND((config->required), (config->status == std:("missing")))) {
                        hasMissing = true;
                        if (config->canGenerate) {
                            hasGeneratable = true;
                        } else {
                            needsUserInput = true;
                        }
                    }
                }
            }
            auto showValues = message->content->channelType == std:("DM");
            auto statusText = generateEnvStatusMessage(envVars, showValues);
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("envVars"), std:("envVars")}, 
                    object::pair{std:("summary"), object{
                        object::pair{std:("total"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->keys(plugin)->get_length();
                        }
                        , 0)}, 
                        object::pair{std:("missing"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->values(plugin)->filter([=](auto c) mutable
                            {
                                return AND((c->required), (c->status == std:("missing")));
                            }
                            )->get_length();
                        }
                        , 0)}, 
                        object::pair{std:("valid"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->values(plugin)->filter([=](auto c) mutable
                            {
                                return c->status == std:("valid");
                            }
                            )->get_length();
                        }
                        , 0)}
                    }}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("envStatus"), statusText}, 
                    object::pair{std:("hasMissing"), std:("hasMissing")}, 
                    object::pair{std:("hasGeneratable"), std:("hasGeneratable")}, 
                    object::pair{std:("needsUserInput"), std:("needsUserInput")}
                }}, 
                object::pair{std:("text"), statusText}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("[EnvStatus] Error in environment status provider:"), error);
            return object{
                object::pair{std:("data"), object{
                    object::pair{std:("envVars"), object{}}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("envStatus"), std:("Error retrieving environment variable status.")}, 
                    object::pair{std:("hasMissing"), false}, 
                    object::pair{std:("hasGeneratable"), false}, 
                    object::pair{std:("needsUserInput"), false}
                }}, 
                object::pair{std:("text"), std:("Error retrieving environment variable status.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
