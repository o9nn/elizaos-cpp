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
    std::optional<std::vector<string | string>> bio;
    std::optional<std::string> characterName;
    'active' | 'inactive' | 'stopped' status;
    std::optional<bool> enabled;
    std::optional<Date> createdAt;
    std::optional<Date> updatedAt;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct AgentCreateParams {
    std::optional<std::string> characterPath;
    std::optional<std::unordered_map<std::string, std::any>> characterJson;
    std::optional<std::unordered_map<std::string, std::any>> agent;
};

struct AgentUpdateParams {
    std::optional<std::string> name;
    std::optional<std::vector<string | string>> bio;
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
    std::optional<UUID> id;
    std::optional<std::string> type;
    std::optional<double> timestamp;
    std::optional<std::string> message;
    std::optional<std::string> details;
    std::optional<UUID> roomId;
    std::optional<{> body;
    std::optional<std::string> modelType;
    std::optional<std::string> modelKey;
    std::optional<std::any> params;
    std::optional<std::any> response;
    std::optional<{> usage;
    std::optional<double> prompt_tokens;
    std::optional<double> completion_tokens;
    std::optional<double> total_tokens;
    std::optional<double> createdAt;
};

interface AgentLogsParams extends PaginationParams {
  level?: 'debug' | 'info' | 'warn' | 'error';
  from?: Date | string;
  to?: Date | string;
  search?: string;
}

} // namespace elizaos
