#pragma once

#include "elizaos/core.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentlogger.hpp"
#include "elizaos/agentaction.hpp"  // For JsonValue definition
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
#include <any>

namespace elizaos {

// Knowledge entry types
enum class KnowledgeType {
    FACT,
    RULE,
    CONCEPT,
    RELATIONSHIP,
    PROCEDURE,
    EXPERIENCE
};

// Knowledge confidence levels
enum class ConfidenceLevel {
    VERY_LOW = 1,
    LOW = 2,
    MEDIUM = 3,
    HIGH = 4,
    VERY_HIGH = 5
};

// Knowledge source types
enum class KnowledgeSource {
    LEARNED,
    PROGRAMMED,
    INFERRED,
    OBSERVED,
    COMMUNICATED
};

// Forward declarations
class KnowledgeBase;
class KnowledgeEntry;
class KnowledgeQuery;

// Knowledge entry structure
class KnowledgeEntry {
public:
    std::string id;
    std::string content;
    KnowledgeType type;
    ConfidenceLevel confidence;
    KnowledgeSource source;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    std::vector<std::string> related_entries;
    
    KnowledgeEntry() : type(KnowledgeType::FACT), confidence(ConfidenceLevel::MEDIUM), source(KnowledgeSource::PROGRAMMED) {
        created_at = std::chrono::system_clock::now();
        updated_at = created_at;
    }
    KnowledgeEntry(const std::string& content, KnowledgeType type = KnowledgeType::FACT);
    
    // Serialization methods
    JsonValue toJson() const;
    static KnowledgeEntry fromJson(const JsonValue& json);
    
    // Utility methods
    void addTag(const std::string& tag);
    void addRelation(const std::string& entryId);
    void updateConfidence(ConfidenceLevel newConfidence);
    bool hasTag(const std::string& tag) const;
};

// Knowledge query parameters
class KnowledgeQuery {
public:
    std::string text;
    std::vector<KnowledgeType> types;
    std::vector<std::string> tags;
    ConfidenceLevel minConfidence = ConfidenceLevel::VERY_LOW;
    int maxResults = 10;
    bool includeRelated = false;
    
    KnowledgeQuery(const std::string& queryText);
};

// Knowledge inference engine
class KnowledgeInferenceEngine {
public:
    KnowledgeInferenceEngine();
    
    // Inference methods
    std::vector<KnowledgeEntry> inferFromFacts(const std::vector<KnowledgeEntry>& facts);
    std::vector<KnowledgeEntry> findRelatedConcepts(const KnowledgeEntry& entry);
    KnowledgeEntry combineEvidence(const std::vector<KnowledgeEntry>& evidence);
    
    // Rule-based inference
    void addInferenceRule(const std::string& ruleName, 
                         std::function<std::vector<KnowledgeEntry>(const std::vector<KnowledgeEntry>&)> rule);
    void removeInferenceRule(const std::string& ruleName);
    
private:
    std::unordered_map<std::string, std::function<std::vector<KnowledgeEntry>(const std::vector<KnowledgeEntry>&)>> rules_;
    std::mutex rulesMutex_;
};

// Main knowledge base class
class KnowledgeBase {
public:
    KnowledgeBase();
    ~KnowledgeBase();
    
    // Core knowledge management
    std::string addKnowledge(const KnowledgeEntry& entry);
    bool updateKnowledge(const std::string& id, const KnowledgeEntry& entry);
    bool removeKnowledge(const std::string& id);
    std::optional<KnowledgeEntry> getKnowledge(const std::string& id);
    
    // Query methods
    std::vector<KnowledgeEntry> query(const KnowledgeQuery& query);
    std::vector<KnowledgeEntry> searchByText(const std::string& text, int maxResults = 10);
    std::vector<KnowledgeEntry> searchByTags(const std::vector<std::string>& tags, int maxResults = 10);
    std::vector<KnowledgeEntry> getRelatedKnowledge(const std::string& entryId, int maxResults = 5);
    
    // Knowledge organization
    std::vector<std::string> getAllTags() const;
    std::unordered_map<KnowledgeType, int> getKnowledgeTypeStats() const;
    std::vector<KnowledgeEntry> getKnowledgeByType(KnowledgeType type);
    
    // Knowledge validation and maintenance
    void validateKnowledge();
    void pruneOldKnowledge(std::chrono::hours maxAge);
    void consolidateKnowledge();
    
    // Inference integration
    std::vector<KnowledgeEntry> performInference(const KnowledgeQuery& query);
    void setInferenceEngine(std::shared_ptr<KnowledgeInferenceEngine> engine);
    
    // Import/Export
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);
    JsonValue exportToJson() const;
    bool importFromJson(const JsonValue& data);
    
    // Statistics and debugging
    size_t getKnowledgeCount() const;
    std::string getStatistics() const;
    void clear();

    // Optional memory & logger integration. When set, knowledge entries are
    // also persisted to the agent memory subsystem and access is traced.
    void setMemoryManager(std::shared_ptr<AgentMemoryManager> mgr);
    std::shared_ptr<AgentMemoryManager> getMemoryManager() const;
    void setLogger(std::shared_ptr<AgentLogger> logger);
    std::shared_ptr<AgentLogger> getLogger() const;

    // Public access to the memory bridge and metrics tracker so that callers
    // (and tests) can drive durable persistence and observe usage.
    void persistToMemory(const KnowledgeEntry& entry);
    std::optional<KnowledgeEntry> loadFromMemory(const std::string& id);
    std::vector<KnowledgeEntry> searchMemory(const std::string& content, int maxResults = 10);
    std::vector<KnowledgeEntry> dumpMemory() const;
    void recordAccess(const KnowledgeEntry& entry);
    bool isValid(const KnowledgeEntry& entry) const;

private:
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger> logger_;
    std::shared_ptr<KnowledgeInferenceEngine> inferenceEngine_;
    mutable std::mutex knowledgeMutex_;
    
    // Internal helper methods
    std::string generateKnowledgeId();
    void saveKnowledgeToMemory(const KnowledgeEntry& entry);
    std::optional<KnowledgeEntry> loadKnowledgeFromMemory(const std::string& id);
    std::vector<KnowledgeEntry> searchMemoryByContent(const std::string& content, int maxResults);
    std::vector<KnowledgeEntry> getAllKnowledgeFromMemory() const;
    
    // Knowledge validation helpers
    bool isValidKnowledgeEntry(const KnowledgeEntry& entry) const;
    void updateKnowledgeMetrics(const KnowledgeEntry& entry);
};

// Global knowledge base instance
extern std::shared_ptr<KnowledgeBase> globalKnowledgeBase;

// Per-entry usage statistics maintained by KnowledgeBase::recordAccess.
namespace knowledge_metrics {
    std::uint64_t getAccessCount(const std::string& id);
    void resetAll();
}


// Utility functions
std::string knowledgeTypeToString(KnowledgeType type);
KnowledgeType stringToKnowledgeType(const std::string& typeStr);
std::string confidenceLevelToString(ConfidenceLevel level);
ConfidenceLevel stringToConfidenceLevel(const std::string& levelStr);
std::string knowledgeSourceToString(KnowledgeSource source);
KnowledgeSource stringToKnowledgeSource(const std::string& sourceStr);

// ============================================================================
// Phase 2.1: Hyperedge and Hypergraph Support
// ============================================================================

/**
 * Hyperedge types for multi-node relationships
 */
struct HyperedgeType {
    enum Type {
        BINARY,      // Standard A->B relationship
        TERNARY,     // A->B via C (e.g., A gives B to C)
        NARY,        // Arbitrary arity
        TEMPORAL,    // Has time component
        WEIGHTED     // Has strength/probability
    };
};

/**
 * A hyperedge connecting multiple knowledge nodes
 */
class Hyperedge {
public:
    std::string id;
    std::string relationshipType;    // e.g., "causes", "implies", "contains"
    std::vector<std::string> nodeIds; // Connected knowledge entries
    HyperedgeType::Type edgeType = HyperedgeType::BINARY;
    double weight = 1.0;              // Edge strength/probability
    std::optional<Timestamp> validFrom;
    std::optional<Timestamp> validUntil;
    std::unordered_map<std::string, std::string> attributes;

    Hyperedge() = default;
    Hyperedge(const std::string& relType, const std::vector<std::string>& nodes);
    bool isValid(const Timestamp& at = std::chrono::system_clock::now()) const;
    bool connects(const std::string& nodeId) const;
    size_t arity() const { return nodeIds.size(); }
};

/**
 * Hypergraph knowledge store supporting multi-node relationships and graph queries
 */
class KnowledgeHypergraph {
public:
    KnowledgeHypergraph();

    // Node management
    void addNode(const KnowledgeEntry& entry);
    void removeNode(const std::string& nodeId);
    std::optional<KnowledgeEntry> getNode(const std::string& nodeId) const;

    // Hyperedge management
    std::string addEdge(const Hyperedge& edge);
    void removeEdge(const std::string& edgeId);
    std::optional<Hyperedge> getEdge(const std::string& edgeId) const;

    // Graph queries
    std::vector<Hyperedge> getEdgesConnecting(const std::string& nodeId) const;
    std::vector<Hyperedge> getEdgesByType(const std::string& relationshipType) const;
    std::vector<std::string> getNeighbors(const std::string& nodeId, int maxDepth = 1) const;

    // Path finding
    std::vector<std::vector<std::string>> findPaths(
        const std::string& fromId,
        const std::string& toId,
        int maxDepth = 5) const;

    // Subgraph extraction
    std::vector<KnowledgeEntry> extractSubgraph(
        const std::string& centerId,
        int radius = 2) const;

    // Pattern matching
    std::vector<std::unordered_map<std::string, std::string>> matchPattern(
        const std::vector<std::string>& patternNodes,
        const std::vector<std::pair<int, int>>& patternEdges,
        const std::string& relationshipType = "") const;

    // Statistics
    size_t nodeCount() const;
    size_t edgeCount() const;
    double averageDegree() const;
    std::vector<std::string> findHubs(int topN = 10) const;

private:
    std::unordered_map<std::string, KnowledgeEntry> nodes_;
    std::unordered_map<std::string, Hyperedge> edges_;
    std::unordered_map<std::string, std::vector<std::string>> nodeToEdges_;
    mutable std::mutex graphMutex_;
    std::string generateEdgeId() const;
};

// ============================================================================
// Phase 2.1: Enhanced Inference Engine
// ============================================================================

/**
 * Inference chain tracking for explainability
 */
struct InferenceStep {
    std::string ruleApplied;
    std::vector<std::string> inputEntryIds;
    std::string outputEntryId;
    double confidence;
    std::string explanation;
};

/**
 * Forward/Backward chaining inference engine
 */
class ChainedInferenceEngine {
public:
    ChainedInferenceEngine();

    struct InferenceRule {
        std::string name;
        std::string description;
        std::vector<std::pair<std::string, std::string>> conditions;
        std::pair<std::string, std::string> conclusion;
        double confidenceModifier = 1.0;
        bool enabled = true;
    };

    // Rule management
    void addRule(const InferenceRule& rule);
    void removeRule(const std::string& ruleName);
    void enableRule(const std::string& ruleName, bool enable = true);
    std::vector<InferenceRule> getRules() const;

    // Forward chaining: given facts, derive new conclusions
    std::vector<KnowledgeEntry> forwardChain(
        const std::vector<KnowledgeEntry>& facts,
        int maxIterations = 10);

    // Backward chaining: given goal, find supporting facts
    std::vector<KnowledgeEntry> backwardChain(
        const KnowledgeEntry& goal,
        const std::vector<KnowledgeEntry>& knowledgeBase);

    // Abductive reasoning: find best explanation for observation
    std::vector<KnowledgeEntry> abductiveReason(
        const KnowledgeEntry& observation,
        const std::vector<KnowledgeEntry>& knowledgeBase);

    // Get inference trace for explainability
    std::vector<InferenceStep> getLastInferenceTrace() const;

    void setMaxChainDepth(int depth);
    int getMaxChainDepth() const;

private:
    std::vector<InferenceRule> rules_;
    std::vector<InferenceStep> lastTrace_;
    int maxChainDepth_ = 10;
    double minConfidence_ = 0.3;
    mutable std::mutex inferenceMutex_;

    bool matchCondition(const KnowledgeEntry& entry,
                       const std::pair<std::string, std::string>& condition) const;
    KnowledgeEntry applyConclusion(
        const std::pair<std::string, std::string>& conclusion,
        const std::vector<KnowledgeEntry>& matchedFacts,
        double confidence) const;
    KnowledgeEntry applyRule(
        const InferenceRule& rule,
        const std::vector<KnowledgeEntry>& matchedFacts);
};

// ============================================================================
// Phase 2.1: Knowledge Fusion Engine
// ============================================================================

struct KnowledgeVersion {
    std::string versionId;
    KnowledgeEntry entry;
    Timestamp timestamp;
    std::string reason;
    std::string changedBy;
};

struct KnowledgeConflict {
    std::vector<KnowledgeEntry> conflictingEntries;
    std::string conflictType;
    double severity;
    std::vector<KnowledgeEntry> resolution;
};

/**
 * Multi-source knowledge fusion engine
 */
class KnowledgeFusionEngine {
public:
    KnowledgeFusionEngine();

    enum class FusionStrategy {
        HIGHEST_CONFIDENCE,
        MOST_RECENT,
        CONSENSUS,
        WEIGHTED_AVERAGE,
        MANUAL
    };

    std::vector<KnowledgeEntry> fuseKnowledge(
        const std::vector<std::vector<KnowledgeEntry>>& sources,
        FusionStrategy strategy = FusionStrategy::HIGHEST_CONFIDENCE);

    std::vector<KnowledgeConflict> detectConflicts(
        const std::vector<KnowledgeEntry>& entries);

    KnowledgeEntry resolveConflict(
        const KnowledgeConflict& conflict,
        FusionStrategy strategy);

    void recordVersion(const KnowledgeEntry& entry,
                      const std::string& reason,
                      const std::string& changedBy = "system");
    std::vector<KnowledgeVersion> getVersionHistory(const std::string& entryId) const;
    std::optional<KnowledgeEntry> getVersionAt(
        const std::string& entryId,
        const Timestamp& at) const;
    std::optional<KnowledgeEntry> rollbackToVersion(const std::string& versionId);

    void setTemporalValidity(const std::string& entryId,
                            std::optional<Timestamp> validFrom,
                            std::optional<Timestamp> validUntil);
    std::vector<KnowledgeEntry> getValidKnowledge(const Timestamp& at) const;

    double calculateSimilarity(const KnowledgeEntry& a, const KnowledgeEntry& b) const;

private:
    std::unordered_map<std::string, std::vector<KnowledgeVersion>> versionHistory_;
    std::vector<KnowledgeConflict> unresolvedConflicts_;
    mutable std::mutex fusionMutex_;
    KnowledgeEntry mergeEntries(const std::vector<KnowledgeEntry>& entries) const;
};

// ============================================================================
// Phase 2.1: Enhanced Knowledge Base
// ============================================================================

/**
 * Enhanced knowledge base with hypergraph and inference
 */
class EnhancedKnowledgeBase : public KnowledgeBase {
public:
    EnhancedKnowledgeBase();
    ~EnhancedKnowledgeBase() = default;

    // Hypergraph operations
    KnowledgeHypergraph& getHypergraph() { return hypergraph_; }
    void enableHypergraph(bool enable = true);
    std::string addRelationship(const std::string& relationshipType,
                               const std::vector<std::string>& nodeIds,
                               double weight = 1.0);
    std::vector<KnowledgeEntry> queryByRelationship(
        const std::string& relationshipType,
        const std::string& involvedNodeId = "");

    // Inference operations
    ChainedInferenceEngine& getInferenceEngine() { return *inferenceEngine_; }
    std::vector<KnowledgeEntry> infer(const KnowledgeQuery& query);
    void enableAutoInference(bool enable = true);

    // Fusion operations
    KnowledgeFusionEngine& getFusionEngine() { return *fusionEngine_; }
    void importAndFuse(const std::vector<KnowledgeEntry>& newKnowledge,
                      KnowledgeFusionEngine::FusionStrategy strategy);

    // Enhanced queries
    std::vector<KnowledgeEntry> semanticSearch(
        const std::string& query,
        int maxResults = 10,
        double minSimilarity = 0.5);

    std::vector<std::vector<KnowledgeEntry>> findConnectionPaths(
        const std::string& fromId,
        const std::string& toId,
        int maxDepth = 5);

    std::vector<std::vector<std::string>> clusterKnowledge(int numClusters = 5);

    struct EnhancedStats {
        size_t totalEntries;
        size_t totalRelationships;
        size_t inferredEntries;
        size_t conflictsPending;
        double graphDensity;
        int averagePathLength;
    };
    EnhancedStats getEnhancedStats() const;

private:
    KnowledgeHypergraph hypergraph_;
    std::unique_ptr<ChainedInferenceEngine> inferenceEngine_;
    std::unique_ptr<KnowledgeFusionEngine> fusionEngine_;
    bool hypergraphEnabled_ = false;
    bool autoInferenceEnabled_ = false;
    mutable std::mutex enhancedMutex_;
};

} // namespace elizaos