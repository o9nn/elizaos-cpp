#include "env-prompt.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> getEnvFilePath() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = getEnvFileService();
    return service.getFilePath();

}

std::future<std::unordered_map<std::string, std::string>> readEnvFile() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = getEnvFileService();
    return service.read();

}

std::future<void> writeEnvFile(const std::unordered_map<std::string, std::string>& envVars) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto service = getEnvFileService();
    service.write(envVars, {
        preserveComments: false,
        updateProcessEnv: true,
        });

}

} // namespace elizaos
