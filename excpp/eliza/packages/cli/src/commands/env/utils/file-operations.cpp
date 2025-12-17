#include "file-operations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getGlobalEnvPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    return envInfo.paths.envFilePath;

}

std::future<std::optional<std::string>> getLocalEnvPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto localEnvPath = path.join(process.cwd(), '.env');
    return existsSync(localEnvPath) ? localEnvPath : nullptr;

}

std::future<EnvVars> parseEnvFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!existsSync(filePath)) {
            return {}
        }

        const auto content = fs.readFile(filePath, 'utf-8');
        // Handle empty file case gracefully
        if (content.trim() == '') {
            return {}
        }
        return dotenv.parse(content);
        } catch (error) {
            console.error(
            "Error parsing .env file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
            return {}
        }

}

std::future<void> writeEnvFile(const std::string& filePath, EnvVars envVars) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto dir = path.dirname(filePath);
        if (!existsSync(dir)) {
            fs.mkdir(dir, { recursive: true });
        }

        const auto content = Object.entries(envVars);
        std::to_string(key) + "=" + std::to_string(value);
        .join('\n');

        fs.writeFile(filePath, content);
        } catch (error) {
            console.error(
            "Error writing .env file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
            );
        }

}

std::future<bool> resetEnvFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!existsSync(filePath)) {
            return false;
        }

        const auto envVars = parseEnvFile(filePath);
        if (Object.keys(envVars).length == 0) {
            return false; // No variables to reset;
        }

        const auto resetVars = Object.keys(envVars).reduce((acc, key) => {;
            acc[key] = '';
            return acc;
            }, {});

            writeEnvFile(filePath, resetVars);
            return true;
            } catch (error) {
                console.error(
                "Error resetting environment file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                );
                return false;
            }

}

} // namespace elizaos
