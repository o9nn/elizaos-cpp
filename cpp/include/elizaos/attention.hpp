#pragma once

/**
 * ElizaOS C++ - Attention Module
 *
 * ECAN-inspired attention allocation: AttentionValue, AttentionBudget,
 * ActivationSpreadingNetwork, AttentionAllocator, AttentionAwareMemoryManager.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include "agentmemory.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// AttentionValue – per-element attention state
// ============================================================================

struct AttentionValue {
    double importance  = 0.0;
    double urgency     = 0.0;
    double novelty     = 0.0;
    double activation  = 0.0;

    static constexpr double kImportanceWeight  = 0.4;
    static constexpr double kUrgencyWeight     = 0.3;
    static constexpr double kNoveltyWeight     = 0.2;
    static constexpr double kActivationWeight  = 0.1;

    double getCompositeScore() const {
        return importance * kImportanceWeight
             + urgency    * kUrgencyWeight
             + novelty    * kNoveltyWeight
             + activation * kActivationWeight;
    }

    void decayUrgency(double factor)    { urgency    *= factor; }
    void decayNovelty(double factor)    { novelty    *= factor; }
    void decayActivation(double factor) { activation *= factor; }
};

struct AttentionPriorityItem {
    UUID   elementId;
    double score = 0.0;
    bool operator<(const AttentionPriorityItem& o) const { return score < o.score; }
};

// ============================================================================
// AttentionBudget
// ============================================================================

class AttentionBudget {
public:
    explicit AttentionBudget(double totalBudget = 100.0);

    bool   allocateAttention(const UUID& elementId, double amount);
    bool   deallocateAttention(const UUID& elementId, double amount);
    double getAvailableBudget() const;
    double getTotalBudget()     const { return totalBudget_; }
    double getAllocation(const UUID& elementId) const;

    std::unordered_map<UUID, double> getAllocations() const;

    void redistributeBudget();
    void resetBudget();
    void adjustTotalBudget(double newBudget);

private:
    double                             totalBudget_;
    std::atomic<double>                allocatedBudget_{0.0};
    std::unordered_map<UUID, double>   allocations_;
    mutable std::mutex                 budgetMutex_;
};

// ============================================================================
// ActivationSpreadingNetwork
// ============================================================================

class ActivationSpreadingNetwork {
public:
    ActivationSpreadingNetwork();

    void addNode(const UUID& nodeId, double initialActivation = 0.0);
    void removeNode(const UUID& nodeId);
    void addEdge(const UUID& fromNode, const UUID& toNode, double weight = 1.0);
    void removeEdge(const UUID& fromNode, const UUID& toNode);

    void spreadActivation(int iterations = 1);
    void injectActivation(const UUID& nodeId, double activation);
    void decayActivations(double decayRate = 0.9);
    void normalizeActivations();
    void clear();

    double             getActivation(const UUID& nodeId) const;
    double             getEdgeWeight(const UUID& fromNode, const UUID& toNode) const;
    std::vector<UUID>  getNeighbors(const UUID& nodeId) const;
    std::vector<UUID>  getTopActivatedNodes(size_t count) const;

private:
    struct Node {
        UUID   id;
        double activation = 0.0;
        std::unordered_map<UUID, double> edges;
    };

    std::unordered_map<UUID, std::unique_ptr<Node>> nodes_;
    mutable std::mutex networkMutex_;
    double maxActivation_ = 1.0;
    double spreadingRate_ = 0.5;
    double threshold_     = 0.01;
};

// ============================================================================
// AttentionAllocator
// ============================================================================

class AttentionAllocator {
public:
    struct AttentionStatistics {
        size_t totalElements   = 0;
        double averageScore    = 0.0;
        double maxScore        = 0.0;
        double minScore        = 0.0;
        double totalBudgetUsed = 0.0;
    };

    explicit AttentionAllocator(double initialBudget = 100.0);

    void           updateAttentionValue(const UUID& elementId, const AttentionValue& value);
    AttentionValue getAttentionValue(const UUID& elementId) const;
    bool           hasAttentionValue(const UUID& elementId) const;

    std::vector<UUID>                    allocateAttention(const std::vector<UUID>& candidates, size_t maxAllocations);
    std::vector<AttentionPriorityItem>   prioritizeElements(const std::vector<UUID>& elements);
    std::vector<std::shared_ptr<Memory>> prioritizeMemories(const std::vector<std::shared_ptr<Memory>>& memories);

    void   updateMemoryAttention(std::shared_ptr<Memory> memory, const AttentionValue& value);
    double calculateNovelty(const std::string& content, const std::vector<std::string>& context);
    void   updateNoveltyModel(const std::string& content);

    void   spreadActivation(int iterations = 1);
    void   addAttentionLink(const UUID& fromElement, const UUID& toElement, double weight = 1.0);
    void   decayAttentionValues(double decayRate = 0.9);
    void   periodicMaintenance();

    std::vector<UUID>   getTopAttentionElements(size_t count) const;
    AttentionStatistics getStatistics() const;

    void setDecayRates(double urgencyDecay, double noveltyDecay, double activationDecay);
    void setBudgetSize(double newBudget);
    void setSpreadingParameters(double spreadingRate, double threshold);

private:
    double calculateImportance(const std::string& content, const std::vector<std::string>& context);
    double calculateUrgency(const Timestamp& timestamp, const std::vector<std::string>& tags);
    void   normalizeAttentionValues();
    std::vector<std::string> extractFeatures(const std::string& content);

    std::unique_ptr<AttentionBudget>            budget_;
    std::unique_ptr<ActivationSpreadingNetwork> spreadingNetwork_;

    std::unordered_map<UUID, AttentionValue>    attentionValues_;
    std::unordered_map<std::string, int>        noveltyModel_;

    double urgencyDecay_    = 0.9;
    double noveltyDecay_    = 0.8;
    double activationDecay_ = 0.7;

    mutable std::mutex allocatorMutex_;
};

// ============================================================================
// AttentionAwareMemoryManager
// ============================================================================

class AttentionAwareMemoryManager {
public:
    AttentionAwareMemoryManager();

    std::vector<std::shared_ptr<Memory>> getMemoriesWithAttention(const MemorySearchParams& params, size_t count);
    std::vector<std::shared_ptr<Memory>> searchMemoriesByAttention(const MemorySearchByEmbeddingParams& params, double minAttentionScore);

    void updateMemoryAttention(const UUID& memoryId, const AttentionValue& value);
    void refreshMemoryAttention(const UUID& memoryId);
    void consolidateMemories();
    void forgetLowAttentionMemories(double threshold = 0.1);

    void setMemoryManager(std::shared_ptr<AgentMemoryManager> memoryManager);
    std::shared_ptr<AgentMemoryManager> getMemoryManager() const;
    std::shared_ptr<AttentionAllocator> getAttentionAllocator() const;

    void clear();

private:
    void   updateMemoryLinks(std::shared_ptr<Memory> memory);
    double calculateMemoryImportance(std::shared_ptr<Memory> memory);
    double calculateMemoryUrgency(std::shared_ptr<Memory> memory);
    double calculateMemoryNovelty(std::shared_ptr<Memory> memory);

    std::shared_ptr<AgentMemoryManager> memoryManager_;
    std::shared_ptr<AttentionAllocator> attentionAllocator_;
    mutable std::mutex                  managerMutex_;
};

// ============================================================================
// Global accessors and convenience namespace
// ============================================================================

AttentionAwareMemoryManager& getGlobalAttentionAwareMemoryManager();

namespace attention {
    std::vector<std::shared_ptr<Memory>> getTopMemories(size_t count);
    std::vector<std::shared_ptr<Memory>> searchWithAttention(const MemorySearchParams& params, double minAttentionScore);
    void           updateAttention(const UUID& elementId, const AttentionValue& value);
    AttentionValue getAttention(const UUID& elementId);
    std::vector<UUID> allocateResources(const std::vector<UUID>& candidates, size_t maxAllocations);
    void           performMaintenance();
    void           decayAttentionValues(double decayRate = 0.9);
}

} // namespace elizaos
