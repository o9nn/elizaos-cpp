#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/__tests__/e2e/registryIntegration.e2e.h"

void Main(void)
{
    vi->mock(std:("../../services/pluginRegistryService"), [=]() mutable
    {
        return (object{
            object::pair{std:("searchPluginsByContent"), vi->fn()}, 
            object::pair{std:("getPluginDetails"), vi->fn()}, 
            object::pair{std:("clonePlugin"), vi->fn()}
        });
    }
    );
    describe(std:("Registry Integration E2E Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent"))}, 
                object::pair{std:("actions"), array<any>()}, 
                object::pair{std:("registerAction"), [=](auto action) mutable
                {
                    return runtime->actions->push(action);
                }
                }, 
                object::pair{std:("getService"), [=](auto name) mutable
                {
                    return nullptr;
                }
                }
            });
            vi->clearAllMocks();
        }
        );
        describe(std:("searchPluginAction"), [=]() mutable
        {
            it(std:("should call searchPluginsByContent and format results"), [=]() mutable
            {
                auto mockResults = array<object>{ object{
                    object::pair{std:("name"), std:("@elizaos/plugin-weather")}, 
                    object::pair{std:("description"), std:("A weather plugin")}, 
                    object::pair{std:("score"), 0.9}, 
                    object::pair{std:("tags"), array<string>{ std:("weather") }}
                } };
                (as<any>(pluginRegistryService->searchPluginsByContent))["mockResolvedValue"](mockResults);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("search for weather plugins")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { searchPluginAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->searchPluginsByContent)->toHaveBeenCalledWith(std:("weather plugins"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Found 1 plugin"))}
                }));
            }
            );
        }
        );
        describe(std:("getPluginDetailsAction"), [=]() mutable
        {
            it(std:("should call getPluginDetails and format details"), [=]() mutable
            {
                auto mockDetails = object{
                    object::pair{std:("name"), std:("@elizaos/plugin-weather")}, 
                    object::pair{std:("description"), std:("A weather plugin")}, 
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("tags"), array<string>{ std:("weather") }}, 
                    object::pair{std:("features"), array<string>{ std:("current weather") }}
                };
                (as<any>(pluginRegistryService->getPluginDetails))["mockResolvedValue"](mockDetails);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("get details for @elizaos/plugin-weather")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { getPluginDetailsAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->getPluginDetails)->toHaveBeenCalledWith(std:("@elizaos/plugin-weather"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("text"), expect->stringContaining(std:("**@elizaos/plugin-weather** Details"))}
                }));
            }
            );
        }
        );
        describe(std:("clonePluginAction"), [=]() mutable
        {
            it(std:("should call clonePlugin and report success"), [=]() mutable
            {
                auto mockResult = object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("pluginName"), std:("@elizaos/plugin-weather")}, 
                    object::pair{std:("localPath"), std:("/cloned-plugins/plugin-weather")}
                };
                (as<any>(pluginRegistryService->clonePlugin))["mockResolvedValue"](mockResult);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("clone plugin @elizaos/plugin-weather")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { clonePluginAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->clonePlugin)->toHaveBeenCalledWith(std:("@elizaos/plugin-weather"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std:("text"), expect->stringContaining(std:("Successfully cloned"))}
                }));
            }
            );
        }
        );
    }
    );
}

MAIN
