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


// Extend the core service types with goals service
  struct ServiceTypeRegistry {
    'goals' GOALS;
};

// Export service type constant

/**
 * Goal data structure from database
 */
struct GoalData {
    UUID id;
    UUID agentId;
    'agent' | 'entity' ownerType;
    UUID ownerId;
    std::string name;
    std::optional<string | null> description;
    bool isCompleted;
    std::optional<Date | null> completedAt;
    Date createdAt;
    Date updatedAt;
    std::unordered_map<std::string, std::any> metadata;
    std::optional<std::vector<std::string>> tags;
};


} // namespace elizaos
