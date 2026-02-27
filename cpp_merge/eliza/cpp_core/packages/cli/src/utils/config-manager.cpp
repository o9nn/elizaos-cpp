#include "config-manager.hpp"
#include <future>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std:> getConfigFilePath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.paths.configPath;

}

std::future<bool> fileExists(const std:& p) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        fs.access(p);
        return true;
        } catch {
            return false;
        }

}

std::future<AgentConfig> loadConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto configPath = getConfigFilePath();
        if (!(fileExists(configPath))) {
            return {
                lastUpdated: std::make_unique<Date>().toISOString(),
                isDefault: true, // Mark config
                };
            }

            const auto content = fs.readFile(configPath, "utf8");
            return /* JSON::parse */ content;
            } catch (error) {
                std::cout << "Error loading configuration: " + error << std::endl;
                // Return default configuration on error
                return {
                    lastUpdated: std::make_unique<Date>().toISOString(),
                    isDefault: true, // Mark config
                    };
                }

}

std::future<void> saveConfig(AgentConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto configPath = getConfigFilePath();
        const auto elizaDir = path.dirname(configPath);

        // Create .eliza directory if it doesn't exist
        if (!(fileExists(elizaDir))) {
            fs.mkdir(elizaDir, Config{recursive = true});
        }

        // Update lastUpdated timestamp
        config.lastUpdated = std::make_unique<Date>().toISOString();

        // Write config to file
        fs.writeFile(configPath, /* JSON.stringify */ std:(config, nullptr, 2), "utf8");
        std::cout << "Configuration saved to " + configPath << std::endl;
        } catch (error) {
            std::cerr << "Error saving configuration: " + error << std::endl;
        }

}

std::future<> checkPluginRequirements(const std:& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    valid;
    message: std:;

}

std::future<std::unordered_map<std:, bool>> getPluginStatus() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto configPath = getConfigFilePath();
    if (!(fileExists(configPath))) {
        return {}
    }

    try {
        const auto configContent = fs.readFile(configPath, "utf-8");
        const auto config = /* JSON::parse */ configContent;
        const std::unordered_map<std:, bool> status = {};

        // Check each plugin's environment variables
        for (const auto& plugin : Object.keys(config.plugins || {})
            const auto check = validatePluginEnvVars(plugin);
            status[plugin] = check.valid;
        }

        return status;
        } catch (error) {
            std::cerr << "Error reading config file: " + error << std::endl;
            return {}
        }

}

} // namespace elizaos
