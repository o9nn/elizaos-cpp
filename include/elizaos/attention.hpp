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

// ECAN Constants
constexpr double MAX_STI = 1.0;  // Maximum short-term importance
constexpr double MAX_LTI = 1.0;  // Maximum long-term importance
constexpr double MIN_STI = 0.0;  // Minimum short-term importance
constexpr double MIN_LTI = 0.0;  // Minimum long-term importance

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
    
    // ECAN compatibility accessors (Short-term importance, Long-term importance)
    double sti() const { return urgency; }      // Alias for short-term importance
    double lti() const { return importance; }   // Alias for long-term importance
    void setSTI(double val) { urgency = val; }
    void setLTI(double val) { importance = val; }
    
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

    // Spreading configuration: bounded setters so the owning allocator can
    // retune the live network instead of only affecting future networks.
    void setSpreadingParameters(double spreadingRate, double activationThreshold);
    double getSpreadingRate() const;
    double getActivationThreshold() const;
    
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

// ============================================================================
// Phase 1.3: Multi-Scale Temporal Attention Window
// ============================================================================

/**
 * Represents attention at different temporal scales
 * Enables attention patterns that consider recent vs. distant history
 */
struct TemporalAttentionWindow {
    double immediateAttention = 0.0;    // Last few seconds (working memory)
    double shortTermAttention = 0.0;    // Last few minutes
    double mediumTermAttention = 0.0;   // Last few hours
    double longTermAttention = 0.0;     // Days to weeks
    
    Timestamp windowStart;
    std::chrono::seconds immediateDuration{30};
    std::chrono::minutes shortTermDuration{30};
    std::chrono::hours mediumTermDuration{24};
    std::chrono::hours longTermDuration{24 * 7};  // One week
    
    // Get weighted composite attention across scales
    double getCompositeAttention(double immediateWeight = 0.4,
                                 double shortTermWeight = 0.3,
                                 double mediumTermWeight = 0.2,
                                 double longTermWeight = 0.1) const {
        return immediateAttention * immediateWeight +
               shortTermAttention * shortTermWeight +
               mediumTermAttention * mediumTermWeight +
               longTermAttention * longTermWeight;
    }
    
    // Update attention for a specific scale based on event time
    void updateFromEvent(const Timestamp& eventTime, double attentionBoost = 0.1) {
        auto now = std::chrono::system_clock::now();
        auto age = now - eventTime;
        
        if (age < immediateDuration) {
            immediateAttention = std::min(1.0, immediateAttention + attentionBoost);
        } else if (age < std::chrono::duration_cast<std::chrono::seconds>(shortTermDuration)) {
            shortTermAttention = std::min(1.0, shortTermAttention + attentionBoost);
        } else if (age < std::chrono::duration_cast<std::chrono::seconds>(mediumTermDuration)) {
            mediumTermAttention = std::min(1.0, mediumTermAttention + attentionBoost * 0.5);
        } else if (age < std::chrono::duration_cast<std::chrono::seconds>(longTermDuration)) {
            longTermAttention = std::min(1.0, longTermAttention + attentionBoost * 0.25);
        }
    }
    
    // Decay all temporal scales
    void decay(double immDecay = 0.9, double shortDecay = 0.95, 
               double medDecay = 0.98, double longDecay = 0.99) {
        immediateAttention *= immDecay;
        shortTermAttention *= shortDecay;
        mediumTermAttention *= medDecay;
        longTermAttention *= longDecay;
    }
};

// ============================================================================
// Phase 1.3: Saliency-Based Attention
// ============================================================================

/**
 * Saliency detector for attention shifting
 * Identifies what should capture attention based on novelty, relevance, and urgency
 */
class SaliencyDetector {
public:
    struct SaliencyFeatures {
        double visualSaliency = 0.0;     // For visual inputs
        double semanticSaliency = 0.0;   // Meaning-based importance
        double temporalSaliency = 0.0;   // Recency/timing importance
        double emotionalSaliency = 0.0;  // Emotional significance
        double goalRelevance = 0.0;      // Relevance to current goals
        
        double getOverallSaliency() const {
            return (visualSaliency + semanticSaliency + temporalSaliency + 
                    emotionalSaliency + goalRelevance) / 5.0;
        }
    };
    
    SaliencyDetector() = default;
    
    // Calculate saliency for different input types
    SaliencyFeatures calculateSaliency(const std::string& content,
                                       const std::vector<std::string>& context = {},
                                       const std::vector<std::string>& currentGoals = {});
    
    // Identify saliency-based attention shift targets
    std::vector<UUID> identifyAttentionShiftTargets(
        const std::unordered_map<UUID, AttentionValue>& currentAttention,
        const std::vector<std::pair<UUID, SaliencyFeatures>>& candidates,
        size_t maxShifts = 5);
    
    // Configure saliency weights
    void setWeights(double visual, double semantic, double temporal, 
                    double emotional, double goal);
    
private:
    double visualWeight_ = 0.2;
    double semanticWeight_ = 0.3;
    double temporalWeight_ = 0.2;
    double emotionalWeight_ = 0.15;
    double goalWeight_ = 0.15;
    
    std::unordered_map<std::string, double> emotionalKeywords_;
    void initializeEmotionalKeywords();
};

// ============================================================================
// Phase 1.3: Attention Cost Budget
// ============================================================================

/**
 * Manages attention allocation with strict resource limits
 * Prevents attention exhaustion through cost tracking
 */
class AttentionCostBudget {
public:
    AttentionCostBudget(double totalBudget = 100.0, double regenerationRate = 1.0);
    
    struct AllocationRequest {
        UUID targetId;
        double requestedAmount;
        double priority;            // Higher priority gets preference
        bool canPartialAllocate;    // Accept less than requested?
        std::chrono::milliseconds timeout{1000};  // How long to wait
    };
    
    struct AllocationResult {
        bool success = false;
        double allocatedAmount = 0.0;
        double remainingBudget = 0.0;
        std::string reason;
    };
    
    // Allocate attention with cost tracking
    AllocationResult requestAllocation(const AllocationRequest& request);
    
    // Release attention back to pool
    void releaseAttention(const UUID& targetId, double amount);
    
    // Budget management
    double getAvailableBudget() const;
    double getTotalBudget() const { return totalBudget_; }
    void setTotalBudget(double budget);
    void setRegenerationRate(double rate);
    
    // Regenerate budget over time
    void tick(double deltaSeconds);
    
    // Get allocation history
    struct AllocationHistory {
        UUID targetId;
        double amount;
        Timestamp allocatedAt;
        bool isActive;
    };
    std::vector<AllocationHistory> getAllocationHistory() const;
    
    // Emergency budget release (when exhausted)
    void emergencyRelease(double threshold = 0.1);
    
private:
    double totalBudget_;
    std::atomic<double> availableBudget_;
    double regenerationRate_;  // Units per second
    
    std::unordered_map<UUID, double> activeAllocations_;
    std::vector<AllocationHistory> history_;
    mutable std::mutex budgetMutex_;
};

// ============================================================================
// Phase 1.3: Attention Pattern Learning
// ============================================================================

/**
 * Learns and predicts attention patterns from history
 * Enables proactive attention allocation
 */
class AttentionPatternLearner {
public:
    AttentionPatternLearner();
    
    struct AttentionEvent {
        UUID targetId;
        std::string category;       // "memory", "task", "input", etc.
        double attentionLevel;
        Timestamp timestamp;
        std::unordered_map<std::string, std::string> context;
    };
    
    struct LearnedPattern {
        std::string patternType;    // "temporal", "contextual", "sequential"
        std::vector<std::string> triggers;
        std::vector<UUID> predictedTargets;
        double confidence;
        int occurrenceCount;
    };
    
    // Record attention events for learning
    void recordEvent(const AttentionEvent& event);
    
    // Learn patterns from recorded history
    void learnPatterns();
    
    // Predict future attention needs
    std::vector<std::pair<UUID, double>> predictAttentionNeeds(
        const std::unordered_map<std::string, std::string>& currentContext,
        size_t maxPredictions = 10);
    
    // Get learned patterns
    std::vector<LearnedPattern> getPatterns() const;
    
    // Configuration
    void setLearningRate(double rate) { learningRate_ = rate; }
    void setMinPatternConfidence(double conf) { minPatternConfidence_ = conf; }
    void setHistorySize(size_t size) { maxHistorySize_ = size; }
    
    // Prune low-confidence patterns
    void prunePatterns(double minConfidence = 0.1);
    
private:
    std::vector<AttentionEvent> eventHistory_;
    std::vector<LearnedPattern> learnedPatterns_;
    
    double learningRate_ = 0.1;
    double minPatternConfidence_ = 0.3;
    size_t maxHistorySize_ = 10000;
    
    mutable std::mutex learnerMutex_;
    
    void detectTemporalPatterns();
    void detectContextualPatterns();
    void detectSequentialPatterns();
};

// ============================================================================
// Phase 1.3: Inter-Agent Attention Transfer
// ============================================================================

class AttentionTransferProtocol;  // forward declaration

/**
 * In-process attention message bus.
 *
 * Provides a real cross-agent boundary so that multiple
 * AttentionTransferProtocol instances running in the same process can
 * exchange focus broadcasts, transfer requests and share collective
 * attention pools without depending on a single locally-installed handler.
 *
 * The bus is thread-safe and may be used either as a process-wide default
 * (AttentionMessageBus::instance()) or as an isolated instance for tests and
 * sandboxed swarms.
 */
class AttentionMessageBus {
public:
    using FocusListener = std::function<void(const std::string& sourceAgentId,
                                             const std::vector<UUID>& focusedItems)>;

    AttentionMessageBus() = default;

    // Process-wide default bus.
    static AttentionMessageBus& instance();

    // Register / unregister a protocol endpoint. Registration is idempotent.
    void registerAgent(const std::string& agentId, AttentionTransferProtocol* protocol);
    void unregisterAgent(const std::string& agentId);
    bool isRegistered(const std::string& agentId) const;
    size_t agentCount() const;

    // Publish a focus broadcast from sourceAgentId to every OTHER registered
    // endpoint. Returns the number of endpoints that received the broadcast.
    size_t publishFocus(const std::string& sourceAgentId,
                        const std::vector<UUID>& focusedItems);

    // Subscribe an out-of-band listener (e.g. an avatar bridge or logger) that
    // observes every focus broadcast on the bus. Returns a subscription id.
    size_t subscribe(FocusListener listener);
    void unsubscribe(size_t subscriptionId);

    // Route a transfer request to the registered target agent. If the target is
    // present its protocol handles the request; otherwise the request is
    // rejected with a descriptive message.
    struct RoutedTransferResult {
        bool delivered;
        bool accepted;
        double actualAmount;
        std::string message;
    };
    RoutedTransferResult routeTransfer(const struct AttentionTransferRequestEnvelope& envelope);

    // Shared collective attention pools that live on the bus (swarm scenarios).
    void contributeToSharedPool(const std::string& poolId, double amount);
    double withdrawFromSharedPool(const std::string& poolId, double requestedAmount);
    double getSharedPoolBalance(const std::string& poolId) const;

private:
    mutable std::mutex busMutex_;
    std::unordered_map<std::string, AttentionTransferProtocol*> endpoints_;
    std::unordered_map<size_t, FocusListener> listeners_;
    std::unordered_map<std::string, double> sharedPools_;
    size_t nextSubscriptionId_ = 1;
};

/**
 * Enables attention coordination between agents
 * Supports collaborative attention and attention sharing
 */
class AttentionTransferProtocol {
public:
    AttentionTransferProtocol(const std::string& agentId);
    ~AttentionTransferProtocol();
    
    struct AttentionTransferRequest {
        std::string sourceAgentId;
        std::string targetAgentId;
        UUID memoryOrTaskId;
        double attentionAmount;
        std::string reason;
        std::chrono::seconds duration{60};  // How long transfer lasts
    };
    
    struct AttentionTransferResponse {
        bool accepted;
        double actualAmount;
        std::string message;
    };
    
    // Initiate attention transfer to another agent
    AttentionTransferResponse requestTransfer(const AttentionTransferRequest& request);
    
    // Handle incoming transfer requests
    using TransferHandler = std::function<AttentionTransferResponse(const AttentionTransferRequest&)>;
    void setTransferHandler(TransferHandler handler);
    
    // Broadcast attention focus to collaborating agents. When connected to a
    // message bus the broadcast is delivered to every other agent on the bus;
    // a locally-installed FocusBroadcastHandler is always invoked as well.
    // Returns the number of remote endpoints reached (0 when bus-less).
    size_t broadcastFocus(const std::vector<UUID>& focusedItems);
    
    // Receive focus broadcasts
    using FocusBroadcastHandler = std::function<void(const std::string& agentId, 
                                                      const std::vector<UUID>& focusedItems)>;
    void setFocusBroadcastHandler(FocusBroadcastHandler handler);

    // Connect this protocol to an attention message bus so that broadcasts and
    // transfer requests cross the agent boundary. Passing nullptr (or calling
    // disconnectFromBus) detaches and falls back to local-handler behavior.
    void connectToBus(AttentionMessageBus* bus);
    void disconnectFromBus();
    bool isConnectedToBus() const;
    const std::string& agentId() const { return agentId_; }

    // Invoked by the bus when another agent broadcasts focus. Triggers the
    // locally-installed FocusBroadcastHandler if present.
    void deliverFocusBroadcast(const std::string& sourceAgentId,
                               const std::vector<UUID>& focusedItems);

    // Invoked by the bus to deliver a routed transfer request to this agent.
    AttentionTransferResponse handleRoutedTransfer(const AttentionTransferRequest& request);
    
    // Collective attention pool for swarm scenarios
    void contributeToPool(const std::string& poolId, double amount);
    double withdrawFromPool(const std::string& poolId, double requestedAmount);
    double getPoolBalance(const std::string& poolId) const;
    
    // Track active transfers
    struct ActiveTransfer {
        AttentionTransferRequest request;
        Timestamp startTime;
        Timestamp expiresAt;
        bool isIncoming;
    };
    std::vector<ActiveTransfer> getActiveTransfers() const;
    
private:
    std::string agentId_;
    TransferHandler transferHandler_;
    FocusBroadcastHandler focusBroadcastHandler_;
    
    std::unordered_map<std::string, double> attentionPools_;
    std::vector<ActiveTransfer> activeTransfers_;
    mutable std::mutex transferMutex_;
    AttentionMessageBus* bus_ = nullptr;
    
    void cleanupExpiredTransfers();
    // Records an accepted transfer in activeTransfers_. Caller must hold transferMutex_.
    void trackTransferLocked(const AttentionTransferRequest& request);
    // Decides whether a transfer is accepted using the installed handler or the
    // default threshold policy. Caller must hold transferMutex_.
    AttentionTransferResponse evaluateTransferLocked(const AttentionTransferRequest& request);
};

// Envelope used by AttentionMessageBus::routeTransfer to avoid a hard
// dependency on the nested AttentionTransferProtocol::AttentionTransferRequest
// type at the point of the bus declaration.
struct AttentionTransferRequestEnvelope {
    std::string sourceAgentId;
    std::string targetAgentId;
    UUID memoryOrTaskId;
    double attentionAmount = 0.0;
    std::string reason;
    std::chrono::seconds duration{60};
};

// ============================================================================
// Phase 1.3: Enhanced Attention Allocator
// ============================================================================

/**
 * Enhanced attention allocator integrating all Phase 1.3 features
 */
class EnhancedAttentionAllocator : public AttentionAllocator {
public:
    EnhancedAttentionAllocator(const std::string& agentId = "default", 
                               double initialBudget = 100.0);
    ~EnhancedAttentionAllocator() = default;
    
    // Multi-scale temporal attention
    void updateTemporalAttention(const UUID& elementId, const Timestamp& eventTime);
    TemporalAttentionWindow getTemporalWindow(const UUID& elementId) const;
    void decayAllTemporalWindows();
    
    // Saliency-based attention shifting
    void enableSaliencyShifting(bool enable = true);
    std::vector<UUID> computeSaliencyBasedShifts(
        const std::vector<std::pair<UUID, std::string>>& candidates);
    
    // Cost-based budgeting
    AttentionCostBudget::AllocationResult allocateWithCost(
        const UUID& targetId, double amount, double priority = 1.0);
    void tickBudget(double deltaSeconds);
    
    // Pattern learning
    void enablePatternLearning(bool enable = true);
    void recordAttentionEvent(const AttentionPatternLearner::AttentionEvent& event);
    std::vector<std::pair<UUID, double>> getPredictedNeeds();
    
    // Inter-agent coordination
    void enableInterAgentTransfer(bool enable = true);
    AttentionTransferProtocol& getTransferProtocol();
    
    // Combined attention score considering all factors
    double getEnhancedAttentionScore(const UUID& elementId) const;
    
    // Statistics
    struct EnhancedStatistics {
        AttentionStatistics base;
        double averageTemporalScore = 0.0;
        double averageSaliencyScore = 0.0;
        double budgetUtilization = 0.0;
        int patternsPredicted = 0;
        int activeTransfers = 0;
    };
    EnhancedStatistics getEnhancedStatistics() const;

private:
    std::string agentId_;
    std::unordered_map<UUID, TemporalAttentionWindow> temporalWindows_;
    std::unique_ptr<SaliencyDetector> saliencyDetector_;
    std::unique_ptr<AttentionCostBudget> costBudget_;
    std::unique_ptr<AttentionPatternLearner> patternLearner_;
    std::unique_ptr<AttentionTransferProtocol> transferProtocol_;
    
    bool saliencyShiftingEnabled_ = false;
    bool patternLearningEnabled_ = false;
    bool interAgentTransferEnabled_ = false;
    
    mutable std::mutex enhancedMutex_;
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
    
    // Phase 1.3: Enhanced attention access
    std::shared_ptr<EnhancedAttentionAllocator> getEnhancedAllocator() const;
    void enableEnhancedAttention(const std::string& agentId = "default");
    
private:
    std::shared_ptr<AttentionAllocator> attentionAllocator_;
    std::shared_ptr<EnhancedAttentionAllocator> enhancedAllocator_;
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
    
    // Phase 1.3: Enhanced operations
    void enableEnhanced(const std::string& agentId = "default");
    double getEnhancedScore(const UUID& elementId);
    std::vector<std::pair<UUID, double>> predictNeeds();
    void recordEvent(const std::string& category, const UUID& targetId, double level);
}

} // namespace elizaos