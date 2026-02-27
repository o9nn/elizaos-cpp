#include "index.test.h"

void Main(void)
{
    describe(std:("Plugin Manager Plugin"), [=]() mutable
    {
        it(std:("should a valid plugin structure"), [=]() mutable
        {
            expect(pluginManagerPlugin)->toBeDefined();
            expect(pluginManagerPlugin->name)->toBe(std:("plugin-manager"));
            expect(pluginManagerPlugin->description)->toBeDefined();
        }
        );
        it(std:("should have required services"), [=]() mutable
        {
            expect(pluginManagerPlugin->services)->toBeDefined();
            expect(pluginManagerPlugin->services->length)->toBeGreaterThan(0);
            auto serviceTypes = OR((pluginManagerPlugin->services->map([=](auto s) mutable
            {
                return s["serviceType"];
            }
            )), (array<any>()));
            expect(serviceTypes)->toContain(std:("plugin_manager"));
            expect(serviceTypes)->toContain(std:("plugin_configuration"));
        }
        );
        it(std:("should have core actions"), [=]() mutable
        {
            expect(pluginManagerPlugin->actions)->toBeDefined();
            expect(pluginManagerPlugin->actions->length)->toBeGreaterThan(0);
            auto actionNames = OR((pluginManagerPlugin->actions->map([=](auto a) mutable
            {
                return a["name"];
            }
            )), (array<any>()));
            expect(actionNames)->toContain(std:("LOAD_PLUGIN"));
            expect(actionNames)->toContain(std:("UNLOAD_PLUGIN"));
            expect(actionNames)->toContain(std:("SEARCH_PLUGINS"));
            expect(actionNames)->toContain(std:("CLONE_PLUGIN"));
            expect(actionNames)->toContain(std:("PUBLISH_PLUGIN"));
            expect(actionNames)->toContain(std:("installPluginFromRegistry"));
            expect(actionNames)->toContain(std:("GET_PLUGIN_DETAILS"));
        }
        );
        it(std:("should have required providers"), [=]() mutable
        {
            expect(pluginManagerPlugin->providers)->toBeDefined();
            expect(pluginManagerPlugin->providers->length)->toBeGreaterThan(0);
            auto providerNames = OR((pluginManagerPlugin->providers->map([=](auto p) mutable
            {
                return p["name"];
            }
            )), (array<any>()));
            expect(providerNames)->toContain(std:("pluginState"));
            expect(providerNames)->toContain(std:("pluginKnowledge"));
            expect(providerNames)->toContain(std:("registryPlugins"));
            expect(providerNames)->toContain(std:("pluginConfigurationStatus"));
        }
        );
        it(std:("should have empty evaluators array"), [=]() mutable
        {
            expect(pluginManagerPlugin->evaluators)->toBeDefined();
            expect(pluginManagerPlugin->evaluators)->toHaveLength(0);
        }
        );
        it(std:("should have empty routes array"), [=]() mutable
        {
            expect(pluginManagerPlugin->routes)->toBeDefined();
            expect(pluginManagerPlugin->routes)->toHaveLength(0);
        }
        );
        it(std:("should have an init function"), [=]() mutable
        {
            expect(pluginManagerPlugin->init)->toBeDefined();
            expect(type_of(pluginManagerPlugin->init))->toBe(std:("function"));
        }
        );
    }
    );
}

MAIN
