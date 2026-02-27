#include "index.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: getAgentCommitHash() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // In a real implementation, this would get the actual git commit hash
    return std::getenv("SWE_AGENT_COMMIT_HASH") || "unknown";

}

std: getRexCommitHash() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::getenv("SWE_REX_COMMIT_HASH") || "unknown";

}

std: getRexVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return std::getenv("SWE_REX_VERSION") || "0.0.0";

}

std: getAgentVersionInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "SWE-agent " + VERSION;

}

} // namespace elizaos
