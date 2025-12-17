#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "base.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Agent {
    UUID id;
    std::string name;
    std::optional<std::variant<std::string, std::vector<std::string>>> bio;
    std::optional<std::string> characterName;
    std::string status;
    std::optional<bool> enabled;
    std::optional<Date> createdAt;
    std::optional<Date> updatedAt;
};

struct AgentCreateParams {
    std::optional<std::string> characterPath;
};

struct AgentUpdateParams {
    std::optional<std::string> name;
    std::optional<std::variant<std::string, std::vector<std::string>>> bio;
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
    std::optional<UUID> id;
    std::optional<std::string> type;
    std::optional<double> timestamp;
    std::optional<std::string> message;
    std::optional<std::string> details;
    std::optional<UUID> roomId;
    std::optional<std::string> modelType;
    std::optional<std::string> modelKey;
    std::optional<std::any> params;
    std::optional<std::any> response;
    std::optional<double> prompt_tokens;
    std::optional<double> completion_tokens;
    std::optional<double> total_tokens;
    std::optional<double> createdAt;
};


} // namespace elizaos
