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
    std::string name;
    std::optional<std::string> description;
    'active' | 'inactive' | 'stopped' status;
    Date createdAt;
    Date updatedAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct AgentCreateParams {
    std::string name;
    std::optional<std::string> description;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct AgentUpdateParams {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct AgentWorld {
    UUID id;
    std::string name;
    std::optional<std::string> description;
    std::optional<std::vector<Agent>> agents;
};

struct AgentWorldSettings {
    UUID worldId;
    std::unordered_map<std::string, std::any> settings;
};

struct AgentPanel {
    std::string id;
    std::string name;
    std::string url;
    std::string type;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct AgentLog {
    UUID id;
    UUID agentId;
    'debug' | 'info' | 'warn' | 'error' level;
    std::string message;
    Date timestamp;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


} // namespace elizaos
