#include "elizaos/agentmemory.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <chrono>
#include <iterator>
#include <set>
#include <queue>
#include <numeric>
#include <tuple>
#include <unordered_set>

namespace elizaos {
namespace {

std::string lowerAscii(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

bool containsCaseInsensitive(const std::string& value, const std::string& query) {
    return query.empty() || lowerAscii(value).find(lowerAscii(query)) != std::string::npos;
}

std::string memoryUniquenessKey(const Memory& memory) {
    constexpr char separator = '\x1f';
    return memory.getContent() + separator + memory.getEntityId() + separator +
           memory.getAgentId() + separator + memory.getRoomId() + separator +
           memory.getWorldId();
}

} // namespace

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
        {
            std::lock_guard<std::mutex> strengthLock(strengthMutex_);
            memoryStrengths_[memory->getId()] = strength;
        }

        return memory->getId();
    });
}

std::shared_ptr<Memory> AgentMemoryManager::findMemoryByIdUnlocked(const UUID& id) const {
    // Caller must already hold memoryMutex_ (or thread-safety must be disabled).
    for (const auto& [tableName, table] : memoryTables_) {
        auto it = table.find(id);
        if (it != table.end()) {
            return it->second;
        }
    }
    return nullptr;
}

bool AgentMemoryManager::eraseMemoryStateUnlocked(const UUID& id) {
    bool removed = false;
    for (auto& [tableName, table] : memoryTables_) {
        removed = table.erase(id) > 0 || removed;
    }
    if (!removed) {
        return false;
    }

    hierarchicalIndex_.removeMemory(id);
    {
        std::lock_guard<std::mutex> strengthLock(strengthMutex_);
        memoryStrengths_.erase(id);
    }
    {
        std::lock_guard<std::mutex> associationLock(associationMutex_);
        associativeNetwork_.erase(id);
        for (auto& [sourceId, links] : associativeNetwork_) {
            links.erase(
                std::remove_if(links.begin(), links.end(),
                    [&id](const AssociativeLink& link) {
                        return link.targetMemoryId == id;
                    }),
                links.end());
        }
    }
    return true;
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
        return eraseMemoryStateUnlocked(memoryId);
    });
}

void AgentMemoryManager::deleteManyMemories(const std::vector<UUID>& memoryIds) {
    withLock([&]() {
        for (const auto& id : memoryIds) {
            eraseMemoryStateUnlocked(id);
        }
    });
}

void AgentMemoryManager::deleteAllMemories(const UUID& roomId, const std::string& tableName) {
    withLock([&]() {
        auto tableIt = memoryTables_.find(tableName);
        if (tableIt == memoryTables_.end()) {
            return;
        }

        std::vector<UUID> ids;
        for (const auto& [id, memory] : tableIt->second) {
            if (memory->getRoomId() == roomId) {
                ids.push_back(id);
            }
        }
        for (const auto& id : ids) {
            eraseMemoryStateUnlocked(id);
        }
    });
}

std::vector<std::shared_ptr<Memory>> AgentMemoryManager::getMemories(const MemorySearchParams& params) {
    return withLock([&]() -> std::vector<std::shared_ptr<Memory>> {
        std::vector<std::shared_ptr<Memory>> result;
        auto tableIt = memoryTables_.find(params.tableName);
        if (tableIt == memoryTables_.end()) {
            return result;
        }

        std::unordered_map<UUID, MemoryStrength> strengthSnapshot;
        {
            std::lock_guard<std::mutex> strengthLock(strengthMutex_);
            strengthSnapshot = memoryStrengths_;
        }

        std::optional<std::set<UUID>> typeIds;
        if (params.memoryType) {
            typeIds = hierarchicalIndex_.getByType(*params.memoryType);
        }
        std::vector<std::set<UUID>> conceptIds;
        conceptIds.reserve(params.concepts.size());
        for (const auto& conceptName : params.concepts) {
            conceptIds.push_back(hierarchicalIndex_.getByConcept(conceptName));
        }

        std::unordered_set<std::string> uniquenessKeys;
        for (const auto& [id, memory] : tableIt->second) {
            if (!matchesSearchCriteria(*memory, params)) {
                continue;
            }
            if (typeIds && typeIds->count(id) == 0) {
                continue;
            }
            if (std::any_of(conceptIds.begin(), conceptIds.end(),
                            [&id](const auto& ids) { return ids.count(id) == 0; })) {
                continue;
            }

            const auto strengthIt = strengthSnapshot.find(id);
            const MemoryStrength strength = strengthIt == strengthSnapshot.end()
                ? MemoryStrength{}
                : strengthIt->second;
            if (params.minStrength && strength.currentStrength < *params.minStrength) {
                continue;
            }
            if (params.minImportance && strength.baseImportance < *params.minImportance) {
                continue;
            }
            if (!params.includeDecayed && strength.currentStrength < decayParams_.minStrength) {
                continue;
            }
            if (params.unique && !uniquenessKeys.insert(memoryUniquenessKey(*memory)).second) {
                continue;
            }
            result.push_back(memory);
        }

        if (params.sortByStrength) {
            std::sort(result.begin(), result.end(), [&strengthSnapshot](const auto& a, const auto& b) {
                return strengthSnapshot[a->getId()].currentStrength >
                       strengthSnapshot[b->getId()].currentStrength;
            });
        } else {
            std::sort(result.begin(), result.end(),
                      [](const auto& a, const auto& b) {
                          return a->getCreatedAt() > b->getCreatedAt();
                      });
        }

        const int startIdx = std::max(0, params.start);
        const int requestedEnd = params.end >= 0
            ? std::min(params.end, static_cast<int>(result.size()))
            : static_cast<int>(result.size());
        if (startIdx >= requestedEnd || startIdx >= static_cast<int>(result.size())) {
            return {};
        }
        const int count = params.count > 0 ? params.count : requestedEnd - startIdx;
        const int endIdx = std::min(startIdx + count, requestedEnd);
        return std::vector<std::shared_ptr<Memory>>(
            result.begin() + startIdx, result.begin() + endIdx);
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
        auto tableIt = memoryTables_.find(params.tableName);
        if (tableIt == memoryTables_.end()) {
            return {};
        }

        for (const auto& [id, memory] : tableIt->second) {
            if (params.entityId && memory->getEntityId() != *params.entityId) continue;
            if (params.roomId && memory->getRoomId() != *params.roomId) continue;
            if (params.worldId && memory->getWorldId() != *params.worldId) continue;
            if (params.query && !containsCaseInsensitive(memory->getContent(), *params.query)) continue;
            if (!memory->getEmbedding()) continue;

            const double similarity =
                calculateEmbeddingSimilarity(params.embedding, *memory->getEmbedding());
            if (similarity >= params.matchThreshold) {
                memory->setSimilarity(similarity);
                candidates.emplace_back(memory, similarity);
            }
        }

        std::sort(candidates.begin(), candidates.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });

        std::vector<std::shared_ptr<Memory>> result;
        std::unordered_set<std::string> uniquenessKeys;
        const int limit = params.count > 0
            ? params.count
            : static_cast<int>(candidates.size());
        for (const auto& [memory, similarity] : candidates) {
            if (params.unique &&
                !uniquenessKeys.insert(memoryUniquenessKey(*memory)).second) {
                continue;
            }
            result.push_back(memory);
            if (static_cast<int>(result.size()) >= limit) {
                break;
            }
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
        memoryTables_["memories"] = {};
        hierarchicalIndex_ = HierarchicalIndex{};
        {
            std::lock_guard<std::mutex> strengthLock(strengthMutex_);
            memoryStrengths_.clear();
        }
        {
            std::lock_guard<std::mutex> associationLock(associationMutex_);
            associativeNetwork_.clear();
        }
    });
}

bool AgentMemoryManager::matchesSearchCriteria(const Memory& memory, const MemorySearchParams& params) {
    if (params.entityId && memory.getEntityId() != *params.entityId) return false;
    if (params.agentId && memory.getAgentId() != *params.agentId) return false;
    if (params.roomId && memory.getRoomId() != *params.roomId) return false;
    if (params.worldId && memory.getWorldId() != *params.worldId) return false;
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
    return withLock([&]() {
        std::vector<std::shared_ptr<Memory>> result;
        auto tableIt = memoryTables_.find(tableName);
        if (tableIt == memoryTables_.end()) {
            return result;
        }
        result.reserve(tableIt->second.size());
        for (const auto& [id, memory] : tableIt->second) {
            result.push_back(memory);
        }
        return result;
    });
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

        // Resolve IDs lock-free to avoid self-deadlock on the non-recursive
        // mutex (getMemoryById would re-acquire withLock/memoryMutex_).
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
        std::vector<std::shared_ptr<Memory>> allMemories;
        std::unordered_map<UUID, std::string> sourceTables;
        for (const auto& [tableName, table] : memoryTables_) {
            for (const auto& [id, memory] : table) {
                allMemories.push_back(memory);
                sourceTables[id] = tableName;
            }
        }

        std::unordered_map<UUID, MemoryStrength> strengths;
        {
            std::lock_guard<std::mutex> strengthLock(strengthMutex_);
            strengths = memoryStrengths_;
        }
        auto result = consolidationEngine_.runConsolidationCycle(
            allMemories, strengths, decayParams_);

        std::unordered_map<UUID, std::shared_ptr<Memory>> mergedById;
        for (const auto& memory : allMemories) {
            mergedById[memory->getId()] = memory;
        }

        for (const auto& merge : result.appliedMerges) {
            const auto sourceTableIt = sourceTables.find(merge.sourceId);
            const auto absorbedTableIt = sourceTables.find(merge.absorbedId);
            const std::string targetTable = sourceTableIt != sourceTables.end()
                ? sourceTableIt->second
                : (absorbedTableIt != sourceTables.end()
                    ? absorbedTableIt->second
                    : "memories");

            for (auto& [type, ids] : hierarchicalIndex_.typeIndex) {
                const bool sourceInherited = ids.erase(merge.sourceId) > 0;
                const bool absorbedInherited = ids.erase(merge.absorbedId) > 0;
                if (sourceInherited || absorbedInherited) ids.insert(merge.mergedId);
            }
            for (auto& [conceptName, ids] : hierarchicalIndex_.conceptIndex) {
                const bool sourceInherited = ids.erase(merge.sourceId) > 0;
                const bool absorbedInherited = ids.erase(merge.absorbedId) > 0;
                if (sourceInherited || absorbedInherited) ids.insert(merge.mergedId);
            }
            for (auto& [entity, ids] : hierarchicalIndex_.entityIndex) {
                const bool sourceInherited = ids.erase(merge.sourceId) > 0;
                const bool absorbedInherited = ids.erase(merge.absorbedId) > 0;
                if (sourceInherited || absorbedInherited) ids.insert(merge.mergedId);
            }
            for (auto& [timeBucket, ids] : hierarchicalIndex_.timeIndex) {
                const bool sourceInherited = ids.erase(merge.sourceId) > 0;
                const bool absorbedInherited = ids.erase(merge.absorbedId) > 0;
                if (sourceInherited || absorbedInherited) ids.insert(merge.mergedId);
            }

            for (auto& [tableName, table] : memoryTables_) {
                table.erase(merge.sourceId);
                table.erase(merge.absorbedId);
            }
            auto mergedIt = mergedById.find(merge.mergedId);
            if (mergedIt != mergedById.end()) {
                memoryTables_[targetTable][merge.mergedId] = mergedIt->second;
            }

            std::lock_guard<std::mutex> associationLock(associationMutex_);
            std::vector<AssociativeLink> inheritedLinks;
            auto collectLinks = [&](const UUID& id) {
                auto it = associativeNetwork_.find(id);
                if (it == associativeNetwork_.end()) return;
                for (auto link : it->second) {
                    if (link.targetMemoryId == merge.sourceId ||
                        link.targetMemoryId == merge.absorbedId) {
                        continue;
                    }
                    link.sourceMemoryId = merge.mergedId;
                    inheritedLinks.push_back(std::move(link));
                }
            };
            collectLinks(merge.sourceId);
            collectLinks(merge.absorbedId);
            associativeNetwork_.erase(merge.sourceId);
            associativeNetwork_.erase(merge.absorbedId);
            for (auto& [sourceId, links] : associativeNetwork_) {
                for (auto& link : links) {
                    if (link.targetMemoryId == merge.sourceId ||
                        link.targetMemoryId == merge.absorbedId) {
                        link.targetMemoryId = merge.mergedId;
                    }
                }
                std::sort(links.begin(), links.end(), [](const auto& a, const auto& b) {
                    return std::tie(a.targetMemoryId, a.linkType) <
                           std::tie(b.targetMemoryId, b.linkType);
                });
                links.erase(std::unique(links.begin(), links.end(), [](const auto& a, const auto& b) {
                    return a.targetMemoryId == b.targetMemoryId && a.linkType == b.linkType;
                }), links.end());
            }
            std::sort(inheritedLinks.begin(), inheritedLinks.end(), [](const auto& a, const auto& b) {
                return std::tie(a.targetMemoryId, a.linkType) <
                       std::tie(b.targetMemoryId, b.linkType);
            });
            inheritedLinks.erase(std::unique(inheritedLinks.begin(), inheritedLinks.end(), [](const auto& a, const auto& b) {
                return a.targetMemoryId == b.targetMemoryId && a.linkType == b.linkType;
            }), inheritedLinks.end());
            if (!inheritedLinks.empty()) {
                associativeNetwork_[merge.mergedId] = std::move(inheritedLinks);
            }
        }

        {
            std::lock_guard<std::mutex> strengthLock(strengthMutex_);
            memoryStrengths_ = std::move(strengths);
        }
        return result;
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
    
    const auto mergeCandidates =
        findMergeCandidates(memories, params_.similarityMergeThreshold);
    std::unordered_map<UUID, std::shared_ptr<Memory>> byId;
    for (const auto& memory : memories) {
        byId[memory->getId()] = memory;
    }
    std::unordered_set<UUID> consumed;
    std::vector<std::shared_ptr<Memory>> mergedMemories;
    for (const auto& [sourceId, absorbedId] : mergeCandidates) {
        if (consumed.count(sourceId) || consumed.count(absorbedId)) {
            continue;
        }
        auto sourceIt = byId.find(sourceId);
        auto absorbedIt = byId.find(absorbedId);
        auto sourceStrengthIt = strengthMap.find(sourceId);
        auto absorbedStrengthIt = strengthMap.find(absorbedId);
        if (sourceIt == byId.end() || absorbedIt == byId.end() ||
            sourceStrengthIt == strengthMap.end() ||
            absorbedStrengthIt == strengthMap.end()) {
            continue;
        }

        auto merged = mergeMemories(sourceIt->second, absorbedIt->second,
                                    sourceStrengthIt->second,
                                    absorbedStrengthIt->second);
        MemoryStrength mergedStrength;
        mergedStrength.currentStrength = std::max(
            sourceStrengthIt->second.currentStrength,
            absorbedStrengthIt->second.currentStrength);
        mergedStrength.baseImportance = std::max(
            sourceStrengthIt->second.baseImportance,
            absorbedStrengthIt->second.baseImportance);
        mergedStrength.emotionalSalience = std::max(
            sourceStrengthIt->second.emotionalSalience,
            absorbedStrengthIt->second.emotionalSalience);
        mergedStrength.accessCount = sourceStrengthIt->second.accessCount +
                                     absorbedStrengthIt->second.accessCount;
        mergedStrength.lastAccessed = std::max(
            sourceStrengthIt->second.lastAccessed,
            absorbedStrengthIt->second.lastAccessed);
        mergedStrength.lastConsolidated = std::max(
            sourceStrengthIt->second.lastConsolidated,
            absorbedStrengthIt->second.lastConsolidated);
        mergedStrength.isConsolidated = sourceStrengthIt->second.isConsolidated ||
                                        absorbedStrengthIt->second.isConsolidated;

        consumed.insert(sourceId);
        consumed.insert(absorbedId);
        strengthMap.erase(sourceId);
        strengthMap.erase(absorbedId);
        strengthMap[merged->getId()] = mergedStrength;
        mergedMemories.push_back(merged);
        result.appliedMerges.push_back({sourceId, absorbedId, merged->getId()});
    }

    if (!mergedMemories.empty()) {
        memories.erase(
            std::remove_if(memories.begin(), memories.end(),
                           [&consumed](const auto& memory) {
                               return consumed.count(memory->getId()) > 0;
                           }),
            memories.end());
        memories.insert(memories.end(), mergedMemories.begin(), mergedMemories.end());
    }
    result.memoriesMerged = static_cast<int>(result.appliedMerges.size());

    double totalStrengthAfter = 0.0;
    for (const auto& [id, strength] : strengthMap) {
        totalStrengthAfter += strength.currentStrength;
    }
    result.averageStrengthAfter = strengthMap.empty()
        ? 0.0
        : totalStrengthAfter / strengthMap.size();
    
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
    const bool firstIsPrimary = s1.currentStrength >= s2.currentStrength;
    const auto& primary = firstIsPrimary ? m1 : m2;
    const auto& secondary = firstIsPrimary ? m2 : m1;
    std::string content = primary->getContent();
    if (!secondary->getContent().empty() && secondary->getContent() != content) {
        if (!content.empty()) content += "\n";
        content += secondary->getContent();
    }

    auto merged = std::make_shared<Memory>(
        generateUUID(), content, primary->getEntityId(), primary->getAgentId(),
        primary->getMetadata());
    merged->setRoomId(primary->getRoomId());
    merged->setWorldId(primary->getWorldId());
    merged->setUnique(primary->isUnique() || secondary->isUnique());

    if (m1->getEmbedding() && m2->getEmbedding()) {
        EmbeddingVector avgEmbedding;
        const auto& e1 = *m1->getEmbedding();
        const auto& e2 = *m2->getEmbedding();
        avgEmbedding.reserve(std::min(e1.size(), e2.size()));
        for (size_t i = 0; i < std::min(e1.size(), e2.size()); ++i) {
            avgEmbedding.push_back((e1[i] + e2[i]) / 2.0f);
        }
        merged->setEmbedding(avgEmbedding);
    } else if (primary->getEmbedding()) {
        merged->setEmbedding(*primary->getEmbedding());
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
    
    auto tokenize = [](const std::string& content) {
        std::set<std::string> words;
        std::string token;
        for (unsigned char c : content) {
            if (std::isalnum(c)) {
                token.push_back(static_cast<char>(std::tolower(c)));
            } else if (!token.empty()) {
                words.insert(std::move(token));
                token.clear();
            }
        }
        if (!token.empty()) {
            words.insert(std::move(token));
        }
        return words;
    };

    const auto words1 = tokenize(c1);
    const auto words2 = tokenize(c2);
    if (words1.empty() || words2.empty()) return 0.0;

    std::vector<std::string> intersection;
    std::set_intersection(words1.begin(), words1.end(),
                          words2.begin(), words2.end(),
                          std::back_inserter(intersection));
    const std::size_t unionSize = words1.size() + words2.size() - intersection.size();
    return unionSize == 0
        ? 0.0
        : static_cast<double>(intersection.size()) / static_cast<double>(unionSize);
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
    const auto decayed = getDecayedMemories(decayParams_.minStrength);
    withLock([&]() {
        for (const auto& id : decayed) {
            eraseMemoryStateUnlocked(id);
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
