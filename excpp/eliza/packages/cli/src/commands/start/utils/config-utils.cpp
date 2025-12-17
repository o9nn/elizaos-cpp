#include "config-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<RuntimeSettings> loadEnvConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    if (envInfo.paths.envFilePath) {
        dotenv.config({ path: envInfo.paths.envFilePath });
    }
    return process.env;

}

} // namespace elizaos
