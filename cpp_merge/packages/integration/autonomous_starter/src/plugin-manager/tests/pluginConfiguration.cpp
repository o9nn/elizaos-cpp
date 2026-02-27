#include "pluginConfiguration.test.h"

void Main(void)
{
    describe(std:("Plugin Configuration System"), [=]() mutable
    {
        it(std:("should all required components"), [=]() mutable
        {
            expect(PluginConfigurationService)->toBeDefined();
            expect(PluginUserInteractionService)->toBeDefined();
            expect(startPluginConfigurationAction)->toBeDefined();
            expect(pluginConfigurationStatusProvider)->toBeDefined();
            expect(pluginConfigurationEvaluator)->toBeDefined();
            expect(pluginManagerPlugin)->toBeDefined();
        }
        );
        it(std:("should have correct plugin structure"), [=]() mutable
        {
            expect(pluginManagerPlugin->name)->toBe(std:("plugin-manager"));
            expect(pluginManagerPlugin->description)->toContain(std:("configuration management"));
            expect(pluginManagerPlugin->services)->toHaveLength(3);
            expect(pluginManagerPlugin->actions)->toHaveLength(4);
            expect(pluginManagerPlugin->providers)->toHaveLength(3);
            expect(pluginManagerPlugin->evaluators)->toHaveLength(1);
        }
        );
        it(std:("should have valid action structure"), [=]() mutable
        {
            expect(startPluginConfigurationAction->name)->toBe(std:("START_PLUGIN_CONFIGURATION"));
            expect(startPluginConfigurationAction->description)->toContain(std:("configuration dialog"));
            expect(startPluginConfigurationAction->validate)->toBeTypeOf(std:("function"));
            expect(startPluginConfigurationAction->handler)->toBeTypeOf(std:("function"));
        }
        );
        it(std:("should have valid provider structure"), [=]() mutable
        {
            expect(pluginConfigurationStatusProvider->name)->toBe(std:("pluginConfigurationStatus"));
            expect(pluginConfigurationStatusProvider->description)->toContain(std:("configuration status"));
            expect(pluginConfigurationStatusProvider->get)->toBeTypeOf(std:("function"));
        }
        );
        it(std:("should have valid evaluator structure"), [=]() mutable
        {
            expect(pluginConfigurationEvaluator->name)->toBe(std:("pluginConfigurationEvaluator"));
            expect(pluginConfigurationEvaluator->description)->toContain(std:("configuration needs"));
            expect(pluginConfigurationEvaluator->validate)->toBeTypeOf(std:("function"));
            expect(pluginConfigurationEvaluator->handler)->toBeTypeOf(std:("function"));
            expect(pluginConfigurationEvaluator->alwaysRun)->toBe(false);
        }
        );
    }
    );
}

MAIN
