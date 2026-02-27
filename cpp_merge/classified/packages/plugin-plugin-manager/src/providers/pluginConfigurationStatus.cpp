#include "pluginConfigurationStatus.hpp"

std::shared_ptr<Provider> pluginConfigurationStatusProvider = object{
    object::pair{std:("name"), std:("pluginConfigurationStatus")}, 
    object::pair{std:("description"), std:("Provides basic plugin configuration status and missing environment variables")}, 
    object::pair{std:("get"), [=](auto runtime, auto message = undefined, auto state = undefined) mutable
    {
        auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
        auto pluginManagerService = as<any>(runtime->getService(PluginManagerServiceType["PLUGIN_MANAGER"]));
        if (!configService) {
            return object{
                object::pair{std:("text"), std:("Configuration service not available")}, 
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
            object::pair{std:("totalPlugins"), 0}, 
            object::pair{std:("configuredPlugins"), 0}, 
            object::pair{std:("needsConfiguration"), 0}
        };
        shared statusText = string_empty;
        if (pluginManagerService) {
            auto allPlugins = pluginManagerService["getAllPlugins"]();
            statusData["totalPlugins"] = allPlugins["length"];
            auto& __array1352_3432 = allPlugins;
            for (auto __indx1352_3432 = 0_N; __indx1352_3432 < __array1352_3432->get_length(); __indx1352_3432++)
            {
                auto& plugin = const_(__array1352_3432)[__indx1352_3432];
                {
                    try
                    {
                        auto result = std::async([=]() { configService->parsePluginRequirements(std:("./plugins/") + plugin["name"] + string_empty); });
                        if (AND((result), (result["requiredVars"]->get_length() > 0))) {
                            shared missingVars = std::async([=]() { configService->getMissingEnvVars(plugin["name"], std:("./plugins/") + plugin["name"] + string_empty); });
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
                                        object::pair{std:("configured"), !missingVars->includes(v->name)}
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
                        elizaLogger->warn(std:("[pluginConfigurationStatus] Failed to check ") + plugin["name"] + std:(":"), error);
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
            }
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
        }
        return object{
            object::pair{std:("text"), statusText}, 
            object::pair{std:("data"), statusData}, 
            object::pair{std:("values"), object{
                object::pair{std:("configurationServicesAvailable"), true}, 
                object::pair{std:("totalPlugins"), statusData["totalPlugins"]}, 
                object::pair{std:("configuredPlugins"), statusData["configuredPlugins"]}, 
                object::pair{std:("needsConfiguration"), statusData["needsConfiguration"]}, 
                object::pair{std:("hasUnconfiguredPlugins"), statusData["needsConfiguration"] > 0}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
