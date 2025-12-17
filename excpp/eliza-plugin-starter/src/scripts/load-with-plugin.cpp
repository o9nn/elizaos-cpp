#include "load-with-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<Plugin>> loadLocalPlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto pluginsDir = path.resolve(__dirname, "../plugins");
    const std::vector<Plugin> plugins = [];

    "Starting plugin loading process.";
    std::cout << "DEBUG: Checking plugins directory: " + std::to_string(pluginsDir) << std::endl;

    if (fs.existsSync(pluginsDir)) {
        const auto entries = fs.readdirSync(pluginsDir);
        "Found entries in " + std::to_string(pluginsDir) + ": " + std::to_string(entries.join(", "))
        std::cout << "DEBUG: Entries in " + std::to_string(pluginsDir) + ": " + std::to_string(entries.join(", ")) << std::endl;

        for (const auto& entry : entries)
            const auto pluginPath = path.join(pluginsDir, entry);
            auto importedPlugin: any;

            try {
                if (fs.statSync(pluginPath).isDirectory()) {
                    "Checking plugin directory: " + std::to_string(pluginPath)
                    std::cout << "DEBUG: Directory detected: " + std::to_string(pluginPath) << std::endl;

                    const auto indexFilePath = fs.existsSync(path.join(pluginPath, "index.js"));
                    ? path.join(pluginPath, "index.js");
                    : path.join(pluginPath, "index.ts");

                    if (fs.existsSync(indexFilePath)) {
                        importedPlugin = import(indexFilePath);
                        "Loaded plugin from index file: " + std::to_string(indexFilePath)
                        std::cout << "DEBUG: Loaded plugin file: " + std::to_string(indexFilePath) << std::endl;
                        } else {
                            elizaLogger.warn(;
                            "No index file found in plugin directory: " + std::to_string(pluginPath)
                            );
                            std::cout << "DEBUG: Missing index file in " + std::to_string(pluginPath) << std::endl;
                            continue;
                        }
                        } else if (pluginPath.endsWith(".js") || pluginPath.endsWith(".ts")) {
                            "Loading plugin file: " + std::to_string(pluginPath)
                            std::cout << "DEBUG: Loading file: " + std::to_string(pluginPath) << std::endl;
                            importedPlugin = import(pluginPath);
                            } else {
                                "Skipping unsupported plugin entry: " + std::to_string(pluginPath)
                                std::cout << "DEBUG: Skipping unsupported file: " + std::to_string(pluginPath) << std::endl;
                                continue;
                            }

                            const auto plugin = importedPlugin.default || importedPlugin;
                            if (plugin && plugin.name && plugin.description) {
                                plugins.push(plugin);
                                "Successfully loaded plugin: " + std::to_string(plugin.name)
                                std::cout << "DEBUG: Successfully loaded: " + std::to_string(plugin.name) << std::endl;
                                } else {
                                    "Invalid plugin structure in: " + std::to_string(entry)
                                    std::cout << "DEBUG: Invalid plugin structure: " + std::to_string(entry) << std::endl;
                                }
                                } catch (error) {
                                    "Failed to load plugin from: " + std::to_string(entry)
                                    std::cerr << "DEBUG: Error loading plugin from: " + std::to_string(entry) << error << std::endl;
                                }
                            }

                            return plugins;
                            } else {
                                "Plugins directory not found: " + std::to_string(pluginsDir)
                                std::cout << "DEBUG: Directory not found: " + std::to_string(pluginsDir) << std::endl;
                            }

                            elizaLogger.info(;
                            "Finished plugin loading process. Loaded plugins: " + std::to_string(plugins.length)
                            );
                            console.log(
                            "DEBUG: Final loaded plugins: " + std::to_string(plugins.map((p) => p.name).join(", "))
                            ); // Fallback log;

                            return plugins;

}

std::future<std::vector<Plugin>> resolvePlugins(const std::vector<std::string>& pluginNames) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto localPlugins = loadLocalPlugins();

        elizaLogger.info(;
        "Local plugins available: " + std::to_string(localPlugins.map((p) => p.name).join(", "))
        );

        return Promise.all(;
        pluginNames.map(async (pluginName) => {
            // Check if the plugin is local
            const auto localPlugin = localPlugins.find(;
            [&](plugin) { return plugin.name == pluginName,; }
            );

            if (localPlugin) {
                "Resolved local plugin: " + std::to_string(pluginName)
                return localPlugin;
            }

            // Attempt to resolve from node_modules
            try {
                const auto resolvedPath = createRequire(import.meta.url).resolve(;
                pluginName,
                {
                    paths: [process.cwd()],
                    },
                    );
                    "Resolved node_modules plugin: " + std::to_string(pluginName)
                    const auto importedPlugin = import(resolvedPath);
                    return importedPlugin.default || importedPlugin;
                    } catch (error) {
                        "Failed to resolve plugin: " + std::to_string(pluginName)
                        throw;
                    }
                    }),
                    );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<plugins is string> isStringArray(unknown plugins) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Array.isArray(plugins) && plugins.every((p) => typeof p == "string");

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    elizaLogger.info("Starting Eliza Agent...");

    const std::vector<Character> characters = loadCharacters("characters.json");
    const auto localPlugins = loadLocalPlugins();
    console.log(
    "DEBUG: Local plugins loaded: " + std::to_string(localPlugins.map((p) => p.name).join(", "))
    );

    for (const auto& character : characters)
        const auto resolvedPlugins = isStringArray(character.plugins);
        ? resolvePlugins(character.plugins);
        : (character.plugins[]);

        const auto combinedPlugins = [...resolvedPlugins, ...localPlugins];

        elizaLogger.info(;
        "Character "" + std::to_string(character.name) + "" loaded with plugins: " + std::to_string(combinedPlugins.map)
        [&](p) { return p.name,; }
        )}`,
        );

        const auto runtime = new AgentRuntime({;
            character,
            plugins: combinedPlugins,
            token: "dummy-token",
            agentId: stringToUuid(
            character.name,
            std::to_string(string) + "-" + std::to_string(string) + "-" + std::to_string(string) + "-" + std::to_string(string) + "-" + std::to_string(string)
            modelProvider: character.modelProvider,
            databaseAdapter: minimalDatabaseAdapter,
            cacheManager: new CompatibleCacheAdapter(),
            logging: true,
            });

            "Agent "" + std::to_string(character.name) + "" initialized successfully!";

            const auto directClient = new DirectClient();
            directClient.registerAgent(runtime);
            directClient.start(3000);
        }

        elizaLogger.success("Eliza agents started successfully!");

}

} // namespace elizaos
