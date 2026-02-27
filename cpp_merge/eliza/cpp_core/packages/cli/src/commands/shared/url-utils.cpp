#include "url-utils.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: getAgentRuntimeUrl(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    opts.remoteUrl.replace(/\/$/, "") || // Use the flag if provided;
    std::getenv("AGENT_RUNTIME_URL").replace(/\/$/, "") || // Fallback to env var;
    "http://localhost:" + std::to_string(opts.port || std::getenv("SERVER_PORT") || "3000")
    );

}

std: getAgentsBaseUrl(OptionValues opts) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::to_string(getAgentRuntimeUrl(opts)) + "/api/agents";

}

} // namespace elizaos
