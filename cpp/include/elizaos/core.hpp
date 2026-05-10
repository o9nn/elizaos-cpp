#pragma once

/**
 * ElizaOS C++ - Core Module
 *
 * Provides foundational cognitive types: TruthValue, HypergraphNode/Edge,
 * Memory, Actor, Goal, State, Task/TaskManager, PLN inference, and
 * the CognitiveFusionEngine.
 */

#include "elizaos.hpp"
#include <algorithm>
#include <any>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// ============================================================================
// Probabilistic Logic Networks – TruthValue
// ============================================================================

struct TruthValue {
    double strength   = 1.0;
    double confidence = 1.0;

    TruthValue() = default;
    TruthValue(double s, double c) : strength(s), confidence(c) {}

    TruthValue conjunction(const TruthValue& other) const;
    TruthValue disjunction(const TruthValue& other) const;
    TruthValue negation()                           const;
    TruthValue implication(const TruthValue& other) const;
};

// ============================================================================
// Hypergraph (AtomSpace)
// ============================================================================

class HypergraphNode {
public:
    HypergraphNode(const UUID& id, const std::string& label);

    const UUID&        getId()    const { return id_; }
    const std::string& getLabel() const { return label_; }

    void setAttribute(const std::string& key, const std::string& value);
    std::optional<std::string> getAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }

    TruthValue truthValue;

private:
    UUID        id_;
    std::string label_;
    std::unordered_map<std::string, std::string> attributes_;
};

class HypergraphEdge {
public:
    HypergraphEdge(const UUID& id, const std::string& label, const std::vector<UUID>& nodeIds);

    const UUID&              getId()      const { return id_; }
    const std::string&       getLabel()   const { return label_; }
    const std::vector<UUID>& getNodeIds() const { return nodeIds_; }

    TruthValue truthValue;

private:
    UUID              id_;
    std::string       label_;
    std::vector<UUID> nodeIds_;
};

// ============================================================================
// Memory metadata variants
// ============================================================================

struct MessageMetadata {
    std::string type   = "message";
    std::string source = "user";
};

struct CustomMetadata {
    std::unordered_map<std::string, std::string> customData;
};

struct DocumentMetadata {
    std::string title;
    std::string author;
    std::string mimeType = "text/plain";
};

using MemoryMetadata = std::variant<MessageMetadata, CustomMetadata, DocumentMetadata>;

// ============================================================================
// Memory
// ============================================================================

class Memory {
public:
    Memory(const UUID& id,
           const std::string& content,
           const UUID& entityId,
           const UUID& agentId);

    Memory(const UUID& id,
           const std::string& content,
           const UUID& entityId,
           const UUID& agentId,
           const MemoryMetadata& metadata);

    // Getters
    const UUID&        getId()        const { return id_; }
    const std::string& getContent()   const { return content_; }
    const UUID&        getEntityId()  const { return entityId_; }
    const UUID&        getAgentId()   const { return agentId_; }
    const UUID&        getRoomId()    const { return roomId_; }
    Timestamp          getCreatedAt() const { return createdAt_; }
    double             getSimilarity() const { return similarity_; }
    bool               isUnique()    const { return unique_; }

    std::optional<EmbeddingVector> getEmbedding() const { return embedding_; }
    const MemoryMetadata&          getMetadata()  const { return metadata_; }

    // Setters
    void setRoomId(const UUID& roomId)                  { roomId_ = roomId; }
    void setEmbedding(const EmbeddingVector& embedding) { embedding_ = embedding; }
    void setSimilarity(double similarity)               { similarity_ = similarity; }
    void setAgentId(const UUID& agentId)                { agentId_ = agentId; }
    void setContent(const std::string& content)         { content_ = content; }
    void setMetadata(const MemoryMetadata& metadata)    { metadata_ = metadata; }
    void setUnique(bool unique)                         { unique_ = unique; }

private:
    UUID                          id_;
    std::string                   content_;
    UUID                          entityId_;
    UUID                          agentId_;
    UUID                          roomId_;
    Timestamp                     createdAt_;
    double                        similarity_ = 0.0;
    bool                          unique_     = false;
    std::optional<EmbeddingVector> embedding_;
    MemoryMetadata                 metadata_;
};

// ============================================================================
// Agent configuration / state types
// ============================================================================

struct AgentConfig {
    std::string agentId;
    std::string agentName;
    std::string bio;
    std::string lore;
    std::string personality;
};

struct Actor {
    UUID        id;
    std::string name;
    std::string description;
};

// Simple goal used by State (distinct from the full GoalManager::Goal)
struct Goal {
    UUID        id;
    std::string description;
    std::string status;      // "active", "pending", "completed", …
    Timestamp   createdAt;
    Timestamp   updatedAt;
};

class State {
public:
    explicit State(const AgentConfig& config);

    // Accessors
    const std::string& getAgentId()   const { return config_.agentId; }
    const std::string& getAgentName() const { return config_.agentName; }
    const std::string& getBio()       const { return config_.bio; }
    const std::string& getLore()      const { return config_.lore; }

    const std::vector<Actor>&                         getActors()         const { return actors_; }
    const std::vector<Goal>&                          getGoals()          const { return goals_; }
    const std::vector<std::shared_ptr<Memory>>&       getRecentMessages() const { return recentMessages_; }

    // Mutators
    void addActor(const Actor& actor);
    void addGoal(const Goal& goal);
    void addRecentMessage(std::shared_ptr<Memory> memory);

private:
    AgentConfig                             config_;
    std::vector<Actor>                      actors_;
    std::vector<Goal>                       goals_;
    std::vector<std::shared_ptr<Memory>>    recentMessages_;
};

// ============================================================================
// Task management
// ============================================================================

enum class TaskStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED,
    CANCELLED
};

class Task {
public:
    Task(const UUID& id, const std::string& name, const std::string& description);

    const UUID&        getId()          const { return id_; }
    const std::string& getName()        const { return name_; }
    const std::string& getDescription() const { return description_; }
    TaskStatus         getStatus()      const { return status_; }
    int                getPriority()    const { return priority_; }
    Timestamp          getCreatedAt()   const { return createdAt_; }

    const JsonValue&              getOptions() const { return options_; }
    const std::vector<std::string>& getTags()  const { return tags_; }

    void setStatus(TaskStatus status)              { status_ = status; updatedAt_ = std::chrono::system_clock::now(); }
    void setPriority(int priority)                 { priority_ = priority; }
    void setScheduledTime(const Timestamp& time)   { scheduledTime_ = time; }
    void setOptions(const JsonValue& opts)         { options_ = opts; }
    void addTag(const std::string& tag)            { tags_.push_back(tag); }

private:
    UUID               id_;
    std::string        name_;
    std::string        description_;
    TaskStatus         status_     = TaskStatus::PENDING;
    int                priority_   = 0;
    Timestamp          createdAt_;
    Timestamp          updatedAt_;
    std::optional<Timestamp> scheduledTime_;
    JsonValue          options_;
    std::vector<std::string> tags_;
};

class TaskWorker {
public:
    virtual ~TaskWorker() = default;
    virtual const std::string& getName() const = 0;
    virtual bool execute(Task& task, const State& state, const JsonValue& options) = 0;
};

class TaskManager {
public:
    TaskManager();
    ~TaskManager();

    UUID createTask(const std::string& name, const std::string& description,
                    const UUID& roomId = "", const UUID& worldId = "");
    bool scheduleTask(const UUID& taskId, const Timestamp& scheduledTime);
    bool cancelTask(const UUID& taskId);

    std::shared_ptr<Task> getTask(const UUID& taskId);
    std::vector<std::shared_ptr<Task>> getPendingTasks();
    std::vector<std::shared_ptr<Task>> getTasksByTag(const std::string& tag);

    void registerWorker(std::shared_ptr<TaskWorker> worker);
    void unregisterWorker(const std::string& workerName);

    void start();
    void stop();
    void pause();
    void resume();

private:
    void executionLoop();
    void processPendingTasks();
    bool executeTask(std::shared_ptr<Task> task);

    mutable std::mutex                                  tasksMutex_;
    std::unordered_map<UUID, std::shared_ptr<Task>>     tasks_;
    std::unordered_map<std::string, std::shared_ptr<TaskWorker>> workers_;

    std::thread          executionThread_;
    std::atomic<bool>    running_{false};
    std::atomic<bool>    paused_{false};
    std::condition_variable cv_;
    std::mutex           cvMutex_;
};

// ============================================================================
// PLN Inference Engine
// ============================================================================

struct VariableBinding {
    std::string variable;
    std::string value;
    VariableBinding(const std::string& v, const std::string& val) : variable(v), value(val) {}
};

struct InferenceRule {
    std::string name;
    std::string pattern;
    std::string conclusion;
    TruthValue  truth;
    int         priority = 0;
};

struct InferenceResult {
    std::string conclusion;
    TruthValue  truth;
    double      confidence = 0.0;
    std::vector<std::string> reasoningChain;

    InferenceResult(const std::string& c, const TruthValue& tv, double conf)
        : conclusion(c), truth(tv), confidence(conf) {}
};

class PLNInferenceEngine {
public:
    PLNInferenceEngine();

    void addRule(const InferenceRule& rule);
    void removeRule(const std::string& ruleName);
    std::vector<InferenceRule> getApplicableRules(const std::string& query) const;

    std::vector<InferenceResult> forwardChain(const State& state, const std::string& query, int maxDepth = 5);
    std::vector<InferenceResult> backwardChain(const State& state, const std::string& goal, int maxDepth = 5);
    InferenceResult bestInference(const State& state, const std::string& query);

    TruthValue combineTruthValues(const TruthValue& tv1, const TruthValue& tv2, const std::string& op);
    TruthValue propagateConfidence(const TruthValue& premise, const TruthValue& rule);

    void setAtomSpace(const std::vector<std::shared_ptr<HypergraphNode>>& nodes,
                      const std::vector<std::shared_ptr<HypergraphEdge>>& edges);
    std::vector<std::shared_ptr<HypergraphNode>> queryAtomSpace(const std::string& query);

private:
    bool unify(const std::string& pattern, const std::string& target, std::vector<VariableBinding>& bindings);
    std::string substituteVariables(const std::string& pattern, const std::vector<VariableBinding>& bindings);
    TruthValue evaluatePattern(const std::string& pattern, const State& state);

    std::vector<InferenceRule>                      rules_;
    std::vector<std::shared_ptr<HypergraphNode>>    atomSpaceNodes_;
    std::vector<std::shared_ptr<HypergraphEdge>>    atomSpaceEdges_;
    mutable std::mutex                              engineMutex_;
};

// ============================================================================
// Abstract interfaces for CognitiveFusionEngine
// ============================================================================

class SymbolicReasoner {
public:
    virtual ~SymbolicReasoner() = default;
    virtual std::vector<InferenceResult> reason(const State& state, const std::string& query) = 0;
};

class ConnectionistProcessor {
public:
    virtual ~ConnectionistProcessor() = default;
    virtual std::vector<float> process(const std::string& input) = 0;
};

class PatternMatcher {
public:
    virtual ~PatternMatcher() = default;
    virtual std::vector<std::string> match(const std::string& input, const std::vector<std::string>& patterns) = 0;
};

// ============================================================================
// Cognitive Fusion Engine
// ============================================================================

class CognitiveFusionEngine {
public:
    struct ReasoningResult {
        std::string              query;
        std::vector<InferenceResult> results;
        double                   overallConfidence = 0.0;
        std::string              summary;
        std::vector<std::shared_ptr<Memory>> relevantMemories;
    };

    CognitiveFusionEngine();

    void registerSymbolicReasoner(std::shared_ptr<SymbolicReasoner> reasoner);
    void registerConnectionistProcessor(std::shared_ptr<ConnectionistProcessor> processor);
    void registerPatternMatcher(std::shared_ptr<PatternMatcher> matcher);
    void registerPLNEngine(std::shared_ptr<PLNInferenceEngine> engine);

    ReasoningResult processQueryWithUncertainty(const State& state, const std::string& query);
    ReasoningResult processQuery(const State& state, const std::string& query);

    void integrateMemory(std::shared_ptr<Memory> memory);
    std::vector<std::shared_ptr<Memory>> retrieveRelevantMemories(const std::string& query, size_t maxResults = 10);

    void buildAtomSpaceFromMemories();
    std::vector<std::shared_ptr<HypergraphNode>> getAtomSpaceNodes() const;
    std::vector<std::shared_ptr<HypergraphEdge>> getAtomSpaceEdges() const;

private:
    TruthValue fuseResults(const std::vector<InferenceResult>& results);
    double calculateOverallConfidence(const ReasoningResult& result);

    std::shared_ptr<SymbolicReasoner>          symbolicReasoner_;
    std::shared_ptr<ConnectionistProcessor>    connectionistProcessor_;
    std::shared_ptr<PatternMatcher>            patternMatcher_;
    std::shared_ptr<PLNInferenceEngine>        plnEngine_;

    std::vector<std::shared_ptr<Memory>>        integratedMemories_;
    std::vector<std::shared_ptr<HypergraphNode>> atomSpaceNodes_;
    std::vector<std::shared_ptr<HypergraphEdge>> atomSpaceEdges_;
    mutable std::mutex                           engineMutex_;
};

} // namespace elizaos
