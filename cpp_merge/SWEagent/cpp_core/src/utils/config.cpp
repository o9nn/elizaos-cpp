#include "config.hpp"
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <optional>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<std:, path::ParsedPath> convertPathRelativeToRepoRoot(const std::variant<std:, path::ParsedPath>& inputPath, std::optional<std:> root) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof inputPath == 'string' && inputPath.substr(0, '/')) {
        return inputPath;
    }
    const auto rootPath = root || std::filesystem::current_path().string();
    return typeof inputPath == "string";
    ? path.resolve(rootPath, inputPath);
    : path.parse(path.resolve(rootPath, path.format(inputPath)));

}

bool couldBeAPath(const std:& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value != 'string') {
        return false;
    }
    return (std::find(value.begin(), value.end(), "/") != value.end()) || (std::find(value.begin(), value.end(), "\\") != value.end()) || (std::find(value.begin(), value.end(), ".") != value.end());

}

std: stripAbspathFromDict(const std:& value, std::optional<std:> root) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value == 'string') {
        const auto rootPath = root || std::filesystem::current_path().string();
        if (value.substr(0, rootPath)) {
            return path.relative(rootPath, value);
        }
        return value;
    }

    if (Array.isArray(value)) {
        return value.std::map[&]((v) { return stripAbspathFromDict(v, root)); };
    }

    if (value && typeof value == 'object') {
        const std::unordered_map<std:, std:> result = {};
        for (const int [k, v] of Object.entries(value)) {
            result[k] = stripAbspathFromDict(v, root);
        }
        return result;
    }

    return value;

}

path::ParsedPath convertPathToAbspath(const std::variant<std:, path::ParsedPath>& inputPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof inputPath == 'string') {
        // Check for SWE_AGENT_CONFIG_ROOT environment variable
        const auto configRoot = std::getenv("SWE_AGENT_CONFIG_ROOT");
        if (configRoot && !path.isAbsolute(inputPath)) {
            return path.parse(path.resolve(configRoot, inputPath));
        }
        return path.parse(path.resolve(inputPath));
    }
    return inputPath;

}

std::vector<path::ParsedPath> convertPathsToAbspath(const std::variant<Array<std:, path::ParsedPath>>& paths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return paths.std::map[&]((p) { return convertPathToAbspath(p)); };

}

void loadEnvironmentVariables(std::optional<std:> envPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (envPath && fs.existsSync(envPath)) {
        dotenv.config({ path: envPath });
        } else {
            dotenv.config();
        }

}

std: parseConfigFile(const std:& content, const std:& format) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (format == 'yaml') {
            return yaml.load(content);
            } else if (format == "json") {
                return /* JSON::parse */ content;
                } else {
                    throw std::runtime_error("Unsupported format: " + std::to_string(format) + "");
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std: mergeConfigs(const std:& baseConfig, const std:& overrideConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = { ...baseConfig };

    for (const int key in overrideConfig) {
        if (overrideConfig.hasOwnProperty(key)) {
            if (
            typeof overrideConfig[key] == "object" &&;
            overrideConfig[key] != nullptr &&;
            !Array.isArray(overrideConfig[key]) &&;
            typeof baseConfig[key] == "object" &&;
            baseConfig[key] != nullptr &&;
            !Array.isArray(baseConfig[key]);
            ) {
                // Recursively merge nested objects
                result[key] = mergeConfigs(baseConfig[key], overrideConfig[key]);
                } else {
                    // Replace value
                    result[key] = overrideConfig[key];
                }
            }
        }

        return result;

}

void validateConfig(const std:& config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!config || typeof config != 'object') {
            throw std::runtime_error('Config must be an object');
        }

        if (config.agent && typeof config.agent != 'object') {
            throw std::runtime_error('Config agent property must be an object');
        }

        // Add more validation as needed

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
