#pragma once

#include "elizaos/core.hpp"
#include <algorithm>  // std::sort / std::remove_if / std::find
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Character Modification Data Structures
// ============================================================================

struct CharacterModification {
    std::string id;
    std::string agentId;
    int versionNumber{0};
    std::string diffXml;
    std::string reasoning;
    std::chrono::system_clock::time_point appliedAt;
    std::optional<std::chrono::system_clock::time_point> rolledBackAt;
    std::chrono::system_clock::time_point createdAt;
};

struct CharacterSnapshot {
    std::string id;
    std::string agentId;
    int versionNumber{0};
    std::string characterData;  // JSON string
    std::chrono::system_clock::time_point createdAt;
};

struct RateLimitStatus {
    int hourlyCount{0};
    int dailyCount{0};
};

struct LockStatus {
    bool locked{false};
    std::string lockedBy;
    std::string lockReason;
};

struct EvolutionRecommendation {
    std::string id;
    std::string recommendation;
    std::string analysisResult;
    std::chrono::system_clock::time_point createdAt;
};

// ============================================================================
// In-Memory Database Adapter for Character Modification
// ============================================================================

/**
 * Database adapter for character modification data.
 * Provides an in-memory implementation that can be backed by SQLite or PostgreSQL
 * when those backends become available. All operations are thread-safe.
 */
class CharacterModificationDatabaseAdapter {
public:
    CharacterModificationDatabaseAdapter() = default;

    /**
     * Save a character modification to the database.
     */
    void saveModification(const CharacterModification& modification) {
        std::lock_guard<std::mutex> lock(mutex_);
        modifications_[modification.agentId].push_back(modification);
    }

    /**
     * Save a character snapshot to the database.
     */
    void saveSnapshot(const CharacterSnapshot& snapshot) {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshots_[snapshot.agentId].push_back(snapshot);
    }

    /**
     * Load modification history for an agent, ordered by version number ascending.
     */
    std::vector<CharacterModification> loadModifications(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = modifications_.find(agentId);
        if (it == modifications_.end()) {
            return {};
        }
        auto result = it->second;
        std::sort(result.begin(), result.end(),
                  [](const CharacterModification& a, const CharacterModification& b) {
                      return a.versionNumber < b.versionNumber;
                  });
        return result;
    }

    /**
     * Load snapshots for an agent, ordered by version number ascending.
     */
    std::vector<CharacterSnapshot> loadSnapshots(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = snapshots_.find(agentId);
        if (it == snapshots_.end()) {
            return {};
        }
        auto result = it->second;
        std::sort(result.begin(), result.end(),
                  [](const CharacterSnapshot& a, const CharacterSnapshot& b) {
                      return a.versionNumber < b.versionNumber;
                  });
        return result;
    }

    /**
     * Mark modifications as rolled back from a given version forward.
     */
    void rollbackModifications(const std::string& agentId, int fromVersion) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = modifications_.find(agentId);
        if (it == modifications_.end()) return;
        const auto now = std::chrono::system_clock::now();
        for (auto& mod : it->second) {
            if (mod.versionNumber > fromVersion) {
                mod.rolledBackAt = now;
            }
        }
    }

    /**
     * Save rate limit attempt for an agent.
     */
    void saveRateLimitAttempt(const std::string& agentId, bool successful) {
        std::lock_guard<std::mutex> lock(mutex_);
        rateLimitAttempts_[agentId].push_back({std::chrono::system_clock::now(), successful});
    }

    /**
     * Check rate limit for an agent. Returns hourly and daily counts of successful attempts.
     */
    RateLimitStatus checkRateLimit(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = rateLimitAttempts_.find(agentId);
        if (it == rateLimitAttempts_.end()) {
            return {0, 0};
        }
        const auto now = std::chrono::system_clock::now();
        const auto oneHourAgo = now - std::chrono::hours(1);
        const auto oneDayAgo = now - std::chrono::hours(24);
        RateLimitStatus status;
        for (const auto& [timestamp, successful] : it->second) {
            if (successful) {
                if (timestamp > oneHourAgo) ++status.hourlyCount;
                if (timestamp > oneDayAgo) ++status.dailyCount;
            }
        }
        return status;
    }

    /**
     * Get lock status for an agent.
     */
    LockStatus getLockStatus(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = locks_.find(agentId);
        if (it == locks_.end()) {
            return {false, "", ""};
        }
        return it->second;
    }

    /**
     * Set lock status for an agent.
     */
    void setLockStatus(const std::string& agentId, bool locked,
                       const std::string& lockedBy = "",
                       const std::string& lockReason = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        locks_[agentId] = {locked, lockedBy, lockReason};
    }

    /**
     * Save evolution recommendation from evaluator.
     */
    void saveEvolutionRecommendation(const std::string& agentId,
                                     const std::string& roomId,
                                     const std::string& conversationId,
                                     const std::string& recommendation,
                                     const std::string& analysisResult) {
        std::lock_guard<std::mutex> lock(mutex_);
        EvolutionRecommendation rec;
        rec.id = generateUUID();
        rec.recommendation = recommendation;
        rec.analysisResult = analysisResult;
        rec.createdAt = std::chrono::system_clock::now();
        recommendations_[agentId].push_back(std::move(rec));
        (void)roomId;
        (void)conversationId;
    }

    /**
     * Get unprocessed evolution recommendations for an agent (up to 10).
     */
    std::vector<EvolutionRecommendation> getUnprocessedRecommendations(
        const std::string& agentId, std::size_t limit = 10) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = recommendations_.find(agentId);
        if (it == recommendations_.end()) {
            return {};
        }
        auto result = it->second;
        std::sort(result.begin(), result.end(),
                  [](const EvolutionRecommendation& a, const EvolutionRecommendation& b) {
                      return a.createdAt < b.createdAt;
                  });
        if (result.size() > limit) {
            result.resize(limit);
        }
        return result;
    }

    /**
     * Mark recommendations as processed (remove from unprocessed list).
     */
    void markRecommendationsProcessed(const std::string& agentId,
                                      const std::vector<std::string>& ids) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = recommendations_.find(agentId);
        if (it == recommendations_.end()) return;
        auto& recs = it->second;
        recs.erase(std::remove_if(recs.begin(), recs.end(),
                                  [&ids](const EvolutionRecommendation& r) {
                                      return std::find(ids.begin(), ids.end(), r.id) != ids.end();
                                  }),
                   recs.end());
    }

    /**
     * Get the latest version number for an agent.
     */
    int getLatestVersion(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = modifications_.find(agentId);
        if (it == modifications_.end() || it->second.empty()) {
            return 0;
        }
        int maxVersion = 0;
        for (const auto& mod : it->second) {
            if (mod.versionNumber > maxVersion && !mod.rolledBackAt.has_value()) {
                maxVersion = mod.versionNumber;
            }
        }
        return maxVersion;
    }

private:
    mutable std::mutex mutex_;

    // In-memory storage (to be replaced by SQLite/PostgreSQL backend)
    std::unordered_map<std::string, std::vector<CharacterModification>> modifications_;
    std::unordered_map<std::string, std::vector<CharacterSnapshot>> snapshots_;

    struct RateLimitEntry {
        std::chrono::system_clock::time_point timestamp;
        bool successful;
    };
    std::unordered_map<std::string, std::vector<RateLimitEntry>> rateLimitAttempts_;
    std::unordered_map<std::string, LockStatus> locks_;
    std::unordered_map<std::string, std::vector<EvolutionRecommendation>> recommendations_;
};

} // namespace elizaos
