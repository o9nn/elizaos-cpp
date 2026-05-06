#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define the minimal database type we need
using DrizzleDatabase = std::function<void()>;

/**
 * ServerMigrationService handles database migrations for server-specific tables
 * This is separate from plugin migrations to maintain clean separation of concerns
 */
class ServerMigrationService {
public:
    ServerMigrationService();
    std::future<void> runMigrations();
    std::future<void> createMessageServersTable();
    std::future<void> createChannelsTable();
    std::future<void> createMessagesTable();
    std::future<void> createChannelParticipantsTable();
    std::future<void> createServerAgentsTable();
    std::future<bool> tableExists(const std::string& tableName);
};


} // namespace elizaos
