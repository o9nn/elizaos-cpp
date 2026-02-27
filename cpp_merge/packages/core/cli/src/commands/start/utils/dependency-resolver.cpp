#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/start/utils/dependency-resolver.h"

array<std::shared_ptr<Plugin>> resolvePluginDependencies(std::shared_ptr<Map<string, std::shared_ptr<Plugin>>> availablePlugins, boolean isTestMode)
{
    shared resolutionOrder = array<string>();
    shared visited = std::make_shared<Set<string>>();
    shared visiting = std::make_shared<Set<string>>();
    auto visit = [=](auto pluginName) mutable
    {
        if (!availablePlugins->has(pluginName)) {
            logger->warn(std::string("Plugin dependency "") + pluginName + std::string("" not found and will be skipped."));
            return;
        }
        if (visited->has(pluginName)) return;
        if (visiting->has(pluginName)) {
            logger->error(std::string("Circular dependency detected involving plugin: ") + pluginName + string_empty);
            return;
        }
        visiting->add(pluginName);
        auto plugin = availablePlugins->get(pluginName);
        if (plugin) {
            auto deps = array<any>{ (OR((plugin->dependencies), (array<any>()))) };
            if (isTestMode) {
                deps->push(const_((OR((plugin->testDependencies), (array<any>()))))[0]);
            }
            for (auto& dep : deps)
            {
                visit(dep);
            }
        }
        visiting->delete(pluginName);
        visited->add(pluginName);
        resolutionOrder->push(pluginName);
    };

    for (auto& name : availablePlugins->keys())
    {
        if (!visited->has(name)) {
            visit(name);
        }
    }
    auto finalPlugins = as<array<std::shared_ptr<Plugin>>>(resolutionOrder->map([=](auto name) mutable
    {
        return availablePlugins->get(name);
    }
    )->filter([=](auto p) mutable
    {
        return p;
    }
    ));
    logger->info(std::string("Final plugins being loaded: ") + finalPlugins->map([=](auto p) mutable
    {
        return p->name;
    }
    )->join(std::string(", ")) + string_empty);
    return finalPlugins;
};


