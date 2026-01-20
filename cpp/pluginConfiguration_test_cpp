#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-manager/tests/pluginConfiguration.test.h"

void Main(void)
{
    describe(std::string("Plugin Configuration System"), [=]() mutable
    {
        it(std::string("should export all required components"), [=]() mutable
        {
            expect(PluginConfigurationService)->toBeDefined();
            expect(PluginUserInteractionService)->toBeDefined();
            expect(startPluginConfigurationAction)->toBeDefined();
            expect(pluginConfigurationStatusProvider)->toBeDefined();
            expect(pluginConfigurationEvaluator)->toBeDefined();
            expect(pluginManagerPlugin)->toBeDefined();
        }
        );
        it(std::string("should have correct plugin structure"), [=]() mutable
        {
            expect(pluginManagerPlugin->name)->toBe(std::string("plugin-manager"));
            expect(pluginManagerPlugin->description)->toContain(std::string("configuration management"));
            expect(pluginManagerPlugin->services)->toHaveLength(3);
            expect(pluginManagerPlugin->actions)->toHaveLength(4);
            expect(pluginManagerPlugin->providers)->toHaveLength(3);
            expect(pluginManagerPlugin->evaluators)->toHaveLength(1);
        }
        );
        it(std::string("should have valid action structure"), [=]() mutable
        {
            expect(startPluginConfigurationAction->name)->toBe(std::string("START_PLUGIN_CONFIGURATION"));
            expect(startPluginConfigurationAction->description)->toContain(std::string("configuration dialog"));
            expect(startPluginConfigurationAction->validate)->toBeTypeOf(std::string("function"));
            expect(startPluginConfigurationAction->handler)->toBeTypeOf(std::string("function"));
        }
        );
        it(std::string("should have valid provider structure"), [=]() mutable
        {
            expect(pluginConfigurationStatusProvider->name)->toBe(std::string("pluginConfigurationStatus"));
            expect(pluginConfigurationStatusProvider->description)->toContain(std::string("configuration status"));
            expect(pluginConfigurationStatusProvider->get)->toBeTypeOf(std::string("function"));
        }
        );
        it(std::string("should have valid evaluator structure"), [=]() mutable
        {
            expect(pluginConfigurationEvaluator->name)->toBe(std::string("pluginConfigurationEvaluator"));
            expect(pluginConfigurationEvaluator->description)->toContain(std::string("configuration needs"));
            expect(pluginConfigurationEvaluator->validate)->toBeTypeOf(std::string("function"));
            expect(pluginConfigurationEvaluator->handler)->toBeTypeOf(std::string("function"));
            expect(pluginConfigurationEvaluator->alwaysRun)->toBe(false);
        }
        );
    }
    );
}

MAIN
