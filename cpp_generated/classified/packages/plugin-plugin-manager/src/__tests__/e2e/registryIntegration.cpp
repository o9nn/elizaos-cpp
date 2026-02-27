#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-plugin-manager/src/__tests__/e2e/registryIntegration.e2e.h"

void Main(void)
{
    vi->mock(std::string("../../services/pluginRegistryService"), [=]() mutable
    {
        return (object{
            object::pair{std::string("searchPluginsByContent"), vi->fn()}, 
            object::pair{std::string("getPluginDetails"), vi->fn()}, 
            object::pair{std::string("clonePlugin"), vi->fn()}
        });
    }
    );
    describe(std::string("Registry Integration E2E Tests"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> runtime;
        beforeEach([=]() mutable
        {
            runtime = as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent"))}, 
                object::pair{std::string("actions"), array<any>()}, 
                object::pair{std::string("registerAction"), [=](auto action) mutable
                {
                    return runtime->actions->push(action);
                }
                }, 
                object::pair{std::string("getService"), [=](auto name) mutable
                {
                    return nullptr;
                }
                }
            });
            vi->clearAllMocks();
        }
        );
        describe(std::string("searchPluginAction"), [=]() mutable
        {
            it(std::string("should call searchPluginsByContent and format results"), [=]() mutable
            {
                auto mockResults = array<object>{ object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-weather")}, 
                    object::pair{std::string("description"), std::string("A weather plugin")}, 
                    object::pair{std::string("score"), 0.9}, 
                    object::pair{std::string("tags"), array<string>{ std::string("weather") }}
                } };
                (as<any>(pluginRegistryService->searchPluginsByContent))["mockResolvedValue"](mockResults);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("search for weather plugins")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { searchPluginAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->searchPluginsByContent)->toHaveBeenCalledWith(std::string("weather plugins"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Found 1 plugin"))}
                }));
            }
            );
        }
        );
        describe(std::string("getPluginDetailsAction"), [=]() mutable
        {
            it(std::string("should call getPluginDetails and format details"), [=]() mutable
            {
                auto mockDetails = object{
                    object::pair{std::string("name"), std::string("@elizaos/plugin-weather")}, 
                    object::pair{std::string("description"), std::string("A weather plugin")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("tags"), array<string>{ std::string("weather") }}, 
                    object::pair{std::string("features"), array<string>{ std::string("current weather") }}
                };
                (as<any>(pluginRegistryService->getPluginDetails))["mockResolvedValue"](mockDetails);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("get details for @elizaos/plugin-weather")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { getPluginDetailsAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->getPluginDetails)->toHaveBeenCalledWith(std::string("@elizaos/plugin-weather"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("**@elizaos/plugin-weather** Details"))}
                }));
            }
            );
        }
        );
        describe(std::string("clonePluginAction"), [=]() mutable
        {
            it(std::string("should call clonePlugin and report success"), [=]() mutable
            {
                auto mockResult = object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("pluginName"), std::string("@elizaos/plugin-weather")}, 
                    object::pair{std::string("localPath"), std::string("/cloned-plugins/plugin-weather")}
                };
                (as<any>(pluginRegistryService->clonePlugin))["mockResolvedValue"](mockResult);
                auto message = as<std::shared_ptr<Memory>>(object{
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("clone plugin @elizaos/plugin-weather")}
                    }}
                });
                auto callback = vi->fn();
                std::async([=]() { clonePluginAction->handler(runtime, message, undefined, undefined, callback); });
                expect(pluginRegistryService->clonePlugin)->toHaveBeenCalledWith(std::string("@elizaos/plugin-weather"));
                expect(callback)->toHaveBeenCalledWith(expect->objectContaining(object{
                    object::pair{std::string("text"), expect->stringContaining(std::string("Successfully cloned"))}
                }));
            }
            );
        }
        );
    }
    );
}

MAIN
