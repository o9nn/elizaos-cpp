#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/providers/pluginConfigurationStatus.h"

std::shared_ptr<Provider> pluginConfigurationStatusProvider = object{
    object::pair{std::string("name"), std::string("pluginConfigurationStatus")}, 
    object::pair{std::string("description"), std::string("Provides information about plugin configuration status, missing environment variables, and active configuration dialogs")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message = undefined, auto state = undefined) mutable
    {
        try
        {
            auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
            auto interactionService = as<std::shared_ptr<PluginUserInteractionService>>(runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]));
            auto pluginManagerService = as<any>(runtime->getService(PluginManagerServiceType["PLUGIN_MANAGER"]));
            if (OR((!configService), (!interactionService))) {
                return object{
                    object::pair{std::string("text"), string_empty}, 
                    object::pair{std::string("data"), object{
                        object::pair{std::string("available"), false}
                    }}, 
                    object::pair{std::string("values"), object{
                        object::pair{std::string("configurationServicesAvailable"), false}
                    }}
                };
            }
            auto statusData = object{
                object::pair{std::string("available"), true}, 
                object::pair{std::string("plugins"), array<any>()}, 
                object::pair{std::string("activeDialogs"), array<any>()}, 
                object::pair{std::string("totalPlugins"), 0}, 
                object::pair{std::string("configuredPlugins"), 0}, 
                object::pair{std::string("needsConfiguration"), 0}
            };
            shared statusText = string_empty;
            if (pluginManagerService) {
                try
                {
                    auto allPlugins = pluginManagerService["getAllPlugins"]();
                    statusData["totalPlugins"] = allPlugins["length"];
                    auto& __array1742_4255 = allPlugins;
                    for (auto __indx1742_4255 = 0_N; __indx1742_4255 < __array1742_4255->get_length(); __indx1742_4255++)
                    {
                        auto& plugin = const_(__array1742_4255)[__indx1742_4255];
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
                                    auto pluginStatus = object{
                                        object::pair{std::string("name"), plugin["name"]}, 
                                        object::pair{std::string("status"), plugin["status"]}, 
                                        object::pair{std::string("requiredVars"), result["requiredVars"]->get_length()}, 
                                        object::pair{std::string("missingVars"), missingVars->get_length()}, 
                                        object::pair{std::string("configured"), missingVars->get_length() == 0}, 
                                        object::pair{std::string("variables"), result["requiredVars"]->map([=](auto v) mutable
                                        {
                                            return (object{
                                                object::pair{std::string("name"), v->name}, 
                                                object::pair{std::string("description"), v->description}, 
                                                object::pair{std::string("required"), v->required}, 
                                                object::pair{std::string("sensitive"), v->sensitive}, 
                                                object::pair{std::string("configured"), !!(*const_(currentConfig))[v->name]}
                                            });
                                        }
                                        )}
                                    };
                                    statusData["plugins"]["push"](pluginStatus);
                                    if (pluginStatus["configured"]) {
                                        statusData["configuredPlugins"]++;
                                    } else {
                                        statusData["needsConfiguration"]++;
                                    }
                                } else {
                                    statusData["plugins"]["push"](object{
                                        object::pair{std::string("name"), plugin["name"]}, 
                                        object::pair{std::string("status"), plugin["status"]}, 
                                        object::pair{std::string("requiredVars"), 0}, 
                                        object::pair{std::string("missingVars"), 0}, 
                                        object::pair{std::string("configured"), true}, 
                                        object::pair{std::string("variables"), array<any>()}
                                    });
                                    statusData["configuredPlugins"]++;
                                }
                            }
                            catch (const any& error)
                            {
                                logger->warn(std::string("[pluginConfigurationStatus] Failed to check configuration for plugin ") + plugin["name"] + std::string(":"), error);
                                statusData["plugins"]["push"](object{
                                    object::pair{std::string("name"), plugin["name"]}, 
                                    object::pair{std::string("status"), plugin["status"]}, 
                                    object::pair{std::string("requiredVars"), 0}, 
                                    object::pair{std::string("missingVars"), 0}, 
                                    object::pair{std::string("configured"), false}, 
                                    object::pair{std::string("variables"), array<any>()}, 
                                    object::pair{std::string("error"), std::string("Configuration check failed")}
                                });
                            }
                        }
                    }
                }
                catch (const any& error)
                {
                    logger->error(std::string("[pluginConfigurationStatus] Failed to get plugins:"), error);
                }
            }
            try
            {
                auto activeDialogs = interactionService->getActiveDialogs();
                statusData["activeDialogs"] = activeDialogs->map([=](auto dialog) mutable
                {
                    return (object{
                        object::pair{std::string("id"), dialog->id}, 
                        object::pair{std::string("pluginName"), dialog->pluginName}, 
                        object::pair{std::string("status"), dialog->status}, 
                        object::pair{std::string("currentVariable"), dialog->currentVariable}, 
                        object::pair{std::string("progress"), object{
                            object::pair{std::string("completed"), Object->keys(dialog->responses)->get_length()}, 
                            object::pair{std::string("total"), dialog->request->missingVars->get_length()}
                        }}, 
                        object::pair{std::string("startedAt"), dialog->startedAt}
                    });
                }
                );
            }
            catch (const any& error)
            {
                logger->warn(std::string("[pluginConfigurationStatus] Failed to get active dialogs:"), error);
            }
            if (statusData["totalPlugins"] == 0) {
                statusText = std::string("No plugins are currently installed.");
            } else {
                statusText += std::string("Plugin Configuration Status:\
");
                statusText += std::string("• Total plugins: ") + statusData["totalPlugins"] + std::string("\
");
                statusText += std::string("• Fully configured: ") + statusData["configuredPlugins"] + std::string("\
");
                statusText += std::string("• Need configuration: ") + statusData["needsConfiguration"] + std::string("\
");
                if (statusData["needsConfiguration"] > 0) {
                    statusText += std::string("\
Plugins needing configuration:\
");
                    statusData["plugins"]["filter"]([=](auto p) mutable
                    {
                        return !p["configured"];
                    }
                    )["forEach"]([=](auto plugin) mutable
                    {
                        statusText += std::string("• ") + plugin["name"] + std::string(": ") + plugin["missingVars"] + std::string(" missing variables\
");
                    }
                    );
                }
                if (statusData["activeDialogs"]["length"] > 0) {
                    statusText += std::string("\
Active configuration dialogs:\
");
                    statusData["activeDialogs"]["forEach"]([=](auto dialog) mutable
                    {
                        statusText += std::string("• ") + dialog["pluginName"] + std::string(": ") + dialog["progress"]["completed"] + std::string("/") + dialog["progress"]["total"] + std::string(" variables collected\
");
                    }
                    );
                }
            }
            return object{
                object::pair{std::string("text"), statusText}, 
                object::pair{std::string("data"), statusData}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("configurationServicesAvailable"), true}, 
                    object::pair{std::string("totalPlugins"), statusData["totalPlugins"]}, 
                    object::pair{std::string("configuredPlugins"), statusData["configuredPlugins"]}, 
                    object::pair{std::string("needsConfiguration"), statusData["needsConfiguration"]}, 
                    object::pair{std::string("activeDialogs"), statusData["activeDialogs"]["length"]}, 
                    object::pair{std::string("hasUnconfiguredPlugins"), statusData["needsConfiguration"] > 0}, 
                    object::pair{std::string("hasActiveDialogs"), statusData["activeDialogs"]["length"] > 0}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std::string("[pluginConfigurationStatus] Error getting plugin configuration status:"), error);
            return object{
                object::pair{std::string("text"), std::string("Error retrieving plugin configuration status.")}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Unknown error"))}
                }}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("configurationServicesAvailable"), false}, 
                    object::pair{std::string("error"), true}
                }}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
