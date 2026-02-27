#include "index.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Plugin Manager Plugin"), [=]() mutable
    {
        it(std::string("should a valid plugin structure"), [=]() mutable
        {
            expect(pluginManagerPlugin)->toBeDefined();
            expect(pluginManagerPlugin->name)->toBe(std::string("plugin-manager"));
            expect(pluginManagerPlugin->description)->toBeDefined();
        }
        );
        it(std::string("should have required services"), [=]() mutable
        {
            expect(pluginManagerPlugin->services)->toBeDefined();
            expect(pluginManagerPlugin->services->length)->toBeGreaterThan(0);
            auto serviceTypes = OR((pluginManagerPlugin->services->map([=](auto s) mutable
            {
                return s["serviceType"];
            }
            )), (array<any>()));
            expect(serviceTypes)->toContain(std::string("plugin_manager"));
            expect(serviceTypes)->toContain(std::string("plugin_configuration"));
        }
        );
        it(std::string("should have core actions"), [=]() mutable
        {
            expect(pluginManagerPlugin->actions)->toBeDefined();
            expect(pluginManagerPlugin->actions->length)->toBeGreaterThan(0);
            auto actionNames = OR((pluginManagerPlugin->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            )), (array<any>()));
            expect(actionNames)->toContain(std::string("LOAD_PLUGIN"));
            expect(actionNames)->toContain(std::string("UNLOAD_PLUGIN"));
            expect(actionNames)->toContain(std::string("SEARCH_PLUGINS"));
            expect(actionNames)->toContain(std::string("CLONE_PLUGIN"));
            expect(actionNames)->toContain(std::string("PUBLISH_PLUGIN"));
            expect(actionNames)->toContain(std::string("installPluginFromRegistry"));
            expect(actionNames)->toContain(std::string("GET_PLUGIN_DETAILS"));
        }
        );
        it(std::string("should have required providers"), [=]() mutable
        {
            expect(pluginManagerPlugin->providers)->toBeDefined();
            expect(pluginManagerPlugin->providers->length)->toBeGreaterThan(0);
            auto providerNames = OR((pluginManagerPlugin->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            )), (array<any>()));
            expect(providerNames)->toContain(std::string("pluginState"));
            expect(providerNames)->toContain(std::string("pluginKnowledge"));
            expect(providerNames)->toContain(std::string("registryPlugins"));
            expect(providerNames)->toContain(std::string("pluginConfigurationStatus"));
        }
        );
        it(std::string("should have empty evaluators array"), [=]() mutable
        {
            expect(pluginManagerPlugin->evaluators)->toBeDefined();
            expect(pluginManagerPlugin->evaluators)->toHaveLength(0);
        }
        );
        it(std::string("should have empty routes array"), [=]() mutable
        {
            expect(pluginManagerPlugin->routes)->toBeDefined();
            expect(pluginManagerPlugin->routes)->toHaveLength(0);
        }
        );
        it(std::string("should have an init function"), [=]() mutable
        {
            expect(pluginManagerPlugin->init)->toBeDefined();
            expect(type_of(pluginManagerPlugin->init))->toBe(std::string("function"));
        }
        );
    }
    );
}

MAIN
