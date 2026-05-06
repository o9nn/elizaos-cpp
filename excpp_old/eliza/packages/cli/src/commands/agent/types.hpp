#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface representing the payload sent when starting an agent.
 */
struct AgentStartPayload {
    std::optional<std::string> characterPath;
};


} // namespace elizaos
