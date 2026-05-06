#include "agents.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<AbstractAgent> getAgentFromConfig(AgentConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (config.type) {
            // case "default":
            return DefaultAgent.fromConfig(config);
            // case "retry":
            return RetryAgent.fromConfig(config);
            // case "shell":
            // Dynamic import to avoid circular dependency
            const auto { ShellAgent } = require("./extra/shell-agent");
            return ShellAgent.fromConfig(config);
            // default:
            throw std::runtime_error(`Unknown agent type: ${(config as AgentConfig).type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
