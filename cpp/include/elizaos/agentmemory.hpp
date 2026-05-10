#pragma once

/**
 * ElizaOS C++ - AgentMemory Module
 *
 * Persistent memory management with embedding-based retrieval.
 * Provides AgentMemoryManager, MemorySearchParams, and convenience
 * functions in the `memory::` namespace.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Search parameter structures
// ============================================================================

struct MemorySearchParams {
    std::string           tableName  = "memories";
    std::optional<UUID>   entityId;
    std::optional<UUID>   agentId;
    std::optional<UUID>   roomId;
    std::optional<UUID>   worldId;
    int                   start  = 0;
    int                   end    = 0;
    int                   count  = 20;
};

struct MemorySearchByEmbeddingParams {
    std::string           tableName      = "memories";
    EmbeddingVector       embedding;
    std::optional<UUID>   entityId;
    std::optional<UUID>   roomId;
    double                matchThreshold = 0.0;
    int                   count          = 10;
};

// ============================================================================
// AgentMemoryManager
// ============================================================================

class AgentMemoryManager {
public:
    AgentMemoryManager();

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
    void enableThreadSafety(bool enable) { threadSafetyEnabled_ = enable; }
    std::vector<std::shared_ptr<Memory>> getAllMemoriesFromTable(const std::string& tableName);

private:
    bool matchesSearchCriteria(const Memory& memory, const MemorySearchParams& params);
    double calculateEmbeddingSimilarity(const EmbeddingVector& e1, const EmbeddingVector& e2);

    template<typename Fn>
    auto withLock(Fn&& fn) -> decltype(fn()) {
        if (threadSafetyEnabled_) {
            std::lock_guard<std::mutex> lock(mutex_);
            return fn();
        }
        return fn();
    }

    std::unordered_map<std::string,
        std::unordered_map<UUID, std::shared_ptr<Memory>>> memoryTables_;
    mutable std::mutex mutex_;
    bool threadSafetyEnabled_ = false;
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
}

} // namespace elizaos
