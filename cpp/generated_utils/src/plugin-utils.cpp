#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/test/utils/plugin-utils.h"

std::shared_ptr<Promise<array<std::shared_ptr<Plugin>>>> loadPluginDependencies(std::shared_ptr<DirectoryInfo> projectInfo)
{
    if (projectInfo->type != std::string("elizaos-plugin")) {
        return array<any>();
    }
    auto project = std::async([=]() { loadProject(process->cwd()); });
    auto dependencyPlugins = array<std::shared_ptr<Plugin>>();
    if (AND((AND((project->isPlugin), (project->pluginModule->dependencies))), (project->pluginModule->dependencies->length > 0))) {
        auto projectPluginsPath = path->join(process->cwd(), std::string(".eliza"), std::string("plugins"));
        for (auto& dependency : project->pluginModule->dependencies)
        {
            auto pluginPath = path->join(projectPluginsPath, std::string("node_modules"), dependency);
            if (fs->existsSync(pluginPath)) {
                try
                {
                    auto pluginProject = std::async([=]() { loadProject(pluginPath); });
                    if (pluginProject->pluginModule) {
                        dependencyPlugins->push(pluginProject->pluginModule);
                    }
                }
                catch (const any& error)
                {
                    logger->error(std::string("Failed to load or build dependency ") + dependency + std::string(":"), error);
                }
            }
        }
    }
    return dependencyPlugins;
};


