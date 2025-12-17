#include "deployment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractDeployment getDeployment(DeploymentConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (config.type) {
            case 'docker':
            return new DockerDeployment(config);
            default:
            throw std::runtime_error(`Unknown deployment type: ${config.type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
