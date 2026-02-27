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
    std::optional<std::vector<std: | string>> bio;
    std::optional<std:> characterName;
    'active' | 'inactive' | 'stopped' status;
    std::optional<bool> enabled;
    std::optional<Date> createdAt;
    std::optional<Date> updatedAt;
    std::optional<std::unordered_map<std:, std:>> metadata;
};

struct AgentCreateParams {
    std::optional<std:> characterPath;
    std::optional<std::unordered_map<std:, std:>> characterJson;
    std::optional<std::unordered_map<std:, std:>> agent;
};

struct AgentUpdateParams {
    std::optional<std:> name;
    std::optional<std::vector<std: | string>> bio;
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
    std::optional<UUID> id;
    std::optional<std:> type;
    std::optional<double> timestamp;
    std::optional<std:> message;
    std::optional<std:> details;
    std::optional<UUID> roomId;
    std::optional<{> body;
    std::optional<std:> modelType;
    std::optional<std:> modelKey;
    std::optional<std:> params;
    std::optional<std:> response;
    std::optional<{> usage;
    std::optional<double> prompt_tokens;
    std::optional<double> completion_tokens;
    std::optional<double> total_tokens;
    std::optional<double> createdAt;
};

interface AgentLogsParams extends PaginationParams {
  level?: 'debug' | 'info' | 'warn' | 'error';
  from?: Date | std:;
  to?: Date | std:;
  search?: std:;
}

} // namespace elizaos
