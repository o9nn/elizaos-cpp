#include "pluginConfigurationStatus.hpp"
#include <string>

std::shared_ptr<Provider> pluginConfigurationStatusProvider = object{
    object::pair{std::string("name"), std::string("pluginConfigurationStatus")}, 
    object::pair{std::string("description"), std::string("Provides basic plugin configuration status and missing environment variables")}, 
    object::pair{std::string("get"), [=](auto runtime, auto message = std::nullopt, auto state = std::nullopt) mutable
    {
        auto configService = as<std::shared_ptr<PluginConfigurationService>>(runtime->getService(PluginManagerServiceType["PLUGIN_CONFIGURATION"]));
        auto pluginManagerService = as<any>(runtime->getService(PluginManagerServiceType["PLUGIN_MANAGER"]));
        if (!configService) {
            return object{
                object::pair{std::string("text"), std::string("Configuration service not available")}, 
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
            object::pair{std::string("totalPlugins"), 0}, 
            object::pair{std::string("configuredPlugins"), 0}, 
            object::pair{std::string("needsConfiguration"), 0}
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
                        auto result = std::async([=]() { configService->parsePluginRequirements(std::string("./plugins/") + plugin["name"] + string_empty); });
                        if (AND((result), (result["requiredVars"]->get_length() > 0))) {
                            shared missingVars = std::async([=]() { configService->getMissingEnvVars(plugin["name"], std::string("./plugins/") + plugin["name"] + string_empty); });
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
                                        object::pair{std::string("configured"), !missingVars->includes(v->name)}
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
                        elizaLogger->warn(std::string("[pluginConfigurationStatus] Failed to check ") + plugin["name"] + std::string(":"), error);
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
            }
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
        }
        return object{
            object::pair{std::string("text"), statusText}, 
            object::pair{std::string("data"), statusData}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("configurationServicesAvailable"), true}, 
                object::pair{std::string("totalPlugins"), statusData["totalPlugins"]}, 
                object::pair{std::string("configuredPlugins"), statusData["configuredPlugins"]}, 
                object::pair{std::string("needsConfiguration"), statusData["needsConfiguration"]}, 
                object::pair{std::string("hasUnconfiguredPlugins"), statusData["needsConfiguration"] > 0}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
