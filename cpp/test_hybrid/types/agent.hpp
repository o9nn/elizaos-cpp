#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// ElizaOS Agent Type Definitions
    struct AgentConfig {
    std::string agentId;
    std::string agentName;
    std::optional<std::string> bio;
    std::optional<std::vector<std::string>> lore;
};

    struct Memory {
    std::string id;
    std::string content;
    std::string entityId;
    std::string agentId;
    double timestamp;
};

    class AgentMemoryManager {
public:
};

    class AgentLogger {
public:
};


} // namespace elizaos
