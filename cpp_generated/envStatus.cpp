#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/providers/envStatus.h"

string formatEnvVarStatus(std::shared_ptr<EnvVarConfig> config, boolean showValues)
{
    auto statusIcon = const_(object{
        object::pair{std::string("missing"), std::string("❌")}, 
        object::pair{std::string("generating"), std::string("⏳")}, 
        object::pair{std::string("validating"), std::string("🔍")}, 
        object::pair{std::string("invalid"), std::string("⚠️")}, 
        object::pair{std::string("valid"), std::string("✅")}
    })[config->status];
    auto typeLabel = config->type->replace(std::string("_"), std::string(" "))->toUpperCase();
    auto requiredLabel = (config->required) ? std::string("Required") : std::string("Optional");
    auto valueDisplay = std::string("Not set");
    if (config->value) {
        if (OR((OR((config->type == std::string("api_key")), (config->type == std::string("secret")))), (config->type == std::string("private_key")))) {
            valueDisplay = (showValues) ? any(config->value) : any(std::string("****************"));
        } else {
            valueDisplay = config->value;
        }
    }
    auto statusText = string_empty + statusIcon + std::string(" **") + (OR((config->description), (std::string("Environment Variable")))) + std::string("**\
");
    statusText += std::string("   Type: ") + typeLabel + std::string(" (") + requiredLabel + std::string(")\
");
    statusText += std::string("   Status: ") + config->status->toUpperCase() + std::string("\
");
    statusText += std::string("   Value: ") + valueDisplay + std::string("\
");
    if (AND((config->canGenerate), (config->status == std::string("missing")))) {
        statusText += std::string("   🤖 Can be auto-generated\
");
    }
    if (config->lastError) {
        statusText += std::string("   Error: ") + config->lastError + std::string("\
");
    }
    return statusText;
};


string generateEnvStatusMessage(std::shared_ptr<EnvVarMetadata> envVars, boolean showValues)
{
    auto plugins = Object->keys(envVars);
    if (plugins->get_length() == 0) {
        return std::string("No environment variables configured yet.");
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
            if (config->status == std::string("valid")) {
                validVars++;
            } else if (AND((config->required), (config->status == std::string("missing")))) {
                missingRequired++;
                if (config->canGenerate) {
                    generatable++;
                } else {
                    needsUserInput++;
                }
            }
        }
    }
    auto statusMessage = std::string("# Environment Variables Status\
\
");
    statusMessage += std::string("**Summary:** ") + validVars + std::string("/") + totalVars + std::string(" variables configured\
");
    if (missingRequired > 0) {
        statusMessage += std::string("**Missing Required:** ") + missingRequired + std::string(" variables\
");
        if (generatable > 0) {
            statusMessage += std::string("**Auto-generatable:** ") + generatable + std::string(" variables\
");
        }
        if (needsUserInput > 0) {
            statusMessage += std::string("**Needs User Input:** ") + needsUserInput + std::string(" variables\
");
        }
    }
    statusMessage += std::string("\
");
    for (auto& [pluginName, plugin] : Object->entries(envVars))
    {
        statusMessage += std::string("## ") + (pluginName->charAt(0)->toUpperCase() + pluginName->slice(1)) + std::string(" Plugin\
\
");
        for (auto& [varName, config] : Object->entries(plugin))
        {
            statusMessage += std::string("### ") + varName + std::string("\
");
            statusMessage += formatEnvVarStatus(config, showValues);
            statusMessage += std::string("\
");
        }
    }
    if (missingRequired > 0) {
        statusMessage += std::string("\
## Recommended Actions\
\
");
        if (generatable > 0) {
            statusMessage += std::string("1. **Generate Variables**: I can automatically generate ") + generatable + std::string(" variables for you.\
");
        }
        if (needsUserInput > 0) {
            statusMessage += std::string("2. **User Input Required**: ") + needsUserInput + std::string(" variables need to be provided by you.\
");
        }
        statusMessage += std::string("\
Use the SET_ENV_VAR action to configure variables or GENERATE_ENV_VAR to auto-generate them.\
");
    }
    return statusMessage;
};


std::shared_ptr<Provider> envStatusProvider = object{
    object::pair{std::string("name"), std::string("ENV_STATUS")}, 
    object::pair{std::string("description"), std::string("Current status of environment variables for all plugins")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto worldId = as<std::shared_ptr<UUID>>(runtime->getSetting(std::string("WORLD_ID")));
            if (!worldId) {
                logger->debug(std::string("[EnvStatus] No WORLD_ID found, skipping env status provider"));
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("envVars"), object{}}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("envStatus"), std::string("No world configuration found.")}, 
                        object::pair{std::string("hasMissing"), false}, 
                        object::pair{std::string("hasGeneratable"), false}, 
                        object::pair{std::string("needsUserInput"), false}
                    }}, 
                    object::pair{std::string("text"), std::string("No world configuration found.")}
                };
            }
            auto world = std::async([=]() { runtime->getWorld(worldId); });
            if (!world->metadata->envVars) {
                logger->debug(std::string("[EnvStatus] No environment variables configured yet"));
                return object{
                    object::pair{std::string("data"), object{
                        object::pair{std::string("envVars"), object{}}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("envStatus"), std::string("No environment variables configured yet.")}, 
                        object::pair{std::string("hasMissing"), false}, 
                        object::pair{std::string("hasGeneratable"), false}, 
                        object::pair{std::string("needsUserInput"), false}
                    }}, 
                    object::pair{std::string("text"), std::string("No environment variables configured yet.")}
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
                    if (AND((config->required), (config->status == std::string("missing")))) {
                        hasMissing = true;
                        if (config->canGenerate) {
                            hasGeneratable = true;
                        } else {
                            needsUserInput = true;
                        }
                    }
                }
            }
            auto showValues = message->content->channelType == std::string("DM");
            auto statusText = generateEnvStatusMessage(envVars, showValues);
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("envVars"), std::string("envVars")}, 
                    object::pair{std::string("summary"), object{
                        object::pair{std::string("total"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->keys(plugin)->get_length();
                        }
                        , 0)}, 
                        object::pair{std::string("missing"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->values(plugin)->filter([=](auto c) mutable
                            {
                                return AND((c->required), (c->status == std::string("missing")));
                            }
                            )->get_length();
                        }
                        , 0)}, 
                        object::pair{std::string("valid"), Object->values(envVars)->reduce([=](auto sum, auto plugin) mutable
                        {
                            return sum + Object->values(plugin)->filter([=](auto c) mutable
                            {
                                return c->status == std::string("valid");
                            }
                            )->get_length();
                        }
                        , 0)}
                    }}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("envStatus"), statusText}, 
                    object::pair{std::string("hasMissing"), std::string("hasMissing")}, 
                    object::pair{std::string("hasGeneratable"), std::string("hasGeneratable")}, 
                    object::pair{std::string("needsUserInput"), std::string("needsUserInput")}
                }}, 
                object::pair{std::string("text"), statusText}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("[EnvStatus] Error in environment status provider:"), error);
            return object{
                object::pair{std::string("data"), object{
                    object::pair{std::string("envVars"), object{}}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("envStatus"), std::string("Error retrieving environment variable status.")}, 
                    object::pair{std::string("hasMissing"), false}, 
                    object::pair{std::string("hasGeneratable"), false}, 
                    object::pair{std::string("needsUserInput"), false}
                }}, 
                object::pair{std::string("text"), std::string("Error retrieving environment variable status.")}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
