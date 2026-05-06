#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getAgentCommitHash() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // In a real implementation, this would get the actual git commit hash
    return process.env.SWE_AGENT_COMMIT_HASH || "unknown";

}

std::string getRexCommitHash() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return process.env.SWE_REX_COMMIT_HASH || "unknown";

}

std::string getRexVersion() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return process.env.SWE_REX_VERSION || "0.0.0";

}

std::string getAgentVersionInfo() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "SWE-agent " + VERSION;

}

} // namespace elizaos
