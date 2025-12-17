#include "file-operations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getGlobalEnvPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = getEnvFileService();
    return service.getFilePath();

}

std::future<std::optional<std::string>> getLocalEnvPath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto localEnvPath = path.join(process.cwd(), '.env');
    return existsSync(localEnvPath) ? localEnvPath : nullptr;

}

std::future<EnvVars> parseEnvFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = createEnvFileService(filePath);
    return service.read();

}

std::future<void> writeEnvFile(const std::string& filePath, EnvVars envVars) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = createEnvFileService(filePath);
    service.write(envVars, {
        preserveComments: true,
        updateProcessEnv: true,
        });

}

std::future<bool> resetEnvFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!existsSync(filePath)) {
            return false;
        }

        const auto service = createEnvFileService(filePath);
        const auto envVars = service.read();

        if (Object.keys(envVars).length == 0) {
            return false; // No variables to reset;
        }

        const auto resetVars = Object.keys(envVars).reduce((acc, key) => {;
            acc[key] = '';
            return acc;
            }, {});

            service.write(resetVars, {
                preserveComments: true,
                updateProcessEnv: false, // Don't update process.env with empty values
                });

                return true;
                } catch (error) {
                    console.error(
                    "Error resetting environment file: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                    );
                    return false;
                }

}

} // namespace elizaos
