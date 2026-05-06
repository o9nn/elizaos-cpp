#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentmemory.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
#include <functional>
#include <atomic>
#include <chrono>
#include <queue>

namespace elizaos {

/**
 * ECAN-inspired Attention Allocation System
 * 
 * This module implements an Economic Attention Networks (ECAN) inspired
 * attention management system for dynamic resource allocation based on
 * importance, urgency, and novelty factors.
 */

// Forward declarations
struct AttentionValue;
class AttentionBudget;
class AttentionAllocator;
class ActivationSpreadingNetwork;

/**
 * Attention Value structure representing the attention economy metrics
 * for a memory or cognitive element
 */
struct AttentionValue {
    double importance = 0.0;    // Long-term significance [0.0, 1.0]
    double urgency = 0.0;       // Short-term priority [0.0, 1.0]
    double novelty = 0.0;       // Degree of novelty/surprise [0.0, 1.0]
    double activation = 0.0;    // Current activation level [0.0, 1.0]
    Timestamp lastUpdated = std::chrono::system_clock::now();
    
    // Composite attention score calculation
    double getCompositeScore() const {
        // Weighted combination of factors
        const double importanceWeight = 0.4;
        const double urgencyWeight = 0.3;
        const double noveltyWeight = 0.2;
        const double activationWeight = 0.1;
        
        return (importance * importanceWeight) +
               (urgency * urgencyWeight) +
               (novelty * noveltyWeight) +
               (activation * activationWeight);
    }
    
    // Decay functions for temporal dynamics
    void decayUrgency(double decayRate = 0.95) {
        urgency *= decayRate;
    }
    
    void decayNovelty(double decayRate = 0.90) {
        novelty *= decayRate;
    }
    
    void decayActivation(double decayRate = 0.85) {
        activation *= decayRate;
    }
};

/**
 * Attention Budget management for resource allocation
 */
class AttentionBudget {
public:
    AttentionBudget(double totalBudget = 100.0);
    
    // Budget allocation and management
    bool allocateAttention(const UUID& elementId, double amount);
    bool deallocateAttention(const UUID& elementId, double amount);
    double getAvailableBudget() const;
    double getTotalBudget() const { return totalBudget_; }
    double getAllocatedBudget() const { return allocatedBudget_.load(); }
    
    // Budget redistribution
    void redistributeBudget();
    void resetBudget();
    
    // Allocation tracking
    std::unordered_map<UUID, double> getAllocations() const;
    double getAllocation(const UUID& elementId) const;
    
    // Budget adjustment
    void adjustTotalBudget(double newBudget);
    
private:
    double totalBudget_;
    std::atomic<double> allocatedBudget_{0.0};
    std::unordered_map<UUID, double> allocations_;
    mutable std::mutex budgetMutex_;
};

/**
 * Activation Spreading Network for attention propagation
 */
class ActivationSpreadingNetwork {
public:
    ActivationSpreadingNetwork();
    
    // Network construction
    void addNode(const UUID& nodeId, double initialActivation = 0.0);
    void addEdge(const UUID& fromNode, const UUID& toNode, double weight = 1.0);
    void removeNode(const UUID& nodeId);
    void removeEdge(const UUID& fromNode, const UUID& toNode);
    
    // Activation spreading
    void spreadActivation(int iterations = 1);
    void injectActivation(const UUID& nodeId, double activation);
    double getActivation(const UUID& nodeId) const;
    
    // Network analysis
    std::vector<UUID> getTopActivatedNodes(size_t count = 10) const;
    std::vector<UUID> getNeighbors(const UUID& nodeId) const;
    double getEdgeWeight(const UUID& fromNode, const UUID& toNode) const;
    
    // Network maintenance
    void decayActivations(double decayRate = 0.95);
    void normalizeActivations();
    void clear();
    
private:
    struct Node {
        UUID id;
        double activation = 0.0;
        std::unordered_map<UUID, double> outEdges; // target -> weight
        std::unordered_map<UUID, double> inEdges;  // source -> weight
    };
    
    std::unordered_map<UUID, std::unique_ptr<Node>> nodes_;
    mutable std::mutex networkMutex_;
    
    // Spreading parameters
    double spreadingRate_ = 0.8;
    double activationThreshold_ = 0.01;
    double maxActivation_ = 1.0;
};

/**
 * Priority queue item for attention-based sorting
 */
struct AttentionPriorityItem {
    UUID elementId;
    double priority;
    AttentionValue attentionValue;
    std::shared_ptr<Memory> memory; // Optional memory reference
    
    bool operator<(const AttentionPriorityItem& other) const {
        return priority < other.priority; // Min-heap, so reverse for max priority
    }
};

/**
 * Main Attention Allocator class implementing ECAN-inspired attention management
 */
class AttentionAllocator {
public:
    AttentionAllocator(double initialBudget = 100.0);
    ~AttentionAllocator() = default;
    
    // Core attention management
    void updateAttentionValue(const UUID& elementId, const AttentionValue& value);
    AttentionValue getAttentionValue(const UUID& elementId) const;
    bool hasAttentionValue(const UUID& elementId) const;
    
    // Resource allocation
    std::vector<UUID> allocateAttention(const std::vector<UUID>& candidates, size_t maxAllocations);
    std::vector<AttentionPriorityItem> prioritizeElements(const std::vector<UUID>& elements);
    
    // Memory integration
    std::vector<std::shared_ptr<Memory>> prioritizeMemories(const std::vector<std::shared_ptr<Memory>>& memories);
    void updateMemoryAttention(std::shared_ptr<Memory> memory, const AttentionValue& value);
    
    // Novelty detection
    double calculateNovelty(const std::string& content, const std::vector<std::string>& context);
    void updateNoveltyModel(const std::string& content);
    
    // Activation spreading
    void enableActivationSpreading(bool enable = true) { activationSpreadingEnabled_ = enable; }
    void spreadActivation(int iterations = 1);
    void addAttentionLink(const UUID& fromElement, const UUID& toElement, double weight = 1.0);
    
    // Temporal dynamics
    void decayAttentionValues(double decayRate = 0.95);
    void periodicMaintenance();
    
    // Statistics and monitoring
    struct AttentionStatistics {
        size_t totalElements;
        double averageAttention;
        double maxAttention;
        double minAttention;
        double budgetUtilization;
        size_t activeElements;
    };
    
    AttentionStatistics getStatistics() const;
    std::vector<UUID> getTopAttentionElements(size_t count = 10) const;
    
    // Configuration
    void setDecayRates(double urgencyDecay, double noveltyDecay, double activationDecay);
    void setBudgetSize(double newBudget);
    void setSpreadingParameters(double spreadingRate, double threshold);
    
private:
    // Core data structures
    std::unordered_map<UUID, AttentionValue> attentionValues_;
    std::unique_ptr<AttentionBudget> budget_;
    std::unique_ptr<ActivationSpreadingNetwork> spreadingNetwork_;
    
    // Novelty detection
    std::unordered_map<std::string, double> noveltyModel_; // Simple frequency-based model
    std::mutex noveltyMutex_;
    
    // Configuration parameters
    double urgencyDecayRate_ = 0.95;
    double noveltyDecayRate_ = 0.90;
    double activationDecayRate_ = 0.85;
    bool activationSpreadingEnabled_ = true;
    
    // Thread safety
    mutable std::mutex attentionMutex_;
    
    // Helper methods
    double calculateImportance(const std::string& content, const std::vector<std::string>& context);
    double calculateUrgency(const Timestamp& timestamp, const std::vector<std::string>& tags);
    void normalizeAttentionValues();
    std::vector<std::string> extractFeatures(const std::string& content);
};

/**
 * Attention-aware Memory Manager extension
 * Integrates attention allocation with memory management
 */
class AttentionAwareMemoryManager {
public:
    AttentionAwareMemoryManager();
    ~AttentionAwareMemoryManager() = default;
    
    // Attention-guided memory operations
    std::vector<std::shared_ptr<Memory>> getMemoriesWithAttention(
        const MemorySearchParams& params, 
        size_t maxResults = 10
    );
    
    std::vector<std::shared_ptr<Memory>> searchMemoriesByAttention(
        const MemorySearchByEmbeddingParams& params,
        double minAttentionScore = 0.1
    );
    
    // Memory attention updates
    void updateMemoryAttention(const UUID& memoryId, const AttentionValue& value);
    void refreshMemoryAttention(const UUID& memoryId);
    
    // Attention-based memory maintenance
    void consolidateMemories(); // Merge similar high-attention memories
    void forgetLowAttentionMemories(double threshold = 0.01);
    
    // Integration with existing memory manager
    void setMemoryManager(std::shared_ptr<class AgentMemoryManager> memoryManager);
    std::shared_ptr<class AgentMemoryManager> getMemoryManager() const;
    
    // Attention allocator access
    std::shared_ptr<AttentionAllocator> getAttentionAllocator() const;
    
private:
    std::shared_ptr<AttentionAllocator> attentionAllocator_;
    std::shared_ptr<class AgentMemoryManager> memoryManager_;
    
    // Memory-attention mapping
    std::unordered_map<UUID, AttentionValue> memoryAttentionMap_;
    mutable std::mutex memoryAttentionMutex_;
    
    // Helper methods
    void updateMemoryLinks(std::shared_ptr<Memory> memory);
    double calculateMemoryImportance(std::shared_ptr<Memory> memory);
    double calculateMemoryUrgency(std::shared_ptr<Memory> memory);
    double calculateMemoryNovelty(std::shared_ptr<Memory> memory);
};

// Global attention-aware memory manager instance
AttentionAwareMemoryManager& getGlobalAttentionAwareMemoryManager();

// Convenience functions for attention-aware memory operations
namespace attention {
    // Memory operations with attention
    std::vector<std::shared_ptr<Memory>> getTopMemories(size_t count = 10);
    std::vector<std::shared_ptr<Memory>> searchWithAttention(
        const MemorySearchParams& params,
        double minAttentionScore = 0.1
    );
    
    // Attention value management
    void updateAttention(const UUID& elementId, const AttentionValue& value);
    AttentionValue getAttention(const UUID& elementId);
    
    // Resource allocation
    std::vector<UUID> allocateResources(const std::vector<UUID>& candidates, size_t maxAllocations);
    
    // System maintenance
    void performMaintenance();
    void decayAttentionValues(double decayRate = 0.95);
}

} // namespace elizaos