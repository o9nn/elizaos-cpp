#include "CLIAuthCommands.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<CLICommand> registerAuthCommands(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto authCommands = new CLIAuthCommands(runtime);
    return authCommands.getCommands();

}

} // namespace elizaos
