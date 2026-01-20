#include "config-manager.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getConfigFilePath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.paths.configPath;

}

std::future<bool> fileExists(const std::string& p) {
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
        if (!(await fileExists(configPath))) {
            return {
                lastUpdated: new Date().toISOString(),
                isDefault: true, // Mark config
                };
            }

            const auto content = fs.readFile(configPath, "utf8");
            return /* JSON.parse */ content;
            } catch (error) {
                std::cout << "Error loading configuration: " + error << std::endl;
                // Return default configuration on error
                return {
                    lastUpdated: new Date().toISOString(),
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
        if (!(await fileExists(elizaDir))) {
            fs.mkdir(elizaDir, { recursive: true });
        }

        // Update lastUpdated timestamp
        config.lastUpdated = new Date().toISOString();

        // Write config to file
        fs.writeFile(configPath, /* JSON.stringify */ std::string(config, nullptr, 2), "utf8");
        std::cout << "Configuration saved to " + configPath << std::endl;
        } catch (error) {
            std::cerr << "Error saving configuration: " + error << std::endl;
        }

}

std::future<> checkPluginRequirements(const std::string& pluginName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    valid: boolean;
    message: string;

}

std::future<std::unordered_map<std::string, bool>> getPluginStatus() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto configPath = getConfigFilePath();
    if (!(await fileExists(configPath))) {
        return {}
    }

    try {
        const auto configContent = fs.readFile(configPath, "utf-8");
        const auto config = /* JSON.parse */ configContent;
        const std::unordered_map<std::string, bool> status = {};

        // Check each plugin's environment variables
        for (const auto& plugin : Object.keys(config.plugins ?? {})
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
