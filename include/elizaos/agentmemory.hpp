#pragma once

/**
 * ElizaOS C++ - AgentMemory Module
 *
 * Persistent memory management with embedding-based retrieval.
 * Provides AgentMemoryManager, MemorySearchParams, and convenience
 * functions in the `memory::` namespace.
 *
 * Hierarchical / Associative enhancements (structure-preserving port):
 * - Hierarchical memory indexing (episodic / semantic / procedural / working / sensory)
 * - Memory consolidation algorithms (sleep-like processing)
 * - Forgetting curves with importance-weighted decay (Ebbinghaus)
 * - Memory defragmentation and optimization
 * - Associative memory networks with spreading activation (Hebbian)
 *
 * This header preserves the original o9nn public API surface (the `unique`,
 * `query`, and `end = -1` search fields, the `memory::` convenience namespace,
 * and thread-safety-on-by-default behaviour) while adding the richer
 * hierarchical / associative subsystem.
 */

#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <queue>
#include <cmath>
#include <set>

namespace elizaos {

// ============================================================================
// Memory Types for Hierarchical Indexing
// ============================================================================

enum class HierarchicalMemoryType {
    EPISODIC,      // Personal experiences, events with temporal context
    SEMANTIC,      // Facts, concepts, general knowledge
    PROCEDURAL,    // Skills, procedures, how-to knowledge
    WORKING,       // Short-term, currently active memories
    SENSORY        // Very short-term perceptual memories
};

// ============================================================================
// Memory Importance and Decay
// ============================================================================

struct MemoryDecayParams {
    double baseDecayRate = 0.05;        // Base decay rate per day
    double importanceMultiplier = 1.0;  // How much importance affects decay
    double accessBoost = 0.2;           // How much each access boosts strength
    double consolidationBonus = 0.5;    // Bonus from consolidation
    double minStrength = 0.01;          // Minimum strength before forgetting
};

struct MemoryStrength {
    static constexpr double DEFAULT_INITIAL_STRENGTH = 1.0;
    static constexpr double DEFAULT_BASE_IMPORTANCE = 0.5;
    static constexpr double DEFAULT_EMOTIONAL_FACTOR = 0.5;

    double currentStrength = DEFAULT_INITIAL_STRENGTH;
    double baseImportance = DEFAULT_BASE_IMPORTANCE;
    double emotionalSalience = 0.0;
    int accessCount = 0;
    Timestamp lastAccessed;
    Timestamp lastConsolidated;
    bool isConsolidated = false;

    double getEffectiveDecayRate(const MemoryDecayParams& params) const {
        double importanceFactor = 1.0 - (baseImportance * params.importanceMultiplier);
        double emotionalFactor = 1.0 - (emotionalSalience * DEFAULT_EMOTIONAL_FACTOR);
        return params.baseDecayRate * importanceFactor * emotionalFactor;
    }

    void applyDecay(const MemoryDecayParams& params, double daysSinceLastAccess) {
        double stability = 1.0 / getEffectiveDecayRate(params);
        double retention = std::exp(-daysSinceLastAccess / stability);
        currentStrength *= retention;
        if (isConsolidated) {
            currentStrength = std::min(1.0, currentStrength * (1.0 + params.consolidationBonus));
        }
        currentStrength = std::max(params.minStrength, currentStrength);
    }

    void boostOnAccess(const MemoryDecayParams& params) {
        accessCount++;
        lastAccessed = std::chrono::system_clock::now();
        double boost = params.accessBoost / std::sqrt(static_cast<double>(accessCount));
        currentStrength = std::min(1.0, currentStrength + boost);
    }
};

// ============================================================================
// Associative Memory Link
// ============================================================================

struct AssociativeLink {
    UUID sourceMemoryId;
    UUID targetMemoryId;
    double associationStrength = 0.5;
    std::string linkType;
    Timestamp createdAt;
    int coActivationCount = 0;

    void strengthen(double amount = 0.1) {
        coActivationCount++;
        associationStrength = std::min(1.0, associationStrength + amount);
    }

    void weaken(double amount = 0.05) {
        associationStrength = std::max(0.0, associationStrength - amount);
    }
};

// ============================================================================
// Hierarchical Memory Index
// ============================================================================

struct HierarchicalIndex {
    std::unordered_map<HierarchicalMemoryType, std::set<UUID>> typeIndex;
    std::unordered_map<std::string, std::set<UUID>> conceptIndex;
    std::unordered_map<std::string, std::set<UUID>> entityIndex;
    std::unordered_map<std::string, std::set<UUID>> timeIndex;

    void addMemory(const UUID& memoryId, HierarchicalMemoryType type,
                   const std::vector<std::string>& concepts = {},
                   const std::string& entityRef = "",
                   const std::string& timeBucket = "") {
        typeIndex[type].insert(memoryId);
        for (const auto& concept_name : concepts) {
            conceptIndex[concept_name].insert(memoryId);
        }
        if (!entityRef.empty()) {
            entityIndex[entityRef].insert(memoryId);
        }
        if (!timeBucket.empty()) {
            timeIndex[timeBucket].insert(memoryId);
        }
    }

    void removeMemory(const UUID& memoryId) {
        for (auto& [type, ids] : typeIndex) { ids.erase(memoryId); }
        for (auto& [cpt_key, ids] : conceptIndex) { ids.erase(memoryId); }
        for (auto& [entity, ids] : entityIndex) { ids.erase(memoryId); }
        for (auto& [time, ids] : timeIndex) { ids.erase(memoryId); }
    }

    std::set<UUID> getByType(HierarchicalMemoryType type) const {
        auto it = typeIndex.find(type);
        return it != typeIndex.end() ? it->second : std::set<UUID>{};
    }

    std::set<UUID> getByConcept(const std::string& cpt_name) const {
        auto it = conceptIndex.find(cpt_name);
        return it != conceptIndex.end() ? it->second : std::set<UUID>{};
    }
};

// ============================================================================
// Memory Consolidation Engine
// ============================================================================

class MemoryConsolidationEngine {
public:
    MemoryConsolidationEngine() = default;

    struct ConsolidationParams {
        double consolidationThreshold = 0.7;
        int minAccessCount = 3;
        double similarityMergeThreshold = 0.9;
        int maxConsolidationsPerCycle = 100;
    };

    struct ConsolidationResult {
        int memoriesProcessed = 0;
        int memoriesConsolidated = 0;
        int memoriesMerged = 0;
        int memoriesForgotten = 0;
        double averageStrengthBefore = 0.0;
        double averageStrengthAfter = 0.0;
    };

    void setParams(const ConsolidationParams& params) { params_ = params; }
    ConsolidationParams getParams() const { return params_; }

    ConsolidationResult runConsolidationCycle(
        std::vector<std::shared_ptr<Memory>>& memories,
        std::unordered_map<UUID, MemoryStrength>& strengthMap,
        const MemoryDecayParams& decayParams);

    std::vector<std::pair<UUID, UUID>> findMergeCandidates(
        const std::vector<std::shared_ptr<Memory>>& memories,
        double similarityThreshold = 0.9);

    std::shared_ptr<Memory> mergeMemories(
        std::shared_ptr<Memory> m1,
        std::shared_ptr<Memory> m2,
        const MemoryStrength& s1,
        const MemoryStrength& s2);

private:
    ConsolidationParams params_;
    double calculateMemorySimilarity(const Memory& m1, const Memory& m2);
};

// ============================================================================
// Search parameter structures
//
// Preserves the original o9nn fields (`unique`, `query`, `end = -1`) while
// adding the hierarchical / associative search options.
// ============================================================================

struct MemorySearchParams {
    std::string           tableName  = "memories";
    std::optional<UUID>   entityId;
    std::optional<UUID>   agentId;
    std::optional<UUID>   roomId;
    std::optional<UUID>   worldId;
    int                   start  = 0;
    int                   end    = -1;
    int                   count  = 10;
    bool                  unique = false;

    std::optional<HierarchicalMemoryType> memoryType;
    std::optional<double> minStrength;
    std::optional<double> minImportance;
    std::vector<std::string> concepts;
    bool includeDecayed = false;
    bool sortByStrength = false;
};

struct MemorySearchByEmbeddingParams {
    std::string           tableName      = "memories";
    EmbeddingVector       embedding;
    std::optional<UUID>   entityId;
    std::optional<UUID>   roomId;
    std::optional<UUID>   worldId;
    std::optional<std::string> query;
    double                matchThreshold = 0.7;
    int                   count          = 10;
    bool                  unique         = false;

    bool useAssociativeSpreading = false;
    int associativeDepth = 2;
    double spreadingDecay = 0.5;
};

// ============================================================================
// AgentMemoryManager - Enhanced
// ============================================================================

class AgentMemoryManager {
public:
    AgentMemoryManager();
    ~AgentMemoryManager() = default;

    // CRUD operations
    UUID                                  createMemory(std::shared_ptr<Memory> memory,
                                                       const std::string& tableName = "memories",
                                                       bool unique = false);
    std::shared_ptr<Memory>               getMemoryById(const UUID& id);
    std::vector<std::shared_ptr<Memory>>  getMemoriesByIds(const std::vector<UUID>& ids,
                                                            const std::string& tableName = "memories");
    bool                                  updateMemory(std::shared_ptr<Memory> memory);
    bool                                  deleteMemory(const UUID& memoryId);
    void                                  deleteManyMemories(const std::vector<UUID>& memoryIds);
    void                                  deleteAllMemories(const UUID& roomId,
                                                            const std::string& tableName = "memories");

    // Query operations
    std::vector<std::shared_ptr<Memory>>  getMemories(const MemorySearchParams& params);
    std::vector<std::shared_ptr<Memory>>  getMemoriesByRoomIds(const std::vector<UUID>& roomIds,
                                                               const std::string& tableName = "memories",
                                                               int limit = -1);
    std::vector<std::shared_ptr<Memory>>  searchMemories(const MemorySearchByEmbeddingParams& params);
    int                                   countMemories(const UUID& roomId,
                                                        bool unique = false,
                                                        const std::string& tableName = "memories");

    // Utility
    void clear();
    void enableThreadSafety(bool enable = true) { threadSafetyEnabled_ = enable; }
    std::vector<std::shared_ptr<Memory>> getAllMemoriesFromTable(const std::string& tableName);

    // =========================================================================
    // Hierarchical Memory Operations
    // =========================================================================

    void indexMemory(const UUID& memoryId, HierarchicalMemoryType type,
                     const std::vector<std::string>& concepts = {});
    std::vector<std::shared_ptr<Memory>> getMemoriesByType(HierarchicalMemoryType type);
    std::vector<std::shared_ptr<Memory>> getMemoriesByConcept(const std::string& cpt_name);
    void reindexAllMemories();

    void setMemoryStrength(const UUID& memoryId, const MemoryStrength& strength);
    MemoryStrength getMemoryStrength(const UUID& memoryId) const;
    void boostMemoryOnAccess(const UUID& memoryId);
    void applyDecayToAllMemories(double daysSinceLastCycle);
    std::vector<UUID> getDecayedMemories(double threshold = 0.1);

    void setConsolidationParams(const MemoryConsolidationEngine::ConsolidationParams& params);
    MemoryConsolidationEngine::ConsolidationResult runConsolidation();
    void setDecayParams(const MemoryDecayParams& params);

    void createAssociation(const UUID& sourceId, const UUID& targetId,
                           const std::string& linkType = "semantic", double strength = 0.5);
    void strengthenAssociation(const UUID& sourceId, const UUID& targetId, double amount = 0.1);
    void weakenAssociation(const UUID& sourceId, const UUID& targetId, double amount = 0.05);
    std::vector<AssociativeLink> getAssociations(const UUID& memoryId) const;
    std::vector<UUID> spreadActivation(const UUID& startId, int depth = 2, double decayFactor = 0.5);

    void defragmentMemories();
    size_t getMemoryCount() const;
    size_t getFragmentedCount() const;

    struct MemoryStatistics {
        size_t totalMemories = 0;
        size_t episodicMemories = 0;
        size_t semanticMemories = 0;
        size_t proceduralMemories = 0;
        size_t workingMemories = 0;
        double averageStrength = 0.0;
        double averageAge = 0.0;
        size_t totalAssociations = 0;
        size_t consolidatedMemories = 0;
        size_t decayedMemories = 0;
    };
    MemoryStatistics getStatistics() const;

private:
    bool matchesSearchCriteria(const Memory& memory, const MemorySearchParams& params);
    double calculateEmbeddingSimilarity(const EmbeddingVector& e1, const EmbeddingVector& e2);
    // Lock-free memory lookup used by methods that already hold memoryMutex_,
    // to avoid self-deadlock on the non-recursive mutex.
    std::shared_ptr<Memory> findMemoryByIdUnlocked(const UUID& id) const;

    template<typename Fn>
    auto withLock(Fn&& fn) -> decltype(fn()) {
        if (threadSafetyEnabled_) {
            std::lock_guard<std::mutex> lock(memoryMutex_);
            return fn();
        }
        return fn();
    }

    template<typename Fn>
    auto withLock(Fn&& fn) const -> decltype(fn()) {
        if (threadSafetyEnabled_) {
            std::lock_guard<std::mutex> lock(memoryMutex_);
            return fn();
        }
        return fn();
    }

    std::unordered_map<std::string,
        std::unordered_map<UUID, std::shared_ptr<Memory>>> memoryTables_;
    mutable std::mutex memoryMutex_;
    bool threadSafetyEnabled_ = true;

    HierarchicalIndex hierarchicalIndex_;
    std::unordered_map<UUID, MemoryStrength> memoryStrengths_;
    std::unordered_map<UUID, std::vector<AssociativeLink>> associativeNetwork_;
    MemoryConsolidationEngine consolidationEngine_;
    MemoryDecayParams decayParams_;
    mutable std::mutex strengthMutex_;
    mutable std::mutex associationMutex_;
};

// ============================================================================
// Global singleton and convenience namespace
// ============================================================================

AgentMemoryManager& getGlobalMemoryManager();

namespace memory {
    UUID                                 store(std::shared_ptr<Memory> memory,
                                               const std::string& tableName = "memories");
    std::shared_ptr<Memory>              retrieve(const UUID& id);
    std::vector<std::shared_ptr<Memory>> search(const MemorySearchParams& params);
    std::vector<std::shared_ptr<Memory>> searchByEmbedding(const MemorySearchByEmbeddingParams& params);
    bool                                 remove(const UUID& id);
    void                                 clearAll();

    void consolidate();
    void applyDecay(double days = 1.0);
    std::vector<UUID> getAssociated(const UUID& memoryId, int depth = 2);
    void createLink(const UUID& source, const UUID& target, const std::string& type = "semantic");
    AgentMemoryManager::MemoryStatistics getStats();
}

} // namespace elizaos
