#include "elizaos/agentmemory.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <set>
#include <queue>
#include <numeric>

namespace elizaos {

// AgentMemoryManager Implementation
AgentMemoryManager::AgentMemoryManager() {
    // Initialize with default memories table
    memoryTables_["memories"] = {};
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
        
        // Initialize memory strength using configurable defaults
        MemoryStrength strength;
        strength.currentStrength = MemoryStrength::DEFAULT_INITIAL_STRENGTH;
        strength.baseImportance = MemoryStrength::DEFAULT_BASE_IMPORTANCE;
        strength.lastAccessed = std::chrono::system_clock::now();
        strength.lastConsolidated = std::chrono::system_clock::now();
        memoryStrengths_[memory->getId()] = strength;
        
        return memory->getId();
    });
}

std::shared_ptr<Memory> AgentMemoryManager::findMemoryByIdUnlocked(const UUID& id) const {
    // Caller must already hold memoryMutex_ (or thread-safety disabled).
    for (const auto& [tableName, table] : memoryTables_) {
        auto it = table.find(id);
        if (it != table.end()) {
            return it->second;
        }
    }
    return nullptr;
}

std::shared_ptr<Memory> AgentMemoryManager::getMemoryById(const UUID& id) {
    return withLock([&]() -> std::shared_ptr<Memory> {
        return findMemoryByIdUnlocked(id);
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
        // Reset the full memory state so that strengths, associations and the
        // hierarchical index do not leak across logical sessions (also fixes
        // cross-test state leakage in the statistics aggregation path).
        hierarchicalIndex_ = HierarchicalIndex{};
    });
    {
        std::lock_guard<std::mutex> lock(strengthMutex_);
        memoryStrengths_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(associationMutex_);
        associativeNetwork_.clear();
    }
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
    
    // Phase 1.2: Enhanced memory operations
    void consolidate() {
        getGlobalMemoryManager().runConsolidation();
    }
    
    void applyDecay(double days) {
        getGlobalMemoryManager().applyDecayToAllMemories(days);
    }
    
    std::vector<UUID> getAssociated(const UUID& memoryId, int depth) {
        return getGlobalMemoryManager().spreadActivation(memoryId, depth);
    }
    
    void createLink(const UUID& source, const UUID& target, const std::string& type) {
        getGlobalMemoryManager().createAssociation(source, target, type);
    }
    
    AgentMemoryManager::MemoryStatistics getStats() {
        return getGlobalMemoryManager().getStatistics();
    }
}

// ============================================================================
// Phase 1.2: Hierarchical Memory Operations Implementation
// ============================================================================

void AgentMemoryManager::indexMemory(const UUID& memoryId, HierarchicalMemoryType type,
                                     const std::vector<std::string>& concepts) {
    withLock([&]() {
        hierarchicalIndex_.addMemory(memoryId, type, concepts);
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemoriesByType(HierarchicalMemoryType type) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        auto ids = hierarchicalIndex_.getByType(type);

        // NOTE: resolve IDs lock-free here. Calling getMemoryById() (which
        // re-acquires withLock/memoryMutex_) would self-deadlock on the
        // non-recursive mutex when thread-safety is enabled.
        for (const auto& id : ids) {
            auto memory = findMemoryByIdUnlocked(id);
            if (memory) {
                result.push_back(memory);
            }
        }

        return result;
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemoriesByConcept(const std::string& cpt_name) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        auto ids = hierarchicalIndex_.getByConcept(cpt_name);

        // Resolve lock-free (see getMemoriesByType note on deadlock avoidance).
        for (const auto& id : ids) {
            auto memory = findMemoryByIdUnlocked(id);
            if (memory) {
                result.push_back(memory);
            }
        }

        return result;
    });
}

void AgentMemoryManager::reindexAllMemories() {
    withLock([&]() {
        // Clear existing index
        hierarchicalIndex_ = HierarchicalIndex{};
        
        // Re-index all memories with default type (semantic)
        for (const auto& [tableName, table] : memoryTables_) {
            for (const auto& [id, memory] : table) {
                hierarchicalIndex_.addMemory(id, HierarchicalMemoryType::SEMANTIC);
            }
        }
    });
}

// ============================================================================
// Phase 1.2: Memory Strength and Decay Implementation
// ============================================================================

void AgentMemoryManager::setMemoryStrength(const UUID& memoryId, const MemoryStrength& strength) {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    memoryStrengths_[memoryId] = strength;
}

MemoryStrength AgentMemoryManager::getMemoryStrength(const UUID& memoryId) const {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    auto it = memoryStrengths_.find(memoryId);
    if (it != memoryStrengths_.end()) {
        return it->second;
    }
    return MemoryStrength{}; // Return default strength
}

void AgentMemoryManager::boostMemoryOnAccess(const UUID& memoryId) {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    auto it = memoryStrengths_.find(memoryId);
    if (it != memoryStrengths_.end()) {
        it->second.boostOnAccess(decayParams_);
    }
}

void AgentMemoryManager::applyDecayToAllMemories(double daysSinceLastCycle) {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    
    for (auto& [id, strength] : memoryStrengths_) {
        strength.applyDecay(decayParams_, daysSinceLastCycle);
    }
}

std::vector<UUID> AgentMemoryManager::getDecayedMemories(double threshold) {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    std::vector<UUID> decayed;
    
    for (const auto& [id, strength] : memoryStrengths_) {
        if (strength.currentStrength < threshold) {
            decayed.push_back(id);
        }
    }
    
    return decayed;
}

// ============================================================================
// Phase 1.2: Memory Consolidation Implementation
// ============================================================================

void AgentMemoryManager::setConsolidationParams(const MemoryConsolidationEngine::ConsolidationParams& params) {
    consolidationEngine_.setParams(params);
}

MemoryConsolidationEngine::ConsolidationResult AgentMemoryManager::runConsolidation() {
    return withLock([&]() -> MemoryConsolidationEngine::ConsolidationResult {
        // Collect all memories from all tables
        std::vector<std::shared_ptr<Memory>> allMemories;
        for (auto& [tableName, table] : memoryTables_) {
            for (auto& [id, memory] : table) {
                allMemories.push_back(memory);
            }
        }
        
        return consolidationEngine_.runConsolidationCycle(allMemories, memoryStrengths_, decayParams_);
    });
}

void AgentMemoryManager::setDecayParams(const MemoryDecayParams& params) {
    decayParams_ = params;
}

MemoryConsolidationEngine::ConsolidationResult MemoryConsolidationEngine::runConsolidationCycle(
    std::vector<std::shared_ptr<Memory>>& memories,
    std::unordered_map<UUID, MemoryStrength>& strengthMap,
    const MemoryDecayParams& decayParams) {
    
    ConsolidationResult result;
    auto now = std::chrono::system_clock::now();
    
    // Calculate average strength before
    double totalStrengthBefore = 0.0;
    for (const auto& [id, strength] : strengthMap) {
        totalStrengthBefore += strength.currentStrength;
    }
    result.averageStrengthBefore = strengthMap.empty() ? 0.0 : totalStrengthBefore / strengthMap.size();
    
    // Process memories that meet consolidation criteria
    int processed = 0;
    for (auto& memory : memories) {
        if (processed >= params_.maxConsolidationsPerCycle) break;
        
        auto it = strengthMap.find(memory->getId());
        if (it == strengthMap.end()) continue;
        
        MemoryStrength& strength = it->second;
        
        // Check if memory should be consolidated
        if (!strength.isConsolidated && 
            strength.baseImportance >= params_.consolidationThreshold &&
            strength.accessCount >= params_.minAccessCount) {
            
            strength.isConsolidated = true;
            strength.lastConsolidated = now;
            strength.currentStrength = std::min(1.0, strength.currentStrength + decayParams.consolidationBonus);
            result.memoriesConsolidated++;
        }
        
        // Check if memory should be forgotten
        if (strength.currentStrength < decayParams.minStrength) {
            result.memoriesForgotten++;
        }
        
        result.memoriesProcessed++;
        processed++;
    }
    
    // Find and potentially merge similar memories
    auto mergeCandidates = findMergeCandidates(memories, params_.similarityMergeThreshold);
    result.memoriesMerged = static_cast<int>(mergeCandidates.size());
    
    // Calculate average strength after
    double totalStrengthAfter = 0.0;
    for (const auto& [id, strength] : strengthMap) {
        totalStrengthAfter += strength.currentStrength;
    }
    result.averageStrengthAfter = strengthMap.empty() ? 0.0 : totalStrengthAfter / strengthMap.size();
    
    return result;
}

std::vector<std::pair<UUID, UUID>> MemoryConsolidationEngine::findMergeCandidates(
    const std::vector<std::shared_ptr<Memory>>& memories,
    double similarityThreshold) {
    
    std::vector<std::pair<UUID, UUID>> candidates;
    
    for (size_t i = 0; i < memories.size(); ++i) {
        for (size_t j = i + 1; j < memories.size(); ++j) {
            double similarity = calculateMemorySimilarity(*memories[i], *memories[j]);
            if (similarity >= similarityThreshold) {
                candidates.emplace_back(memories[i]->getId(), memories[j]->getId());
            }
        }
    }
    
    return candidates;
}

std::shared_ptr<Memory> MemoryConsolidationEngine::mergeMemories(
    std::shared_ptr<Memory> m1,
    std::shared_ptr<Memory> m2,
    const MemoryStrength& s1,
    const MemoryStrength& s2) {
    
    // Create merged memory with combined content.
    // o9nn's Memory has no default constructor / setContent(); construct
    // directly from the stronger source memory's fields instead.
    const std::shared_ptr<Memory>& stronger =
        (s1.currentStrength >= s2.currentStrength) ? m1 : m2;
    auto merged = std::make_shared<Memory>(
        stronger->getId(),
        stronger->getContent(),
        stronger->getEntityId(),
        stronger->getAgentId());
    merged->setRoomId(stronger->getRoomId());

    // Combine embeddings (average)
    if (m1->getEmbedding() && m2->getEmbedding()) {
        EmbeddingVector avgEmbedding;
        const auto& e1 = *m1->getEmbedding();
        const auto& e2 = *m2->getEmbedding();
        
        for (size_t i = 0; i < std::min(e1.size(), e2.size()); ++i) {
            avgEmbedding.push_back((e1[i] + e2[i]) / 2.0f);
        }
        merged->setEmbedding(avgEmbedding);
    }
    
    return merged;
}

double MemoryConsolidationEngine::calculateMemorySimilarity(const Memory& m1, const Memory& m2) {
    // Use embedding similarity if available
    if (m1.getEmbedding() && m2.getEmbedding()) {
        const auto& e1 = *m1.getEmbedding();
        const auto& e2 = *m2.getEmbedding();
        
        if (e1.size() != e2.size()) return 0.0;
        
        double dotProduct = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;
        
        for (size_t i = 0; i < e1.size(); ++i) {
            dotProduct += e1[i] * e2[i];
            norm1 += e1[i] * e1[i];
            norm2 += e2[i] * e2[i];
        }
        
        if (norm1 == 0.0 || norm2 == 0.0) return 0.0;
        return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    }
    
    // Fallback to simple content comparison
    const std::string& c1 = m1.getContent();
    const std::string& c2 = m2.getContent();
    
    if (c1.empty() || c2.empty()) return 0.0;
    if (c1 == c2) return 1.0;
    
    // Simple Jaccard-like similarity on words
    std::set<std::string> words1, words2;
    // Split and compare (simplified)
    return 0.0;
}

// ============================================================================
// Phase 1.2: Associative Memory Network Implementation
// ============================================================================

void AgentMemoryManager::createAssociation(const UUID& sourceId, const UUID& targetId,
                                           const std::string& linkType, double strength) {
    std::lock_guard<std::mutex> lock(associationMutex_);
    
    AssociativeLink link;
    link.sourceMemoryId = sourceId;
    link.targetMemoryId = targetId;
    link.linkType = linkType;
    link.associationStrength = strength;
    link.createdAt = std::chrono::system_clock::now();
    
    associativeNetwork_[sourceId].push_back(link);
    
    // Create reverse link as well (bidirectional association)
    AssociativeLink reverseLink = link;
    reverseLink.sourceMemoryId = targetId;
    reverseLink.targetMemoryId = sourceId;
    associativeNetwork_[targetId].push_back(reverseLink);
}

void AgentMemoryManager::strengthenAssociation(const UUID& sourceId, const UUID& targetId, double amount) {
    std::lock_guard<std::mutex> lock(associationMutex_);
    
    auto it = associativeNetwork_.find(sourceId);
    if (it != associativeNetwork_.end()) {
        for (auto& link : it->second) {
            if (link.targetMemoryId == targetId) {
                link.strengthen(amount);
                break;
            }
        }
    }
}

void AgentMemoryManager::weakenAssociation(const UUID& sourceId, const UUID& targetId, double amount) {
    std::lock_guard<std::mutex> lock(associationMutex_);
    
    auto it = associativeNetwork_.find(sourceId);
    if (it != associativeNetwork_.end()) {
        for (auto& link : it->second) {
            if (link.targetMemoryId == targetId) {
                link.weaken(amount);
                break;
            }
        }
    }
}

std::vector<AssociativeLink> AgentMemoryManager::getAssociations(const UUID& memoryId) const {
    std::lock_guard<std::mutex> lock(associationMutex_);
    
    auto it = associativeNetwork_.find(memoryId);
    if (it != associativeNetwork_.end()) {
        return it->second;
    }
    return {};
}

std::vector<UUID> AgentMemoryManager::spreadActivation(const UUID& startId, int depth, double decayFactor) {
    std::lock_guard<std::mutex> lock(associationMutex_);
    
    std::vector<UUID> activated;
    std::set<UUID> visited;
    std::queue<std::pair<UUID, int>> queue;
    std::unordered_map<UUID, double> activationLevels;
    
    queue.push({startId, 0});
    visited.insert(startId);
    activationLevels[startId] = 1.0;
    
    while (!queue.empty()) {
        auto [currentId, currentDepth] = queue.front();
        queue.pop();
        
        if (currentDepth >= depth) continue;
        
        auto it = associativeNetwork_.find(currentId);
        if (it == associativeNetwork_.end()) continue;
        
        double currentActivation = activationLevels[currentId];
        
        for (const auto& link : it->second) {
            if (visited.find(link.targetMemoryId) == visited.end()) {
                visited.insert(link.targetMemoryId);
                
                // Activation decays with distance and link strength
                double newActivation = currentActivation * decayFactor * link.associationStrength;
                activationLevels[link.targetMemoryId] = newActivation;
                
                if (newActivation > 0.01) { // Threshold for significant activation
                    activated.push_back(link.targetMemoryId);
                    queue.push({link.targetMemoryId, currentDepth + 1});
                }
            }
        }
    }
    
    // Sort by activation level (highest first)
    std::sort(activated.begin(), activated.end(),
        [&activationLevels](const UUID& a, const UUID& b) {
            return activationLevels[a] > activationLevels[b];
        });
    
    return activated;
}

// ============================================================================
// Phase 1.2: Memory Defragmentation Implementation
// ============================================================================

void AgentMemoryManager::defragmentMemories() {
    withLock([&]() {
        // Remove decayed memories below threshold
        auto decayed = getDecayedMemories(decayParams_.minStrength);
        
        for (const auto& id : decayed) {
            // Remove from tables
            for (auto& [tableName, table] : memoryTables_) {
                table.erase(id);
            }
            
            // Remove from index
            hierarchicalIndex_.removeMemory(id);
            
            // Remove from strength map
            memoryStrengths_.erase(id);
            
            // Remove from associative network
            associativeNetwork_.erase(id);
            
            // Remove incoming links
            for (auto& [sourceId, links] : associativeNetwork_) {
                links.erase(
                    std::remove_if(links.begin(), links.end(),
                        [&id](const AssociativeLink& link) {
                            return link.targetMemoryId == id;
                        }),
                    links.end()
                );
            }
        }
    });
}

size_t AgentMemoryManager::getMemoryCount() const {
    return withLock([&]() -> size_t {
        size_t count = 0;
        for (const auto& [tableName, table] : memoryTables_) {
            count += table.size();
        }
        return count;
    });
}

size_t AgentMemoryManager::getFragmentedCount() const {
    std::lock_guard<std::mutex> lock(strengthMutex_);
    size_t count = 0;
    
    for (const auto& [id, strength] : memoryStrengths_) {
        if (strength.currentStrength < decayParams_.minStrength * 2) {
            count++;
        }
    }
    
    return count;
}

// ============================================================================
// Phase 1.2: Memory Statistics Implementation
// ============================================================================

AgentMemoryManager::MemoryStatistics AgentMemoryManager::getStatistics() const {
    MemoryStatistics stats;
    
    withLock([&]() {
        // Count memories by type
        stats.episodicMemories = hierarchicalIndex_.getByType(HierarchicalMemoryType::EPISODIC).size();
        stats.semanticMemories = hierarchicalIndex_.getByType(HierarchicalMemoryType::SEMANTIC).size();
        stats.proceduralMemories = hierarchicalIndex_.getByType(HierarchicalMemoryType::PROCEDURAL).size();
        stats.workingMemories = hierarchicalIndex_.getByType(HierarchicalMemoryType::WORKING).size();
        
        // Total memories
        for (const auto& [tableName, table] : memoryTables_) {
            stats.totalMemories += table.size();
        }
    });
    
    // Calculate averages from strength map
    {
        std::lock_guard<std::mutex> lock(strengthMutex_);
        
        double totalStrength = 0.0;
        auto now = std::chrono::system_clock::now();
        double totalAgeDays = 0.0;
        
        for (const auto& [id, strength] : memoryStrengths_) {
            totalStrength += strength.currentStrength;
            
            auto ageDuration = now - strength.lastAccessed;
            double ageDays = std::chrono::duration<double, std::ratio<86400>>(ageDuration).count();
            totalAgeDays += ageDays;
            
            if (strength.isConsolidated) {
                stats.consolidatedMemories++;
            }
            
            if (strength.currentStrength < decayParams_.minStrength) {
                stats.decayedMemories++;
            }
        }
        
        if (!memoryStrengths_.empty()) {
            stats.averageStrength = totalStrength / memoryStrengths_.size();
            stats.averageAge = totalAgeDays / memoryStrengths_.size();
        }
    }
    
    // Count associations
    {
        std::lock_guard<std::mutex> lock(associationMutex_);
        for (const auto& [id, links] : associativeNetwork_) {
            stats.totalAssociations += links.size();
        }
        // Divide by 2 since associations are bidirectional
        stats.totalAssociations /= 2;
    }
    
    return stats;
}

} // namespace elizaos
