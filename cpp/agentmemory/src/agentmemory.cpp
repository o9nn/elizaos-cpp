#include "elizaos/agentmemory.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>

namespace elizaos {

// AgentMemoryManager Implementation
AgentMemoryManager::AgentMemoryManager() {
    // Initialize with default memories table
    memoryTables_["memories"] = {};
}

bool AgentMemoryManager::initialize() {
    // Simple initialization - ensure default table exists and return true
    // This method can be extended later for more complex initialization
    memoryTables_["memories"] = {};
    return true;
}

UUID AgentMemoryManager::createMemory(std::shared_ptr<Memory> memory, const std::string& tableName, bool unique) {
    return withLock([&]() -> UUID {
        auto& table = memoryTables_[tableName];
        
        if (unique) {
            // Check for duplicate content in the same room/entity context
            for (const auto& [id, existingMemory] : table) {
                if (existingMemory->getContent() == memory->getContent() &&
                    existingMemory->getEntityId() == memory->getEntityId() &&
                    existingMemory->getRoomId() == memory->getRoomId()) {
                    return existingMemory->getId(); // Return existing ID
                }
            }
        }
        
        table[memory->getId()] = memory;
        return memory->getId();
    });
}

UUID AgentMemoryManager::addMemory(std::shared_ptr<Memory> memory, const std::string& tableName) {
    // Convenience method that calls createMemory with unique=false by default
    return createMemory(memory, tableName, false);
}

std::shared_ptr<Memory> AgentMemoryManager::getMemoryById(const UUID& id) {
    return withLock([&]() -> std::shared_ptr<Memory> {
        // Search across all tables
        for (const auto& [tableName, table] : memoryTables_) {
            auto it = table.find(id);
            if (it != table.end()) {
                return it->second;
            }
        }
        return nullptr;
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemoriesByIds(const std::vector<UUID>& ids, const std::string& tableName) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        
        if (memoryTables_.find(tableName) == memoryTables_.end()) {
            return result;
        }
        
        const auto& table = memoryTables_[tableName];
        for (const auto& id : ids) {
            auto it = table.find(id);
            if (it != table.end()) {
                result.push_back(it->second);
            }
        }
        
        return result;
    });
}

bool AgentMemoryManager::updateMemory(std::shared_ptr<Memory> memory) {
    return withLock([&]() -> bool {
        // Find the memory across all tables and update it
        for (auto& [tableName, table] : memoryTables_) {
            auto it = table.find(memory->getId());
            if (it != table.end()) {
                it->second = memory;
                return true;
            }
        }
        return false;
    });
}

bool AgentMemoryManager::deleteMemory(const UUID& memoryId) {
    return withLock([&]() -> bool {
        // Find and delete from all tables
        for (auto& [tableName, table] : memoryTables_) {
            auto it = table.find(memoryId);
            if (it != table.end()) {
                table.erase(it);
                return true;
            }
        }
        return false;
    });
}

void AgentMemoryManager::deleteManyMemories(const std::vector<UUID>& memoryIds) {
    withLock([&]() {
        for (const auto& id : memoryIds) {
            // Find and delete from all tables (without acquiring lock again)
            for (auto& [tableName, table] : memoryTables_) {
                auto it = table.find(id);
                if (it != table.end()) {
                    table.erase(it);
                    break; // Found and deleted, no need to search other tables
                }
            }
        }
    });
}

void AgentMemoryManager::deleteAllMemories(const UUID& roomId, const std::string& tableName) {
    withLock([&]() {
        if (memoryTables_.find(tableName) == memoryTables_.end()) {
            return;
        }
        
        auto& table = memoryTables_[tableName];
        auto it = table.begin();
        while (it != table.end()) {
            if (it->second->getRoomId() == roomId) {
                it = table.erase(it);
            } else {
                ++it;
            }
        }
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemories(const MemorySearchParams& params) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        
        if (memoryTables_.find(params.tableName) == memoryTables_.end()) {
            return result;
        }
        
        const auto& table = memoryTables_[params.tableName];
        
        // Collect matching memories
        for (const auto& [id, memory] : table) {
            if (matchesSearchCriteria(*memory, params)) {
                result.push_back(memory);
            }
        }
        
        // Sort by creation time (newest first)
        std::sort(result.begin(), result.end(), 
                  [](const std::shared_ptr<Memory>& a, const std::shared_ptr<Memory>& b) {
                      return a->getCreatedAt() > b->getCreatedAt();
                  });
        
        // Apply pagination
        int startIdx = params.start;
        int endIdx = params.end > 0 ? std::min(params.end, static_cast<int>(result.size())) : static_cast<int>(result.size());
        int count = params.count > 0 ? params.count : endIdx - startIdx;
        
        if (startIdx >= static_cast<int>(result.size())) {
            return {};
        }
        
        endIdx = std::min(startIdx + count, endIdx);
        
        return std::vector<std::shared_ptr<Memory>>(result.begin() + startIdx, result.begin() + endIdx);
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemoriesByRoomIds(const std::vector<UUID>& roomIds, const std::string& tableName, int limit) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        
        if (memoryTables_.find(tableName) == memoryTables_.end()) {
            return result;
        }
        
        const auto& table = memoryTables_[tableName];
        
        for (const auto& [id, memory] : table) {
            if (std::find(roomIds.begin(), roomIds.end(), memory->getRoomId()) != roomIds.end()) {
                result.push_back(memory);
            }
        }
        
        // Sort by creation time (newest first)
        std::sort(result.begin(), result.end(), 
                  [](const std::shared_ptr<Memory>& a, const std::shared_ptr<Memory>& b) {
                      return a->getCreatedAt() > b->getCreatedAt();
                  });
        
        if (limit > 0 && static_cast<int>(result.size()) > limit) {
            result.resize(limit);
        }
        
        return result;
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::searchMemories(const MemorySearchByEmbeddingParams& params) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::pair<std::shared_ptr<Memory>, double>> candidates;
        
        if (memoryTables_.find(params.tableName) == memoryTables_.end()) {
            return {};
        }
        
        const auto& table = memoryTables_[params.tableName];
        
        for (const auto& [id, memory] : table) {
            // Apply basic filters
            if (params.entityId && memory->getEntityId() != *params.entityId) continue;
            if (params.roomId && memory->getRoomId() != *params.roomId) continue;
            
            // Calculate embedding similarity if available
            if (memory->getEmbedding()) {
                double similarity = calculateEmbeddingSimilarity(params.embedding, *memory->getEmbedding());
                
                if (similarity >= params.matchThreshold) {
                    memory->setSimilarity(similarity);
                    candidates.emplace_back(memory, similarity);
                }
            }
        }
        
        // Sort by similarity (highest first)
        std::sort(candidates.begin(), candidates.end(), 
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Extract memories and apply limit
        std::vector<std::shared_ptr<Memory>> result;
        int limit = params.count > 0 ? params.count : static_cast<int>(candidates.size());
        
        for (int i = 0; i < std::min(limit, static_cast<int>(candidates.size())); ++i) {
            result.push_back(candidates[i].first);
        }
        
        return result;
    });
}

int AgentMemoryManager::countMemories(const UUID& roomId, bool unique, const std::string& tableName) {
    return withLock([&]() -> int {
        if (memoryTables_.find(tableName) == memoryTables_.end()) {
            return 0;
        }
        
        const auto& table = memoryTables_[tableName];
        
        if (unique) {
            std::set<std::string> uniqueContents;
            for (const auto& [id, memory] : table) {
                if (memory->getRoomId() == roomId) {
                    uniqueContents.insert(memory->getContent());
                }
            }
            return static_cast<int>(uniqueContents.size());
        } else {
            int count = 0;
            for (const auto& [id, memory] : table) {
                if (memory->getRoomId() == roomId) {
                    count++;
                }
            }
            return count;
        }
    });
}

void AgentMemoryManager::clear() {
    withLock([&]() {
        memoryTables_.clear();
        memoryTables_["memories"] = {}; // Re-initialize default table
    });
}

bool AgentMemoryManager::matchesSearchCriteria(const Memory& memory, const MemorySearchParams& params) {
    if (params.entityId && memory.getEntityId() != *params.entityId) return false;
    if (params.agentId && memory.getAgentId() != *params.agentId) return false;
    if (params.roomId && memory.getRoomId() != *params.roomId) return false;
    // Note: worldId not directly supported in current Memory class
    
    return true;
}

double AgentMemoryManager::calculateEmbeddingSimilarity(const EmbeddingVector& embedding1, const EmbeddingVector& embedding2) {
    if (embedding1.size() != embedding2.size()) {
        return 0.0;
    }
    
    // Calculate cosine similarity
    double dotProduct = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;
    
    for (size_t i = 0; i < embedding1.size(); ++i) {
        dotProduct += embedding1[i] * embedding2[i];
        norm1 += embedding1[i] * embedding1[i];
        norm2 += embedding2[i] * embedding2[i];
    }
    
    if (norm1 == 0.0 || norm2 == 0.0) {
        return 0.0;
    }
    
    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getAllMemoriesFromTable(const std::string& tableName) {
    std::vector<std::shared_ptr<Memory>> result;
    
    if (memoryTables_.find(tableName) == memoryTables_.end()) {
        return result;
    }
    
    const auto& table = memoryTables_[tableName];
    for (const auto& [id, memory] : table) {
        result.push_back(memory);
    }
    
    return result;
}

// Global memory manager instance
AgentMemoryManager& getGlobalMemoryManager() {
    static AgentMemoryManager instance;
    return instance;
}

// Convenience functions
namespace memory {
    UUID store(std::shared_ptr<Memory> memory, const std::string& tableName) {
        return getGlobalMemoryManager().createMemory(memory, tableName);
    }
    
    std::shared_ptr<Memory> retrieve(const UUID& id) {
        return getGlobalMemoryManager().getMemoryById(id);
    }
    
    std::vector<std::shared_ptr<Memory>> search(const MemorySearchParams& params) {
        return getGlobalMemoryManager().getMemories(params);
    }
    
    std::vector<std::shared_ptr<Memory>> searchByEmbedding(const MemorySearchByEmbeddingParams& params) {
        return getGlobalMemoryManager().searchMemories(params);
    }
    
    bool remove(const UUID& id) {
        return getGlobalMemoryManager().deleteMemory(id);
    }
    
    void clearAll() {
        getGlobalMemoryManager().clear();
    }
}

} // namespace elizaos
