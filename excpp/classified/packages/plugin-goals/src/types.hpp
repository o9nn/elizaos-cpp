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


// Extend the core service types with goals service
  struct ServiceTypeRegistry {
    std::string GOALS;
};

// Export service type constant

/**
 * Goal data structure from database
 */
struct GoalData {
    UUID id;
    UUID agentId;
    std::string ownerType;
    UUID ownerId;
    std::string name;
    std::optional<std::string> description;
    bool isCompleted;
    std::optional<std::optional<Date>> completedAt;
    Date createdAt;
    Date updatedAt;
    std::optional<std::vector<std::string>> tags;
};


} // namespace elizaos
