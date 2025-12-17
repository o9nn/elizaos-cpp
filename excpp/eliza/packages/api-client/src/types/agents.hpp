#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "base.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Agent {
    UUID id;
    std::string name;
    std::optional<std::string> description;
    std::string status;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
};

struct AgentCreateParams {
    std::string name;
    std::optional<std::string> description;
};

struct AgentUpdateParams {
    std::optional<std::string> name;
    std::optional<std::string> description;
};

struct AgentWorld {
    UUID id;
    std::string name;
    std::optional<std::string> description;
    std::optional<std::vector<Agent>> agents;
};

struct AgentWorldSettings {
    UUID worldId;
};

struct AgentPanel {
    std::string id;
    std::string name;
    std::string url;
    std::string type;
};

struct AgentLog {
    UUID id;
    UUID agentId;
    std::string level;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
};


} // namespace elizaos
