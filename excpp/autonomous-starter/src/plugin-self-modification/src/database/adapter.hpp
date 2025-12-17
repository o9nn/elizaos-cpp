#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Database adapter for character modification data
 * Provides database-agnostic interface for both SQLite and PostgreSQL
 */
class CharacterModificationDatabaseAdapter {
public:
    CharacterModificationDatabaseAdapter();
    std::future<void> initializeSchema();
    std::future<void> initializeSQLiteSchema();
    std::future<void> initializePostgreSQLSchema();
    std::future<std::string> generateId();
    std::future<std::string> getCurrentTimestamp();
    std::future<void> saveModification(CharacterModification modification);
    std::future<void> saveSnapshot(CharacterSnapshot snapshot);
    std::future<std::vector<CharacterModification>> loadModificationHistory(UUID agentId);
    std::future<std::vector<CharacterSnapshot>> loadSnapshots(UUID agentId);
    std::future<void> markModificationsRolledBack(UUID agentId, double fromVersion);
    std::future<void> saveRateLimitAttempt(UUID agentId, bool successful);
    Promise< checkRateLimit(UUID agentId);
    Promise< getLockStatus(UUID agentId);
    std::future<void> setLockStatus(UUID agentId, bool locked, std::optional<std::string> lockedBy, std::optional<std::string> lockReason);
    std::future<void> saveEvolutionRecommendation(UUID agentId, const std::optional<UUID>& roomId, UUID conversationId, const std::string& recommendation, const std::string& analysisResult);
    Array< getUnprocessedRecommendations(UUID agentId);
};


} // namespace elizaos
