#include "base.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Agent {
    UUID id;
    std: name;
    std::optional<std:> description;
    'active' | 'inactive' | 'stopped' status;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct AgentCreateParams {
    std: name;
    std::optional<std:> description;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct AgentUpdateParams {
    std::optional<std:> name;
    std::optional<std:> description;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct AgentWorld {
    UUID id;
    std: name;
    std::optional<std:> description;
    std::optional<std::vector<Agent>> agents;
};

struct AgentWorldSettings {
    UUID worldId;
    std::unordered_map<std:, std:> settings;
};

struct AgentPanel {
    std: id;
    std: name;
    std: url;
    std: type;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct AgentLog {
    UUID id;
    UUID agentId;
    'debug' | 'info' | 'warn' | 'error' level;
    std: message;
    Date timestamp;
    std::optional<std::unordered_map<std:, std:>> metadata;
};


} // namespace elizaos
