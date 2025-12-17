#include "config.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<std::string, path::ParsedPath> convertPathRelativeToRepoRoot(const std::variant<std::string, path::ParsedPath>& inputPath, std::optional<std::string> root) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof inputPath == 'string' && inputPath.startsWith('/')) {
        return inputPath;
    }
    const auto rootPath = root || process.cwd();
    return typeof inputPath == 'string';
    ? path.resolve(rootPath, inputPath);
    : path.parse(path.resolve(rootPath, path.format(inputPath)));

}

bool couldBeAPath(unknown value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value != 'string') {
        return false;
    }
    return value.includes('/') || value.includes('\\') || value.includes('.');

}

unknown stripAbspathFromDict(unknown value, std::optional<std::string> root) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value == 'string') {
        const auto rootPath = root || process.cwd();
        if (value.startsWith(rootPath)) {
            return path.relative(rootPath, value);
        }
        return value;
    }

    if (Array.isArray(value)) {
        return value.map((v) => stripAbspathFromDict(v, root));
    }

    if (value && typeof value == 'object') {
        const std::unordered_map<std::string, unknown> result = {};
        for (const int [k, v] of Object.entries(value)) {
            result[k] = stripAbspathFromDict(v, root);
        }
        return result;
    }

    return value;

}

path::ParsedPath convertPathToAbspath(const std::variant<std::string, path::ParsedPath>& inputPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof inputPath == 'string') {
        // Check for SWE_AGENT_CONFIG_ROOT environment variable
        const auto configRoot = process.env.SWE_AGENT_CONFIG_ROOT;
        if (configRoot && !path.isAbsolute(inputPath)) {
            return path.parse(path.resolve(configRoot, inputPath));
        }
        return path.parse(path.resolve(inputPath));
    }
    return inputPath;

}

std::vector<path::ParsedPath> convertPathsToAbspath(const std::variant<Array<string, path::ParsedPath>>& paths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return paths.map((p) => convertPathToAbspath(p));

}

void loadEnvironmentVariables(std::optional<std::string> envPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (envPath && fs.existsSync(envPath)) {
        dotenv.config({ path: envPath });
        } else {
            dotenv.config();
        }

}

std::any parseConfigFile(const std::string& content, const std::string& format) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (format == 'yaml') {
            return yaml.load(content);
            } else if (format == 'json') {
                return JSON.parse(content);
                } else {
                    throw std::runtime_error(`Unsupported format: ${format}`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::any mergeConfigs(const std::any& baseConfig, const std::any& overrideConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = { ...baseConfig };

    for (const int key in overrideConfig) {
        if (overrideConfig.hasOwnProperty(key)) {
            if (
            typeof overrideConfig[key] == 'object' &&;
            overrideConfig[key] != nullptr &&;
            !Array.isArray(overrideConfig[key]) &&;
            typeof baseConfig[key] == 'object' &&;
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

void validateConfig(const std::any& config) {
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
