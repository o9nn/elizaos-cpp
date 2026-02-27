#include "pluginConfigurationStatus.hpp"

std::shared_ptr<Provider> pluginConfigurationStatusProvider = object{
    object::pair{std:("name"), std:("pluginConfigurationStatus")}, 
    object::pair{std:("description"), std:("Provides information about plugin configuration status, missing environment variables, and active configuration dialogs")}, 
    object::pair{std:("get"), [=](auto runtime, auto message = undefined, auto state = undefined) mutable
    {
        try
        {
            auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
            auto interactionService = as<std::shared_ptr<PluginUserInteractionService>>(runtime->getService(PluginManagerServiceType["PLUGIN_USER_INTERACTION"]));
            auto pluginManagerService = as<any>(runtime->getService(PluginManagerServiceType["PLUGIN_MANAGER"]));
            if (OR((!configService), (!interactionService))) {
                return object{
                    object::pair{std:("text"), string_empty}, 
                    object::pair{std:("data"), object{
                        object::pair{std:("available"), false}
                    }}, 
                    object::pair{std:("values"), object{
                        object::pair{std:("configurationServicesAvailable"), false}
                    }}
                };
            }
            auto statusData = object{
                object::pair{std:("available"), true}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("activeDialogs"), array<any>()}, 
                object::pair{std:("totalPlugins"), 0}, 
                object::pair{std:("configuredPlugins"), 0}, 
                object::pair{std:("needsConfiguration"), 0}
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
                                    auto pluginStatus = object{
                                        object::pair{std:("name"), plugin["name"]}, 
                                        object::pair{std:("status"), plugin["status"]}, 
                                        object::pair{std:("requiredVars"), result["requiredVars"]->get_length()}, 
                                        object::pair{std:("missingVars"), missingVars->get_length()}, 
                                        object::pair{std:("configured"), missingVars->get_length() == 0}, 
                                        object::pair{std:("variables"), result["requiredVars"]->map([=](auto v) mutable
                                        {
                                            return (object{
                                                object::pair{std:("name"), v->name}, 
                                                object::pair{std:("description"), v->description}, 
                                                object::pair{std:("required"), v->required}, 
                                                object::pair{std:("sensitive"), v->sensitive}, 
                                                object::pair{std:("configured"), !!(*const_(currentConfig))[v->name]}
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
                                        object::pair{std:("name"), plugin["name"]}, 
                                        object::pair{std:("status"), plugin["status"]}, 
                                        object::pair{std:("requiredVars"), 0}, 
                                        object::pair{std:("missingVars"), 0}, 
                                        object::pair{std:("configured"), true}, 
                                        object::pair{std:("variables"), array<any>()}
                                    });
                                    statusData["configuredPlugins"]++;
                                }
                            }
                            catch (const any& error)
                            {
                                logger->warn(std:("[pluginConfigurationStatus] Failed to check configuration for plugin ") + plugin["name"] + std:(":"), error);
                                statusData["plugins"]["push"](object{
                                    object::pair{std:("name"), plugin["name"]}, 
                                    object::pair{std:("status"), plugin["status"]}, 
                                    object::pair{std:("requiredVars"), 0}, 
                                    object::pair{std:("missingVars"), 0}, 
                                    object::pair{std:("configured"), false}, 
                                    object::pair{std:("variables"), array<any>()}, 
                                    object::pair{std:("error"), std:("Configuration check failed")}
                                });
                            }
                        }
                    }
                }
                catch (const any& error)
                {
                    logger->error(std:("[pluginConfigurationStatus] Failed to get plugins:"), error);
                }
            }
            try
            {
                auto activeDialogs = interactionService->getActiveDialogs();
                statusData["activeDialogs"] = activeDialogs->map([=](auto dialog) mutable
                {
                    return (object{
                        object::pair{std:("id"), dialog->id}, 
                        object::pair{std:("pluginName"), dialog->pluginName}, 
                        object::pair{std:("status"), dialog->status}, 
                        object::pair{std:("currentVariable"), dialog->currentVariable}, 
                        object::pair{std:("progress"), object{
                            object::pair{std:("completed"), Object->keys(dialog->responses)->get_length()}, 
                            object::pair{std:("total"), dialog->request->missingVars->get_length()}
                        }}, 
                        object::pair{std:("startedAt"), dialog->startedAt}
                    });
                }
                );
            }
            catch (const any& error)
            {
                logger->warn(std:("[pluginConfigurationStatus] Failed to get active dialogs:"), error);
            }
            if (statusData["totalPlugins"] == 0) {
                statusText = std:("No plugins are currently installed.");
            } else {
                statusText += std:("Plugin Configuration Status:\
");
                statusText += std:("• Total plugins: ") + statusData["totalPlugins"] + std:("\
");
                statusText += std:("• Fully configured: ") + statusData["configuredPlugins"] + std:("\
");
                statusText += std:("• Need configuration: ") + statusData["needsConfiguration"] + std:("\
");
                if (statusData["needsConfiguration"] > 0) {
                    statusText += std:("\
Plugins needing configuration:\
");
                    statusData["plugins"]["filter"]([=](auto p) mutable
                    {
                        return !p["configured"];
                    }
                    )["forEach"]([=](auto plugin) mutable
                    {
                        statusText += std:("• ") + plugin["name"] + std:(": ") + plugin["missingVars"] + std:(" missing variables\
");
                    }
                    );
                }
                if (statusData["activeDialogs"]["length"] > 0) {
                    statusText += std:("\
Active configuration dialogs:\
");
                    statusData["activeDialogs"]["forEach"]([=](auto dialog) mutable
                    {
                        statusText += std:("• ") + dialog["pluginName"] + std:(": ") + dialog["progress"]["completed"] + std:("/") + dialog["progress"]["total"] + std:(" variables collected\
");
                    }
                    );
                }
            }
            return object{
                object::pair{std:("text"), statusText}, 
                object::pair{std:("data"), statusData}, 
                object::pair{std:("values"), object{
                    object::pair{std:("configurationServicesAvailable"), true}, 
                    object::pair{std:("totalPlugins"), statusData["totalPlugins"]}, 
                    object::pair{std:("configuredPlugins"), statusData["configuredPlugins"]}, 
                    object::pair{std:("needsConfiguration"), statusData["needsConfiguration"]}, 
                    object::pair{std:("activeDialogs"), statusData["activeDialogs"]["length"]}, 
                    object::pair{std:("hasUnconfiguredPlugins"), statusData["needsConfiguration"] > 0}, 
                    object::pair{std:("hasActiveDialogs"), statusData["activeDialogs"]["length"] > 0}
                }}
            };
        }
        catch (const any& error)
        {
            logger->error(std:("[pluginConfigurationStatus] Error getting plugin configuration status:"), error);
            return object{
                object::pair{std:("text"), std:("Error retrieving plugin configuration status.")}, 
                object::pair{std:("data"), object{
                    object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Unknown error"))}
                }}, 
                object::pair{std:("values"), object{
                    object::pair{std:("configurationServicesAvailable"), false}, 
                    object::pair{std:("error"), true}
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
