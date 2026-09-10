#pragma once

#include "elizaos/core.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace elizaos {

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
    std::string characterData;
    std::chrono::system_clock::time_point createdAt;
};

struct RateLimitStatus {
    int hourlyCount{0};
    int dailyCount{0};
};

struct RateLimitAttempt {
    std::chrono::system_clock::time_point attemptedAt;
    bool successful{false};
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
    // Appended after the original fields to preserve positional aggregate initialization.
    std::string agentId;
    std::string roomId;
    std::string conversationId;
    bool processed{false};
    std::optional<std::chrono::system_clock::time_point> processedAt;
    std::map<std::string, std::string> metadata;
};

/**
 * Thread-safe character-modification storage.
 *
 * The default constructor retains the original, deterministic in-memory mode.
 * Passing a path enables explicit file-backed mode and immediately attempts a
 * transactional reload. The JSON file is versioned and is replaced by writing
 * and syncing a same-directory temporary file followed by an atomic rename.
 *
 * Existing void mutators remain source compatible. They never throw for normal
 * validation or I/O failures, leave memory unchanged when persistence fails,
 * and expose the failure through lastPersistenceError(). New try* variants
 * return the outcome directly. A successful mutating operation clears the last
 * error. File-backed instances are synchronized within the adapter instance;
 * cross-process writers require an external lock.
 */
class CharacterModificationDatabaseAdapter {
public:
    static constexpr int kSchemaVersion = 1;
    static constexpr std::size_t kMaxFileBytes = 16U * 1024U * 1024U;
    static constexpr std::size_t kMaxRecordsPerCollection = 100000U;
    static constexpr std::size_t kMaxIdentifierLength = 4096U;
    static constexpr std::size_t kMaxTextLength = 4U * 1024U * 1024U;
    static constexpr std::size_t kMaxMetadataEntries = 256U;
    static constexpr std::size_t kMaxRecommendationQueryLimit = 1000U;

    CharacterModificationDatabaseAdapter() = default;

    explicit CharacterModificationDatabaseAdapter(std::filesystem::path persistencePath)
        : persistencePath_(std::move(persistencePath)) {
        if (persistencePath_->empty()) {
            lastError_ = "persistence path must not be empty";
            persistencePath_.reset();
            return;
        }
        (void)reload();
    }

    CharacterModificationDatabaseAdapter(const CharacterModificationDatabaseAdapter&) = delete;
    CharacterModificationDatabaseAdapter& operator=(const CharacterModificationDatabaseAdapter&) = delete;

    bool isFileBacked() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return persistencePath_.has_value();
    }

    std::string lastPersistenceError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return lastError_;
    }

    std::string lastError() const { return lastPersistenceError(); }

    /** Reload replaces the complete in-memory state only after full validation. */
    bool reload() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!persistencePath_) {
            lastError_ = "reload requires file-backed mode";
            return false;
        }

        try {
            std::error_code ec;
            const bool exists = std::filesystem::exists(*persistencePath_, ec);
            if (ec) {
                lastError_ = "cannot inspect persistence file: " + ec.message();
                return false;
            }
            if (!exists) {
                state_ = State{};
                lastError_.clear();
                return true;
            }
            if (!std::filesystem::is_regular_file(*persistencePath_, ec) || ec) {
                lastError_ = "persistence path is not a regular file";
                return false;
            }
            const auto size = std::filesystem::file_size(*persistencePath_, ec);
            if (ec) {
                lastError_ = "cannot determine persistence file size: " + ec.message();
                return false;
            }
            if (size > kMaxFileBytes) {
                lastError_ = "persistence file exceeds the configured byte limit";
                return false;
            }

            std::ifstream input(*persistencePath_, std::ios::binary);
            if (!input) {
                lastError_ = "cannot open persistence file for reading";
                return false;
            }
            std::string bytes(static_cast<std::size_t>(size), '\0');
            if (size != 0U) {
                input.read(bytes.data(), static_cast<std::streamsize>(size));
                if (!input || static_cast<std::uintmax_t>(input.gcount()) != size) {
                    lastError_ = "short read while loading persistence file";
                    return false;
                }
            }
            if (bytes.empty()) {
                lastError_ = "persistence file is empty";
                return false;
            }

            State candidate;
            std::string error;
            const auto root = nlohmann::json::parse(bytes);
            if (!decodeState(root, candidate, error)) {
                lastError_ = error;
                return false;
            }
            pruneExpiredRateLimitHistory(candidate, std::chrono::system_clock::now());
            if (!validateState(candidate, error)) {
                lastError_ = error;
                return false;
            }
            state_ = std::move(candidate);
            lastError_.clear();
            return true;
        } catch (const std::exception& e) {
            lastError_ = std::string("failed to load persistence file: ") + e.what();
            return false;
        } catch (...) {
            lastError_ = "failed to load persistence file: unknown error";
            return false;
        }
    }

    /** Force a deterministic rewrite of the current file-backed state. */
    bool persist() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!persistencePath_) {
            lastError_ = "persist requires file-backed mode";
            return false;
        }
        try {
            State candidate = state_;
            pruneExpiredRateLimitHistory(candidate, std::chrono::system_clock::now());
            std::string error;
            if (!validateState(candidate, error) || !writeStateFile(candidate, error)) {
                lastError_ = error;
                return false;
            }
            state_ = std::move(candidate);
            lastError_.clear();
            return true;
        } catch (const std::exception& e) {
            lastError_ = std::string("failed to persist state: ") + e.what();
            return false;
        } catch (...) {
            lastError_ = "failed to persist state: unknown error";
            return false;
        }
    }

    void saveModification(const CharacterModification& modification) {
        (void)trySaveModification(modification);
    }

    bool trySaveModification(const CharacterModification& modification) {
        return mutate([&](State& candidate, std::string&) {
            candidate.modifications[modification.agentId].push_back(modification);
            return true;
        });
    }

    void saveSnapshot(const CharacterSnapshot& snapshot) { (void)trySaveSnapshot(snapshot); }

    bool trySaveSnapshot(const CharacterSnapshot& snapshot) {
        return mutate([&](State& candidate, std::string&) {
            candidate.snapshots[snapshot.agentId].push_back(snapshot);
            return true;
        });
    }

    std::vector<CharacterModification> loadModifications(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.modifications.find(agentId);
        if (it == state_.modifications.end()) return {};
        auto result = it->second;
        std::sort(result.begin(), result.end(), modificationLess);
        return result;
    }

    std::vector<CharacterSnapshot> loadSnapshots(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.snapshots.find(agentId);
        if (it == state_.snapshots.end()) return {};
        auto result = it->second;
        std::sort(result.begin(), result.end(), snapshotLess);
        return result;
    }

    void rollbackModifications(const std::string& agentId, int fromVersion) {
        (void)tryRollbackModifications(agentId, fromVersion);
    }

    bool tryRollbackModifications(
        const std::string& agentId, int fromVersion,
        std::chrono::system_clock::time_point rolledBackAt = std::chrono::system_clock::now()) {
        return mutate([&](State& candidate, std::string& error) {
            if (!validIdentifier(agentId)) {
                error = "agentId must be non-empty and within the identifier limit";
                return false;
            }
            if (fromVersion < 0) {
                error = "rollback version must not be negative";
                return false;
            }
            auto it = candidate.modifications.find(agentId);
            if (it != candidate.modifications.end()) {
                for (auto& modification : it->second) {
                    if (modification.versionNumber > fromVersion) {
                        modification.rolledBackAt = rolledBackAt;
                    }
                }
            }
            return true;
        });
    }

    void saveRateLimitAttempt(const std::string& agentId, bool successful) {
        (void)trySaveRateLimitAttempt(agentId, successful);
    }

    bool trySaveRateLimitAttempt(
        const std::string& agentId, bool successful,
        std::chrono::system_clock::time_point attemptedAt = std::chrono::system_clock::now()) {
        return mutate([&](State& candidate, std::string& error) {
            if (!validIdentifier(agentId)) {
                error = "agentId must be non-empty and within the identifier limit";
                return false;
            }
            candidate.rateLimitAttempts[agentId].push_back({attemptedAt, successful});
            return true;
        });
    }

    bool tryPruneExpiredRateLimitHistory(
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now()) {
        return mutate([&](State& candidate, std::string&) {
            pruneExpiredRateLimitHistory(candidate, now);
            return true;
        }, now);
    }

    RateLimitStatus checkRateLimit(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.rateLimitAttempts.find(agentId);
        if (it == state_.rateLimitAttempts.end()) return {};
        const auto now = std::chrono::system_clock::now();
        const auto oneHourAgo = now - std::chrono::hours(1);
        const auto oneDayAgo = now - std::chrono::hours(24);
        RateLimitStatus status;
        for (const auto& attempt : it->second) {
            if (!attempt.successful) continue;
            if (attempt.attemptedAt > oneHourAgo) ++status.hourlyCount;
            if (attempt.attemptedAt > oneDayAgo) ++status.dailyCount;
        }
        return status;
    }

    std::vector<RateLimitAttempt> loadRateLimitAttempts(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.rateLimitAttempts.find(agentId);
        if (it == state_.rateLimitAttempts.end()) return {};
        auto result = it->second;
        std::sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.attemptedAt != rhs.attemptedAt) return lhs.attemptedAt < rhs.attemptedAt;
            return lhs.successful < rhs.successful;
        });
        return result;
    }

    LockStatus getLockStatus(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.locks.find(agentId);
        return it == state_.locks.end() ? LockStatus{} : it->second;
    }

    void setLockStatus(const std::string& agentId, bool locked,
                       const std::string& lockedBy = "",
                       const std::string& lockReason = "") {
        (void)trySetLockStatus(agentId, locked, lockedBy, lockReason);
    }

    bool trySetLockStatus(const std::string& agentId, bool locked,
                          const std::string& lockedBy = "",
                          const std::string& lockReason = "") {
        return mutate([&](State& candidate, std::string& error) {
            if (!validIdentifier(agentId)) {
                error = "agentId must be non-empty and within the identifier limit";
                return false;
            }
            candidate.locks[agentId] = {locked, lockedBy, lockReason};
            return true;
        });
    }

    void saveEvolutionRecommendation(const std::string& agentId,
                                     const std::string& roomId,
                                     const std::string& conversationId,
                                     const std::string& recommendation,
                                     const std::string& analysisResult) {
        (void)trySaveEvolutionRecommendation(agentId, roomId, conversationId,
                                             recommendation, analysisResult);
    }

    bool trySaveEvolutionRecommendation(
        const std::string& agentId, const std::string& roomId,
        const std::string& conversationId, const std::string& recommendation,
        const std::string& analysisResult,
        const std::map<std::string, std::string>& metadata = {}) {
        return mutate([&](State& candidate, std::string&) {
            EvolutionRecommendation value;
            value.id = generateUUID();
            value.agentId = agentId;
            value.roomId = roomId;
            value.conversationId = conversationId;
            value.recommendation = recommendation;
            value.analysisResult = analysisResult;
            value.createdAt = std::chrono::system_clock::now();
            value.metadata = metadata;
            candidate.recommendations[agentId].push_back(std::move(value));
            return true;
        });
    }

    bool trySaveEvolutionRecommendation(const EvolutionRecommendation& recommendation) {
        return mutate([&](State& candidate, std::string&) {
            candidate.recommendations[recommendation.agentId].push_back(recommendation);
            return true;
        });
    }

    std::vector<EvolutionRecommendation> getUnprocessedRecommendations(
        const std::string& agentId, std::size_t limit = 10) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        if (limit == 0U || limit > kMaxRecommendationQueryLimit) {
            lastError_ = "recommendation limit must be between 1 and " +
                         std::to_string(kMaxRecommendationQueryLimit);
            return {};
        }
        std::vector<EvolutionRecommendation> result;
        auto it = state_.recommendations.find(agentId);
        if (it == state_.recommendations.end()) return result;
        for (const auto& recommendation : it->second) {
            if (!recommendation.processed) result.push_back(recommendation);
        }
        std::sort(result.begin(), result.end(), recommendationLess);
        if (result.size() > limit) result.resize(limit);
        return result;
    }

    /** Return recommendations including retained processed records. */
    std::vector<EvolutionRecommendation> loadRecommendations(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return {};
        }
        auto it = state_.recommendations.find(agentId);
        if (it == state_.recommendations.end()) return {};
        auto result = it->second;
        std::sort(result.begin(), result.end(), recommendationLess);
        return result;
    }

    void markRecommendationsProcessed(const std::string& agentId,
                                      const std::vector<std::string>& ids) {
        (void)tryMarkRecommendationsProcessed(agentId, ids);
    }

    bool tryMarkRecommendationsProcessed(
        const std::string& agentId, const std::vector<std::string>& ids,
        std::chrono::system_clock::time_point processedAt = std::chrono::system_clock::now()) {
        return mutate([&](State& candidate, std::string& error) {
            if (!validIdentifier(agentId)) {
                error = "agentId must be non-empty and within the identifier limit";
                return false;
            }
            if (ids.empty() || ids.size() > kMaxRecordsPerCollection) {
                error = "processed recommendation IDs must be non-empty and within the record limit";
                return false;
            }
            std::set<std::string> uniqueIds;
            for (const auto& id : ids) {
                if (!validIdentifier(id) || !uniqueIds.insert(id).second) {
                    error = "processed recommendation IDs must be valid and unique";
                    return false;
                }
            }
            auto it = candidate.recommendations.find(agentId);
            if (it == candidate.recommendations.end()) return true;
            for (auto& recommendation : it->second) {
                if (uniqueIds.count(recommendation.id) != 0U) {
                    recommendation.processed = true;
                    recommendation.processedAt = processedAt;
                }
            }
            return true;
        });
    }

    int getLatestVersion(const std::string& agentId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!validIdentifier(agentId)) {
            lastError_ = "agentId must be non-empty and within the identifier limit";
            return 0;
        }
        auto it = state_.modifications.find(agentId);
        if (it == state_.modifications.end()) return 0;
        int maximum = 0;
        for (const auto& modification : it->second) {
            if (!modification.rolledBackAt && modification.versionNumber > maximum) {
                maximum = modification.versionNumber;
            }
        }
        return maximum;
    }

private:
    using ModificationMap = std::map<std::string, std::vector<CharacterModification>>;
    using SnapshotMap = std::map<std::string, std::vector<CharacterSnapshot>>;
    using RateLimitMap = std::map<std::string, std::vector<RateLimitAttempt>>;
    using LockMap = std::map<std::string, LockStatus>;
    using RecommendationMap = std::map<std::string, std::vector<EvolutionRecommendation>>;

    struct State {
        ModificationMap modifications;
        SnapshotMap snapshots;
        RateLimitMap rateLimitAttempts;
        LockMap locks;
        RecommendationMap recommendations;
    };

    mutable std::mutex mutex_;
    mutable std::string lastError_;
    std::optional<std::filesystem::path> persistencePath_;
    State state_;

    static bool validIdentifier(const std::string& value) {
        return !value.empty() && value.size() <= kMaxIdentifierLength;
    }

    static bool validText(const std::string& value, bool allowEmpty = true) {
        return (allowEmpty || !value.empty()) && value.size() <= kMaxTextLength;
    }

    static std::int64_t epochNanoseconds(std::chrono::system_clock::time_point value) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch()).count();
    }

    static bool fromEpochNanoseconds(std::int64_t value,
                                     std::chrono::system_clock::time_point& result) {
        const auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds(value));
        result = std::chrono::system_clock::time_point(duration);
        return epochNanoseconds(result) == value;
    }

    static bool modificationLess(const CharacterModification& lhs,
                                 const CharacterModification& rhs) {
        if (lhs.versionNumber != rhs.versionNumber) return lhs.versionNumber < rhs.versionNumber;
        return lhs.id < rhs.id;
    }

    static bool snapshotLess(const CharacterSnapshot& lhs, const CharacterSnapshot& rhs) {
        if (lhs.versionNumber != rhs.versionNumber) return lhs.versionNumber < rhs.versionNumber;
        return lhs.id < rhs.id;
    }

    static bool recommendationLess(const EvolutionRecommendation& lhs,
                                   const EvolutionRecommendation& rhs) {
        if (lhs.createdAt != rhs.createdAt) return lhs.createdAt < rhs.createdAt;
        return lhs.id < rhs.id;
    }

    static void pruneExpiredRateLimitHistory(State& state,
                                             std::chrono::system_clock::time_point now) {
        const auto cutoff = now - std::chrono::hours(24);
        for (auto it = state.rateLimitAttempts.begin(); it != state.rateLimitAttempts.end();) {
            auto& attempts = it->second;
            attempts.erase(std::remove_if(attempts.begin(), attempts.end(),
                                          [&](const RateLimitAttempt& attempt) {
                                              return attempt.attemptedAt <= cutoff;
                                          }),
                           attempts.end());
            if (attempts.empty()) {
                it = state.rateLimitAttempts.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename Mutation>
    bool mutate(Mutation&& mutation,
                std::chrono::system_clock::time_point pruneAt = std::chrono::system_clock::now()) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            State candidate = state_;
            std::string error;
            if (!mutation(candidate, error)) {
                lastError_ = error.empty() ? "mutation rejected" : error;
                return false;
            }
            pruneExpiredRateLimitHistory(candidate, pruneAt);
            if (!validateState(candidate, error)) {
                lastError_ = error;
                return false;
            }
            if (persistencePath_ && !writeStateFile(candidate, error)) {
                lastError_ = error;
                return false;
            }
            state_ = std::move(candidate);
            lastError_.clear();
            return true;
        } catch (const std::exception& e) {
            lastError_ = std::string("mutation failed without changing state: ") + e.what();
            return false;
        } catch (...) {
            lastError_ = "mutation failed without changing state: unknown error";
            return false;
        }
    }

    static bool validateMetadata(const std::map<std::string, std::string>& metadata,
                                 std::string& error) {
        if (metadata.size() > kMaxMetadataEntries) {
            error = "recommendation metadata exceeds the entry limit";
            return false;
        }
        for (const auto& entry : metadata) {
            if (!validIdentifier(entry.first) || !validText(entry.second)) {
                error = "recommendation metadata contains an invalid key or value";
                return false;
            }
        }
        return true;
    }

    static bool validateState(const State& state, std::string& error) {
        std::size_t modificationCount = 0;
        std::set<std::string> modificationIds;
        for (const auto& entry : state.modifications) {
            if (!validIdentifier(entry.first)) {
                error = "modification map contains an invalid agentId";
                return false;
            }
            std::set<int> versions;
            modificationCount += entry.second.size();
            for (const auto& value : entry.second) {
                if (!validIdentifier(value.id) || value.agentId != entry.first ||
                    value.versionNumber < 0 || !validText(value.diffXml, false) ||
                    !validText(value.reasoning)) {
                    error = "invalid character modification";
                    return false;
                }
                if (!modificationIds.insert(value.id).second ||
                    !versions.insert(value.versionNumber).second) {
                    error = "duplicate character modification ID or agent version";
                    return false;
                }
            }
        }
        if (modificationCount > kMaxRecordsPerCollection) {
            error = "character modifications exceed the record limit";
            return false;
        }

        std::size_t snapshotCount = 0;
        std::set<std::string> snapshotIds;
        for (const auto& entry : state.snapshots) {
            if (!validIdentifier(entry.first)) {
                error = "snapshot map contains an invalid agentId";
                return false;
            }
            std::set<int> versions;
            snapshotCount += entry.second.size();
            for (const auto& value : entry.second) {
                if (!validIdentifier(value.id) || value.agentId != entry.first ||
                    value.versionNumber < 0 || !validText(value.characterData, false)) {
                    error = "invalid character snapshot";
                    return false;
                }
                if (!snapshotIds.insert(value.id).second ||
                    !versions.insert(value.versionNumber).second) {
                    error = "duplicate character snapshot ID or agent version";
                    return false;
                }
            }
        }
        if (snapshotCount > kMaxRecordsPerCollection) {
            error = "character snapshots exceed the record limit";
            return false;
        }

        std::size_t attemptCount = 0;
        for (const auto& entry : state.rateLimitAttempts) {
            if (!validIdentifier(entry.first)) {
                error = "rate-limit map contains an invalid agentId";
                return false;
            }
            attemptCount += entry.second.size();
        }
        if (attemptCount > kMaxRecordsPerCollection) {
            error = "rate-limit attempts exceed the record limit";
            return false;
        }

        if (state.locks.size() > kMaxRecordsPerCollection) {
            error = "locks exceed the record limit";
            return false;
        }
        for (const auto& entry : state.locks) {
            if (!validIdentifier(entry.first) || !validText(entry.second.lockedBy) ||
                !validText(entry.second.lockReason)) {
                error = "invalid lock record";
                return false;
            }
        }

        std::size_t recommendationCount = 0;
        std::set<std::string> recommendationIds;
        for (const auto& entry : state.recommendations) {
            if (!validIdentifier(entry.first)) {
                error = "recommendation map contains an invalid agentId";
                return false;
            }
            recommendationCount += entry.second.size();
            for (const auto& value : entry.second) {
                if (!validIdentifier(value.id) || value.agentId != entry.first ||
                    !validIdentifier(value.roomId) || !validIdentifier(value.conversationId) ||
                    !validText(value.recommendation, false) || !validText(value.analysisResult) ||
                    (value.processed != value.processedAt.has_value()) ||
                    !validateMetadata(value.metadata, error)) {
                    if (error.empty()) error = "invalid evolution recommendation";
                    return false;
                }
                if (!recommendationIds.insert(value.id).second) {
                    error = "duplicate evolution recommendation ID";
                    return false;
                }
            }
        }
        if (recommendationCount > kMaxRecordsPerCollection) {
            error = "evolution recommendations exceed the record limit";
            return false;
        }
        return true;
    }

    static nlohmann::json encodeState(const State& state) {
        nlohmann::json modifications = nlohmann::json::array();
        std::vector<CharacterModification> sortedModifications;
        for (const auto& entry : state.modifications) {
            sortedModifications.insert(sortedModifications.end(), entry.second.begin(), entry.second.end());
        }
        std::sort(sortedModifications.begin(), sortedModifications.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.agentId != rhs.agentId) return lhs.agentId < rhs.agentId;
            return modificationLess(lhs, rhs);
        });
        for (const auto& value : sortedModifications) {
            modifications.push_back({
                {"agentId", value.agentId}, {"appliedAtUnixNanos", epochNanoseconds(value.appliedAt)},
                {"createdAtUnixNanos", epochNanoseconds(value.createdAt)}, {"diffXml", value.diffXml},
                {"id", value.id}, {"reasoning", value.reasoning},
                {"rolledBackAtUnixNanos", value.rolledBackAt
                    ? nlohmann::json(epochNanoseconds(*value.rolledBackAt)) : nlohmann::json(nullptr)},
                {"versionNumber", value.versionNumber}
            });
        }

        nlohmann::json snapshots = nlohmann::json::array();
        std::vector<CharacterSnapshot> sortedSnapshots;
        for (const auto& entry : state.snapshots) {
            sortedSnapshots.insert(sortedSnapshots.end(), entry.second.begin(), entry.second.end());
        }
        std::sort(sortedSnapshots.begin(), sortedSnapshots.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.agentId != rhs.agentId) return lhs.agentId < rhs.agentId;
            return snapshotLess(lhs, rhs);
        });
        for (const auto& value : sortedSnapshots) {
            snapshots.push_back({{"agentId", value.agentId}, {"characterData", value.characterData},
                                 {"createdAtUnixNanos", epochNanoseconds(value.createdAt)},
                                 {"id", value.id}, {"versionNumber", value.versionNumber}});
        }

        nlohmann::json attempts = nlohmann::json::array();
        for (const auto& entry : state.rateLimitAttempts) {
            auto sorted = entry.second;
            std::sort(sorted.begin(), sorted.end(), [](const auto& lhs, const auto& rhs) {
                if (lhs.attemptedAt != rhs.attemptedAt) return lhs.attemptedAt < rhs.attemptedAt;
                return lhs.successful < rhs.successful;
            });
            for (const auto& value : sorted) {
                attempts.push_back({{"agentId", entry.first},
                                    {"attemptedAtUnixNanos", epochNanoseconds(value.attemptedAt)},
                                    {"successful", value.successful}});
            }
        }

        nlohmann::json locks = nlohmann::json::array();
        for (const auto& entry : state.locks) {
            locks.push_back({{"agentId", entry.first}, {"lockReason", entry.second.lockReason},
                             {"locked", entry.second.locked}, {"lockedBy", entry.second.lockedBy}});
        }

        nlohmann::json recommendations = nlohmann::json::array();
        std::vector<EvolutionRecommendation> sortedRecommendations;
        for (const auto& entry : state.recommendations) {
            sortedRecommendations.insert(sortedRecommendations.end(), entry.second.begin(), entry.second.end());
        }
        std::sort(sortedRecommendations.begin(), sortedRecommendations.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.agentId != rhs.agentId) return lhs.agentId < rhs.agentId;
            return recommendationLess(lhs, rhs);
        });
        for (const auto& value : sortedRecommendations) {
            recommendations.push_back({
                {"agentId", value.agentId}, {"analysisResult", value.analysisResult},
                {"conversationId", value.conversationId},
                {"createdAtUnixNanos", epochNanoseconds(value.createdAt)}, {"id", value.id},
                {"metadata", value.metadata}, {"processed", value.processed},
                {"processedAtUnixNanos", value.processedAt
                    ? nlohmann::json(epochNanoseconds(*value.processedAt)) : nlohmann::json(nullptr)},
                {"recommendation", value.recommendation}, {"roomId", value.roomId}
            });
        }

        return {{"format", "elizaos.character-modification-database"},
                {"locks", std::move(locks)}, {"modifications", std::move(modifications)},
                {"rateLimitAttempts", std::move(attempts)},
                {"recommendations", std::move(recommendations)},
                {"schemaVersion", kSchemaVersion}, {"snapshots", std::move(snapshots)}};
    }

    static bool hasExactKeys(const nlohmann::json& object,
                             std::initializer_list<const char*> keys) {
        if (!object.is_object() || object.size() != keys.size()) return false;
        return std::all_of(keys.begin(), keys.end(), [&](const char* key) { return object.contains(key); });
    }

    static bool readTimestamp(const nlohmann::json& object, const char* key,
                              std::chrono::system_clock::time_point& value,
                              std::string& error) {
        const auto& encodedValue = object.at(key);
        std::int64_t encoded = 0;
        if (encodedValue.is_number_unsigned()) {
            const auto unsignedValue = encodedValue.get<std::uint64_t>();
            if (unsignedValue > static_cast<std::uint64_t>((std::numeric_limits<std::int64_t>::max)())) {
                error = std::string(key) + " exceeds the signed timestamp range";
                return false;
            }
            encoded = static_cast<std::int64_t>(unsignedValue);
        } else if (encodedValue.is_number_integer()) {
            encoded = encodedValue.get<std::int64_t>();
        } else {
            error = std::string(key) + " must be a signed integer nanosecond timestamp";
            return false;
        }
        if (!fromEpochNanoseconds(encoded, value)) {
            error = std::string(key) + " is outside the supported system_clock range";
            return false;
        }
        return true;
    }

    static bool readInt(const nlohmann::json& value, int& result) {
        if (value.is_number_unsigned()) {
            const auto unsignedValue = value.get<std::uint64_t>();
            if (unsignedValue > static_cast<std::uint64_t>((std::numeric_limits<int>::max)())) {
                return false;
            }
            result = static_cast<int>(unsignedValue);
            return true;
        }
        if (!value.is_number_integer()) return false;
        const auto signedValue = value.get<std::int64_t>();
        if (signedValue < static_cast<std::int64_t>((std::numeric_limits<int>::min)()) ||
            signedValue > static_cast<std::int64_t>((std::numeric_limits<int>::max)())) {
            return false;
        }
        result = static_cast<int>(signedValue);
        return true;
    }

    static bool decodeState(const nlohmann::json& root, State& state, std::string& error) {
        try {
            int schemaVersion = 0;
            if (!hasExactKeys(root, {"format", "locks", "modifications", "rateLimitAttempts",
                                     "recommendations", "schemaVersion", "snapshots"}) ||
                !root.at("format").is_string() ||
                root.at("format").get<std::string>() != "elizaos.character-modification-database" ||
                !readInt(root.at("schemaVersion"), schemaVersion) || schemaVersion != kSchemaVersion) {
                error = "unsupported or malformed character-modification persistence header";
                return false;
            }
            for (const char* collection : {"locks", "modifications", "rateLimitAttempts",
                                           "recommendations", "snapshots"}) {
                if (!root.at(collection).is_array() ||
                    root.at(collection).size() > kMaxRecordsPerCollection) {
                    error = std::string(collection) + " must be an array within the record limit";
                    return false;
                }
            }

            for (const auto& item : root.at("modifications")) {
                if (!hasExactKeys(item, {"agentId", "appliedAtUnixNanos", "createdAtUnixNanos",
                                         "diffXml", "id", "reasoning", "rolledBackAtUnixNanos",
                                         "versionNumber"}) ||
                    !item.at("agentId").is_string() || !item.at("diffXml").is_string() ||
                    !item.at("id").is_string() || !item.at("reasoning").is_string() ||
                    !item.at("versionNumber").is_number_integer()) {
                    error = "malformed character modification record";
                    return false;
                }
                CharacterModification value;
                value.agentId = item.at("agentId").get<std::string>();
                value.diffXml = item.at("diffXml").get<std::string>();
                value.id = item.at("id").get<std::string>();
                value.reasoning = item.at("reasoning").get<std::string>();
                if (!readInt(item.at("versionNumber"), value.versionNumber)) {
                    error = "character modification version is outside the supported integer range";
                    return false;
                }
                if (!readTimestamp(item, "appliedAtUnixNanos", value.appliedAt, error) ||
                    !readTimestamp(item, "createdAtUnixNanos", value.createdAt, error)) return false;
                if (!item.at("rolledBackAtUnixNanos").is_null()) {
                    std::chrono::system_clock::time_point rolledBack;
                    if (!readTimestamp(item, "rolledBackAtUnixNanos", rolledBack, error)) return false;
                    value.rolledBackAt = rolledBack;
                }
                state.modifications[value.agentId].push_back(std::move(value));
            }

            for (const auto& item : root.at("snapshots")) {
                if (!hasExactKeys(item, {"agentId", "characterData", "createdAtUnixNanos", "id",
                                         "versionNumber"}) ||
                    !item.at("agentId").is_string() || !item.at("characterData").is_string() ||
                    !item.at("id").is_string() || !item.at("versionNumber").is_number_integer()) {
                    error = "malformed character snapshot record";
                    return false;
                }
                CharacterSnapshot value;
                value.agentId = item.at("agentId").get<std::string>();
                value.characterData = item.at("characterData").get<std::string>();
                value.id = item.at("id").get<std::string>();
                if (!readInt(item.at("versionNumber"), value.versionNumber)) {
                    error = "character snapshot version is outside the supported integer range";
                    return false;
                }
                if (!readTimestamp(item, "createdAtUnixNanos", value.createdAt, error)) return false;
                state.snapshots[value.agentId].push_back(std::move(value));
            }

            for (const auto& item : root.at("rateLimitAttempts")) {
                if (!hasExactKeys(item, {"agentId", "attemptedAtUnixNanos", "successful"}) ||
                    !item.at("agentId").is_string() || !item.at("successful").is_boolean()) {
                    error = "malformed rate-limit attempt record";
                    return false;
                }
                std::chrono::system_clock::time_point attemptedAt;
                if (!readTimestamp(item, "attemptedAtUnixNanos", attemptedAt, error)) return false;
                state.rateLimitAttempts[item.at("agentId").get<std::string>()].push_back(
                    {attemptedAt, item.at("successful").get<bool>()});
            }

            for (const auto& item : root.at("locks")) {
                if (!hasExactKeys(item, {"agentId", "lockReason", "locked", "lockedBy"}) ||
                    !item.at("agentId").is_string() || !item.at("lockReason").is_string() ||
                    !item.at("locked").is_boolean() || !item.at("lockedBy").is_string()) {
                    error = "malformed lock record";
                    return false;
                }
                const auto agentId = item.at("agentId").get<std::string>();
                if (state.locks.count(agentId) != 0U) {
                    error = "duplicate lock agentId";
                    return false;
                }
                state.locks.emplace(agentId, LockStatus{item.at("locked").get<bool>(),
                                                        item.at("lockedBy").get<std::string>(),
                                                        item.at("lockReason").get<std::string>()});
            }

            for (const auto& item : root.at("recommendations")) {
                if (!hasExactKeys(item, {"agentId", "analysisResult", "conversationId",
                                         "createdAtUnixNanos", "id", "metadata", "processed",
                                         "processedAtUnixNanos", "recommendation", "roomId"}) ||
                    !item.at("agentId").is_string() || !item.at("analysisResult").is_string() ||
                    !item.at("conversationId").is_string() || !item.at("id").is_string() ||
                    !item.at("metadata").is_object() || !item.at("processed").is_boolean() ||
                    !item.at("recommendation").is_string() || !item.at("roomId").is_string()) {
                    error = "malformed evolution recommendation record";
                    return false;
                }
                EvolutionRecommendation value;
                value.agentId = item.at("agentId").get<std::string>();
                value.analysisResult = item.at("analysisResult").get<std::string>();
                value.conversationId = item.at("conversationId").get<std::string>();
                value.id = item.at("id").get<std::string>();
                value.processed = item.at("processed").get<bool>();
                value.recommendation = item.at("recommendation").get<std::string>();
                value.roomId = item.at("roomId").get<std::string>();
                if (!readTimestamp(item, "createdAtUnixNanos", value.createdAt, error)) return false;
                if (!item.at("processedAtUnixNanos").is_null()) {
                    std::chrono::system_clock::time_point processedAt;
                    if (!readTimestamp(item, "processedAtUnixNanos", processedAt, error)) return false;
                    value.processedAt = processedAt;
                }
                for (const auto& metadataEntry : item.at("metadata").items()) {
                    if (!metadataEntry.value().is_string()) {
                        error = "recommendation metadata values must be strings";
                        return false;
                    }
                    value.metadata.emplace(metadataEntry.key(), metadataEntry.value().get<std::string>());
                }
                state.recommendations[value.agentId].push_back(std::move(value));
            }

            return validateState(state, error);
        } catch (const std::exception& e) {
            error = std::string("malformed persistence data: ") + e.what();
            return false;
        }
    }

    bool writeStateFile(const State& state, std::string& error) const {
        if (!persistencePath_) {
            error = "persistence path is not configured";
            return false;
        }
        try {
            const std::string bytes = encodeState(state).dump(2) + "\n";
            if (bytes.size() > kMaxFileBytes) {
                error = "serialized persistence state exceeds the configured byte limit";
                return false;
            }

            const auto parent = persistencePath_->has_parent_path()
                ? persistencePath_->parent_path() : std::filesystem::path(".");
            std::error_code ec;
            std::filesystem::create_directories(parent, ec);
            if (ec) {
                error = "cannot create persistence directory: " + ec.message();
                return false;
            }

            auto temporary = *persistencePath_;
            temporary += ".tmp";
            std::filesystem::remove(temporary, ec);
            ec.clear();

            if (!writeAndSyncTemporaryFile(temporary, bytes, error)) {
                std::filesystem::remove(temporary, ec);
                return false;
            }

#if defined(_WIN32)
            if (!MoveFileExW(temporary.wstring().c_str(), persistencePath_->wstring().c_str(),
                             MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                error = "cannot atomically replace persistence file: Windows error " +
                        std::to_string(GetLastError());
                std::filesystem::remove(temporary, ec);
                return false;
            }
#else
            if (::rename(temporary.c_str(), persistencePath_->c_str()) != 0) {
                error = std::string("cannot atomically replace persistence file: ") +
                        std::strerror(errno);
                std::filesystem::remove(temporary, ec);
                return false;
            }
#endif
            return true;
        } catch (const std::exception& e) {
            error = std::string("failed to persist state: ") + e.what();
            return false;
        } catch (...) {
            error = "failed to persist state: unknown error";
            return false;
        }
    }

    static bool writeAndSyncTemporaryFile(const std::filesystem::path& path,
                                          const std::string& bytes,
                                          std::string& error) {
#if defined(_WIN32)
        const HANDLE file = CreateFileW(path.wstring().c_str(), GENERIC_WRITE, 0, nullptr,
                                        CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            error = "cannot create temporary persistence file: Windows error " +
                    std::to_string(GetLastError());
            return false;
        }
        std::size_t written = 0;
        while (written < bytes.size()) {
            const auto remaining = bytes.size() - written;
            const DWORD chunk = static_cast<DWORD>((std::min)(
                remaining, static_cast<std::size_t>((std::numeric_limits<DWORD>::max)())));
            DWORD count = 0;
            if (!WriteFile(file, bytes.data() + written, chunk, &count, nullptr) || count == 0U) {
                error = "cannot write temporary persistence file: Windows error " +
                        std::to_string(GetLastError());
                CloseHandle(file);
                return false;
            }
            written += count;
        }
        if (!FlushFileBuffers(file)) {
            error = "cannot sync temporary persistence file: Windows error " +
                    std::to_string(GetLastError());
            CloseHandle(file);
            return false;
        }
        if (!CloseHandle(file)) {
            error = "cannot close temporary persistence file: Windows error " +
                    std::to_string(GetLastError());
            return false;
        }
#else
        const int file = ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (file < 0) {
            error = std::string("cannot create temporary persistence file: ") +
                    std::strerror(errno);
            return false;
        }
        std::size_t written = 0;
        while (written < bytes.size()) {
            const auto count = ::write(file, bytes.data() + written, bytes.size() - written);
            if (count < 0) {
                if (errno == EINTR) continue;
                const int writeError = errno;
                (void)::close(file);
                error = std::string("cannot write temporary persistence file: ") +
                        std::strerror(writeError);
                return false;
            }
            if (count == 0) {
                (void)::close(file);
                error = "cannot write temporary persistence file: zero-byte write";
                return false;
            }
            written += static_cast<std::size_t>(count);
        }
        if (::fsync(file) != 0) {
            const int syncError = errno;
            (void)::close(file);
            error = std::string("cannot sync temporary persistence file: ") +
                    std::strerror(syncError);
            return false;
        }
        if (::close(file) != 0) {
            error = std::string("cannot close temporary persistence file: ") +
                    std::strerror(errno);
            return false;
        }
#endif
        return true;
    }
};

} // namespace elizaos
