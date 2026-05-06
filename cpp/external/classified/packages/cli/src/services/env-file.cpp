#include "env-file.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<EnvFileService> getEnvFileService() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!globalInstance) {
        globalInstance = new EnvFileService();
        globalInstance.initialize();
    }
    return globalInstance;

}

EnvFileService createEnvFileService(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new EnvFileService(filePath);

}

} // namespace elizaos
