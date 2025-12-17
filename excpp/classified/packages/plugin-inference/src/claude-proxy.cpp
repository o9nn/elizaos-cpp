#include "claude-proxy.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createClaudeHandler(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto proxy = new ClaudeProxy(runtime);

        return async (request: ClaudeMessagesRequest): Promise<ClaudeResponse> => {;
            try {
                return proxy.processRequest(request);
                } catch (error) {
                    std::cerr << "[CLAUDE_PROXY] Error processing request:" << error << std::endl;
                    throw;
                }
                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
