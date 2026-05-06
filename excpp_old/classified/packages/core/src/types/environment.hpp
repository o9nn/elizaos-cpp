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


struct Component {
    UUID id;
    UUID entityId;
    UUID agentId;
    UUID roomId;
    UUID worldId;
    UUID sourceEntityId;
    std::string type;
    double createdAt;
    Metadata data;
};

/**
 * Represents a user account
 */
struct Entity {
    std::optional<UUID> id;
    std::vector<std::string> names;
    std::optional<Metadata> metadata;
    UUID agentId;
    std::optional<std::vector<Component>> components;
};

/**
 * Defines roles within a system, typically for access control or permissions, often within a `World`.
 * - `OWNER`: Represents the highest level of control, typically the creator or primary administrator.
 * - `ADMIN`: Represents administrative privileges, usually a subset of owner capabilities.
 * - `NONE`: Indicates no specific role or default, minimal permissions.
 * These roles are often used in `World.metadata.roles` to assign roles to entities.
 */
enum Role {
}

using World = {

enum ChannelType {
  // Legacy types - kept for backward compatibility but should be replaced
}

using Room = {

using RoomMetadata = {

/**
 * Room participant with account details
 */
struct Participant {
    UUID id;
    Entity entity;
};

/**
 * Represents a relationship between users
 */
struct Relationship {
    UUID id;
    UUID sourceEntityId;
    UUID targetEntityId;
    UUID agentId;
    std::vector<std::string> tags;
    Metadata metadata;
    std::optional<std::string> createdAt;
};


} // namespace elizaos
