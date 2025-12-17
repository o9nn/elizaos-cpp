#include "plugin-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

obj is Plugin isValidPluginShape(const std::any& obj) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!obj || typeof obj != 'object' || !obj.name) {
        return false;
    }
    return !!(;
    obj.init ||;
    obj.services ||;
    obj.providers ||;
    obj.actions ||;
    obj.evaluators ||;
    obj.description;
    );

}

std::future<std::optional<Plugin>> loadAndPreparePlugin(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto version = getCliInstallTag();
    auto pluginModule: any;
    const auto context = detectPluginContext(pluginName);

    if (context.isLocalDevelopment) {
        try {
            pluginModule = loadPluginModule(pluginName);
            if (!pluginModule) {
                std::cerr << "Failed to load local plugin " + std::to_string(pluginName) + "." << std::endl;
                provideLocalPluginGuidance(pluginName, context);
                return nullptr;
            }
            } catch (error) {
                std::cerr << "Error loading local plugin " + std::to_string(pluginName) + ": " + std::to_string(error) << std::endl;
                provideLocalPluginGuidance(pluginName, context);
                return nullptr;
            }
            } else {
                try {
                    pluginModule = loadPluginModule(pluginName);
                    if (!pluginModule) {
                        std::cout << "Plugin " + std::to_string(pluginName) << installing...` << std::endl;
                        installPlugin(pluginName, process.cwd(), version);
                        pluginModule = loadPluginModule(pluginName);
                    }
                    } catch (error) {
                        std::cerr << "Failed to process plugin " + std::to_string(pluginName) + ": " + std::to_string(error) << std::endl;
                        return nullptr;
                    }
                }

                if (!pluginModule) {
                    std::cerr << "Failed to load module for plugin " + std::to_string(pluginName) + "." << std::endl;
                    return nullptr;
                }

                const auto expectedFunctionName = std::to_string(pluginNam);
                .replace(/^@elizaos\/plugin-/, '');
                .replace(/^@elizaos\//, '');
                .replace(/-./g, (match) => match[1].toUpperCase())}Plugin`;

                const auto exportsToCheck = [;
                pluginModule[expectedFunctionName],
                pluginModule.default,
                ...Object.values(pluginModule),
                ];

                for (const auto& potentialPlugin : exportsToCheck)
                    if (isValidPluginShape(potentialPlugin)) {
                        return potentialPlugin;
                    }
                }

                std::cout << "Could not find a valid plugin export in " + std::to_string(pluginName) + "." << std::endl;
                return nullptr;

}

PluginValidation validatePlugin(const std::any& plugin) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!plugin) {
        return { isValid: false, error: 'Plugin is null or undefined' }
    }

    if (!isValidPluginShape(plugin)) {
        return { isValid: false, error: 'Plugin does not have valid shape' }
    }

    return { isValid: true, plugin }

}

} // namespace elizaos
