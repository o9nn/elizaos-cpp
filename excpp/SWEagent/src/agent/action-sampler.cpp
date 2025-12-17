#include "action-sampler.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractActionSampler createActionSampler(ActionSamplerConfig config, AbstractModel model, ToolHandler tools) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (config.type) {
            case 'ask_colleagues':
            return AskColleagues.fromConfig(config, model, tools);
            case 'binary_trajectory_comparison':
            return BinaryTrajectoryComparison.fromConfig(config, model, tools);
            default:
            throw std::runtime_error(`Unknown action sampler type: ${(config as ActionSamplerConfig).type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
