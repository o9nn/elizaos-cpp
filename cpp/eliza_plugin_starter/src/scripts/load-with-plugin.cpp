#include "load-with-plugin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<Plugin>> loadLocalPlugins() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto pluginsDir = path.resolve(__dirname, "../plugins");
    const std::vector<Plugin> plugins = [];

    "elizaLogger.info(" + "Starting plugin loading process.";
    std::cout << "DEBUG: Checking plugins directory: " + pluginsDir << std::endl;

    if (fs.existsSync(pluginsDir)) {
        const auto entries = fs.readdirSync(pluginsDir);
        "elizaLogger.info(" + "Found entries in " + pluginsDir + ": " + std::to_string(entries.join(", "))
        std::cout << "DEBUG: Entries in " + pluginsDir + ": " + std::to_string(entries.join(", ")) << std::endl;

        for (const auto& entry : entries)
            const auto pluginPath = path.join(pluginsDir, entry);
            auto importedPlugin: any;

            try {
                if (fs.statSync(pluginPath).isDirectory()) {
                    "elizaLogger.info(" + "Checking plugin directory: " + pluginPath
                    std::cout << "DEBUG: Directory detected: " + pluginPath << std::endl;

                    const auto indexFilePath = fs.existsSync(path.join(pluginPath, "index.js"));
                    ? path.join(pluginPath, "index.js");
                    : path.join(pluginPath, "index.ts");

                    if (fs.existsSync(indexFilePath)) {
                        importedPlugin = import(indexFilePath);
                        "elizaLogger.info(" + "Loaded plugin from index file: " + indexFilePath
                        std::cout << "DEBUG: Loaded plugin file: " + indexFilePath << std::endl;
                        } else {
                            elizaLogger.warn(;
                            "No index file found in plugin directory: " + pluginPath
                            );
                            std::cout << "DEBUG: Missing index file in " + pluginPath << std::endl;
                            continue;
                        }
                        } else if (pluginPath.endsWith(".js") || pluginPath.endsWith(".ts")) {
                            "elizaLogger.info(" + "Loading plugin file: " + pluginPath
                            std::cout << "DEBUG: Loading file: " + pluginPath << std::endl;
                            importedPlugin = import(pluginPath);
                            } else {
                                "elizaLogger.warn(" + "Skipping unsupported plugin entry: " + pluginPath
                                std::cout << "DEBUG: Skipping unsupported file: " + pluginPath << std::endl;
                                continue;
                            }

                            const auto plugin = importedPlugin.default || importedPlugin;
                            if (plugin && plugin.name && plugin.description) {
                                plugins.push_back(plugin);
                                "elizaLogger.info(" + "Successfully loaded plugin: " + plugin.name
                                std::cout << "DEBUG: Successfully loaded: " + plugin.name << std::endl;
                                } else {
                                    "elizaLogger.warn(" + "Invalid plugin structure in: " + entry
                                    std::cout << "DEBUG: Invalid plugin structure: " + entry << std::endl;
                                }
                                } catch (error) {
                                    "elizaLogger.error(" + "Failed to load plugin from: " + entry
                                    std::cerr << "DEBUG: Error loading plugin from: " + entry << error << std::endl;
                                }
                            }

                            return plugins;
                            } else {
                                "elizaLogger.warn(" + "Plugins directory not found: " + pluginsDir
                                std::cout << "DEBUG: Directory not found: " + pluginsDir << std::endl;
                            }

                            elizaLogger.info(;
                            "Finished plugin loading process. Loaded plugins: " + plugins.size()
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
                "elizaLogger.info(" + "Resolved local plugin: " + pluginName
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
                    "elizaLogger.info(" + "Resolved node_modules plugin: " + pluginName
                    const auto importedPlugin = import(resolvedPath);
                    return importedPlugin.default || importedPlugin;
                    } catch (error) {
                        "elizaLogger.error(" + "Failed to resolve plugin: " + pluginName
                        throw;
                    }
                    }),
                    );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<plugins is string> isStringArray(const std::any& plugins) {
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
        "Character \"" + character.name + "\" loaded with plugins: " + combinedPlugins.map
        [&](p) { return p.name,; }
        ")}"
        );

        const auto runtime = new AgentRuntime({;
            character,
            plugins: combinedPlugins,
            token: "dummy-token",
            agentId: stringToUuid(
            character.name,
            ") as " + string + "-" + string + "-" + string + "-" + string + "-" + string
            modelProvider: character.modelProvider,
            databaseAdapter: minimalDatabaseAdapter,
            cacheManager: new CompatibleCacheAdapter(),
            logging: true,
            });

            "elizaLogger.success(" + "Agent \"" + character.name + "\" initialized successfully!";

            const auto directClient = new DirectClient();
            directClient.registerAgent(runtime);
            directClient.start(3000);
        }

        elizaLogger.success("Eliza agents started successfully!");

}

} // namespace elizaos
