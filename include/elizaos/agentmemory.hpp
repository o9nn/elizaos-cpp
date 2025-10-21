#pragma once

#include "elizaos/core.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
#include <functional>

namespace elizaos {

/**
 * AgentMemory - Memory management system for agent interactions
 * C++ implementation providing memory storage, retrieval, and search capabilities
 * 
 * Provides functionality similar to the TypeScript IDatabaseAdapter memory operations
 */

struct MemorySearchParams {
    std::optional<UUID> entityId;
    std::optional<UUID> agentId;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    int count = 10;
    bool unique = false;
    std::string tableName = "memories";
    int start = 0;
    int end = -1;
};

struct MemorySearchByEmbeddingParams {
    EmbeddingVector embedding;
    double matchThreshold = 0.7;
    int count = 10;
    bool unique = false;
    std::string tableName = "memories";
    std::optional<std::string> query;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::optional<UUID> entityId;
};

class AgentMemoryManager {
public:
    AgentMemoryManager();
    ~AgentMemoryManager() = default;
    
    // Initialization
    bool initialize();

    // Core memory operations
    UUID createMemory(std::shared_ptr<Memory> memory, const std::string& tableName = "memories", bool unique = false);
    UUID addMemory(std::shared_ptr<Memory> memory, const std::string& tableName = "memories"); // Convenience method
    std::shared_ptr<Memory> getMemoryById(const UUID& id);
    std::vector<std::shared_ptr<Memory>> getMemoriesByIds(const std::vector<UUID>& ids, const std::string& tableName = "memories");
    bool updateMemory(std::shared_ptr<Memory> memory);
    bool deleteMemory(const UUID& memoryId);
    void deleteManyMemories(const std::vector<UUID>& memoryIds);
    void deleteAllMemories(const UUID& roomId, const std::string& tableName = "memories");

    // Search and retrieval operations
    std::vector<std::shared_ptr<Memory>> getMemories(const MemorySearchParams& params);
    std::vector<std::shared_ptr<Memory>> getMemoriesByRoomIds(const std::vector<UUID>& roomIds, const std::string& tableName = "memories", int limit = -1);
    std::vector<std::shared_ptr<Memory>> searchMemories(const MemorySearchByEmbeddingParams& params);

    // Statistics and utilities
    int countMemories(const UUID& roomId, bool unique = false, const std::string& tableName = "memories");
    void clear(); // Clear all memories (for testing)

    // Thread-safe operations
    void enableThreadSafety(bool enable = true) { threadSafetyEnabled_ = enable; }

private:
    // Internal storage - using maps for different table types
    std::unordered_map<std::string, std::unordered_map<UUID, std::shared_ptr<Memory>>> memoryTables_;
    
    // Thread safety
    mutable std::mutex memoryMutex_;
    bool threadSafetyEnabled_ = true;
    
    // Helper methods
    bool matchesSearchCriteria(const Memory& memory, const MemorySearchParams& params);
    double calculateEmbeddingSimilarity(const EmbeddingVector& embedding1, const EmbeddingVector& embedding2);
    std::vector<std::shared_ptr<Memory>> getAllMemoriesFromTable(const std::string& tableName);
    
    // Thread-safe wrapper for operations
    template<typename F>
    auto withLock(F&& func) -> decltype(func()) {
        if (threadSafetyEnabled_) {
            std::lock_guard<std::mutex> lock(memoryMutex_);
            return func();
        } else {
            return func();
        }
    }
};

// Global memory manager instance
AgentMemoryManager& getGlobalMemoryManager();

// Convenience functions for common operations
namespace memory {
    UUID store(std::shared_ptr<Memory> memory, const std::string& tableName = "memories");
    std::shared_ptr<Memory> retrieve(const UUID& id);
    std::vector<std::shared_ptr<Memory>> search(const MemorySearchParams& params);
    std::vector<std::shared_ptr<Memory>> searchByEmbedding(const MemorySearchByEmbeddingParams& params);
    bool remove(const UUID& id);
    void clearAll();
}

} // namespace elizaos