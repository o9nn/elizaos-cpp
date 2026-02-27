#include "agent-start.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> stopAgent(IAgentRuntime runtime, AgentServer server) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    runtime.close();
    server.unregisterAgent(runtime.agentId);
    logger.success("Agent " + std::to_string(runtime.character.name) + " stopped successfully!");

}

} // namespace elizaos
