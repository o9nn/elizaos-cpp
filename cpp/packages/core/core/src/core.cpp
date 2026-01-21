#include "elizaos/core.hpp"
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace elizaos {

// TruthValue operations implementation
TruthValue TruthValue::conjunction(const TruthValue& other) const {
    // PLN conjunction: strength = min(s1, s2), confidence = min(c1, c2)
    return TruthValue(std::min(strength, other.strength), std::min(confidence, other.confidence));
}

TruthValue TruthValue::disjunction(const TruthValue& other) const {
    // PLN disjunction: strength = max(s1, s2), confidence = min(c1, c2)
    return TruthValue(std::max(strength, other.strength), std::min(confidence, other.confidence));
}

TruthValue TruthValue::negation() const {
    // PLN negation: strength = 1 - s, confidence = c
    return TruthValue(1.0 - strength, confidence);
}

TruthValue TruthValue::implication(const TruthValue& other) const {
    // PLN implication: strength = 1 - s1 + s1*s2, confidence = min(c1, c2)
    double implStrength = 1.0 - strength + strength * other.strength;
    return TruthValue(implStrength, std::min(confidence, other.confidence));
}

// Utility function to generate UUIDs (simplified version)
std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

// HypergraphNode implementation
HypergraphNode::HypergraphNode(const UUID& id, const std::string& label) 
    : id_(id), label_(label) {
}

void HypergraphNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::optional<std::string> HypergraphNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

// HypergraphEdge implementation
HypergraphEdge::HypergraphEdge(const UUID& id, const std::string& label, const std::vector<UUID>& nodeIds)
    : id_(id), label_(label), nodeIds_(nodeIds) {
}

// Enhanced Memory implementation
Memory::Memory(const UUID& id, const std::string& content, const UUID& entityId, const UUID& agentId)
    : id_(id), content_(content), entityId_(entityId), agentId_(agentId), 
      createdAt_(std::chrono::system_clock::now()) {
    // Default metadata
    MessageMetadata defaultMetadata;
    metadata_ = defaultMetadata;
}

Memory::Memory(const UUID& id, const std::string& content, const UUID& entityId, const UUID& agentId, 
               const MemoryMetadata& metadata)
    : id_(id), content_(content), entityId_(entityId), agentId_(agentId), 
      createdAt_(std::chrono::system_clock::now()), metadata_(metadata) {
}

// State implementation  
State::State(const AgentConfig& config) : config_(config) {
}

void State::addActor(const Actor& actor) {
    actors_.push_back(actor);
}

void State::addGoal(const Goal& goal) {
    goals_.push_back(goal);
}

void State::addRecentMessage(std::shared_ptr<Memory> memory) {
    recentMessages_.push_back(memory);
    
    // Keep only recent messages (e.g., last 32)
    const size_t maxRecentMessages = 32;
    if (recentMessages_.size() > maxRecentMessages) {
        recentMessages_.erase(recentMessages_.begin(), 
                            recentMessages_.begin() + (recentMessages_.size() - maxRecentMessages));
    }
}

// Task implementation
Task::Task(const UUID& id, const std::string& name, const std::string& description)
    : id_(id), name_(name), description_(description), 
      createdAt_(std::chrono::system_clock::now()),
      updatedAt_(std::chrono::system_clock::now()) {
}

// TaskManager implementation
TaskManager::TaskManager() {
}

TaskManager::~TaskManager() {
    stop();
}

UUID TaskManager::createTask(const std::string& name, const std::string& description, 
                            const UUID& roomId, const UUID& worldId) {
    (void)roomId; // Mark as intentionally unused
    (void)worldId; // Mark as intentionally unused
    
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    UUID taskId = generateUUID();
    auto task = std::make_shared<Task>(taskId, name, description);
    
    tasks_[taskId] = task;
    return taskId;
}

bool TaskManager::scheduleTask(const UUID& taskId, const Timestamp& scheduledTime) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second->setScheduledTime(scheduledTime);
        return true;
    }
    return false;
}

bool TaskManager::cancelTask(const UUID& taskId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        it->second->setStatus(TaskStatus::CANCELLED);
        return true;
    }
    return false;
}

std::shared_ptr<Task> TaskManager::getTask(const UUID& taskId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Task>> TaskManager::getPendingTasks() {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<Task>> pendingTasks;
    for (const auto& [id, task] : tasks_) {
        if (task->getStatus() == TaskStatus::PENDING) {
            pendingTasks.push_back(task);
        }
    }
    
    // Sort by priority (higher priority first)
    std::sort(pendingTasks.begin(), pendingTasks.end(),
              [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
                  return a->getPriority() > b->getPriority();
              });
    
    return pendingTasks;
}

std::vector<std::shared_ptr<Task>> TaskManager::getTasksByTag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<Task>> taggedTasks;
    for (const auto& [id, task] : tasks_) {
        const auto& tags = task->getTags();
        if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
            taggedTasks.push_back(task);
        }
    }
    return taggedTasks;
}

void TaskManager::registerWorker(std::shared_ptr<TaskWorker> worker) {
    std::lock_guard<std::mutex> lock(workersMutex_);
    workers_[worker->getName()] = worker;
}

void TaskManager::unregisterWorker(const std::string& workerName) {
    std::lock_guard<std::mutex> lock(workersMutex_);
    workers_.erase(workerName);
}

void TaskManager::start() {
    if (!running_) {
        running_ = true;
        paused_ = false;
        executionThread_ = std::thread(&TaskManager::executionLoop, this);
    }
}

void TaskManager::stop() {
    if (running_) {
        running_ = false;
        if (executionThread_.joinable()) {
            executionThread_.join();
        }
    }
}

void TaskManager::pause() {
    paused_ = true;
}

void TaskManager::resume() {
    paused_ = false;
}

void TaskManager::executionLoop() {
    while (running_) {
        if (!paused_) {
            processPendingTasks();
        }
        std::this_thread::sleep_for(tickInterval_);
    }
}

void TaskManager::processPendingTasks() {
    auto pendingTasks = getPendingTasks();
    auto now = std::chrono::system_clock::now();
    
    for (auto& task : pendingTasks) {
        // Check if task should be executed now
        auto scheduledTime = task->getScheduledTime();
        if (scheduledTime && scheduledTime.value() > now) {
            continue; // Not time yet
        }
        
        // Check if task has 'queue' tag (required for processing)
        const auto& tags = task->getTags();
        if (std::find(tags.begin(), tags.end(), "queue") == tags.end()) {
            continue;
        }
        
        executeTask(task);
    }
}

bool TaskManager::executeTask(std::shared_ptr<Task> task) {
    std::lock_guard<std::mutex> lock(workersMutex_);
    
    auto workerIt = workers_.find(task->getName());
    if (workerIt == workers_.end()) {
        // No worker found for this task
        return false;
    }
    
    auto worker = workerIt->second;
    task->setStatus(TaskStatus::RUNNING);
    task->updateTimestamp();
    
    try {
        // Create a dummy state for now - in real usage this would come from context
        AgentConfig dummyConfig{"", "", "", "", ""};
        State dummyState(dummyConfig);
        
        bool success = worker->execute(*task, dummyState, task->getOptions());
        
        if (success) {
            // Check if task should repeat
            const auto& tags = task->getTags();
            if (std::find(tags.begin(), tags.end(), "repeat") != tags.end()) {
                task->setStatus(TaskStatus::PENDING);
                task->updateTimestamp();
            } else {
                task->setStatus(TaskStatus::COMPLETED);
                // Remove completed non-repeating tasks
                std::lock_guard<std::mutex> taskLock(tasksMutex_);
                tasks_.erase(task->getId());
            }
        } else {
            task->setStatus(TaskStatus::FAILED);
        }
        return success;
    } catch (...) {
        task->setStatus(TaskStatus::FAILED);
        return false;
    }
}

// PLNInferenceEngine implementation
PLNInferenceEngine::PLNInferenceEngine() {
}

PLNInferenceEngine::~PLNInferenceEngine() {
}

void PLNInferenceEngine::addRule(const InferenceRule& rule) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    rules_.push_back(rule);
}

void PLNInferenceEngine::removeRule(const std::string& ruleName) {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    rules_.erase(std::remove_if(rules_.begin(), rules_.end(),
                               [&ruleName](const InferenceRule& rule) {
                                   return rule.name == ruleName;
                               }), rules_.end());
}

std::vector<InferenceRule> PLNInferenceEngine::getApplicableRules(const std::string& query) const {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    std::vector<InferenceRule> applicable;
    
    for (const auto& rule : rules_) {
        if (rule.pattern.find(query) != std::string::npos || 
            rule.conclusion.find(query) != std::string::npos) {
            applicable.push_back(rule);
        }
    }
    
    return applicable;
}

std::vector<InferenceResult> PLNInferenceEngine::forwardChain(const State& state, const std::string& query, int maxDepth) {
    std::vector<InferenceResult> results;
    
    if (maxDepth <= 0) {
        return results;
    }
    
    auto applicableRules = getApplicableRules(query);
    
    for (const auto& rule : applicableRules) {
        std::vector<VariableBinding> bindings;
        if (unify(rule.pattern, query, bindings)) {
            std::string conclusion = substituteVariables(rule.conclusion, bindings);
            TruthValue conclusionTruth = evaluatePattern(conclusion, state);
            
            // Combine rule truth with pattern evaluation
            TruthValue finalTruth = propagateConfidence(conclusionTruth, rule.truth);
            
            InferenceResult result(conclusion, finalTruth, finalTruth.confidence);
            result.reasoningChain.push_back("Applied rule: " + rule.name);
            result.reasoningChain.push_back("Pattern: " + rule.pattern);
            result.reasoningChain.push_back("Conclusion: " + conclusion);
            
            results.push_back(result);
            
            // Continue forward chaining with the new conclusion
            auto subResults = forwardChain(state, conclusion, maxDepth - 1);
            results.insert(results.end(), subResults.begin(), subResults.end());
        }
    }
    
    return results;
}

std::vector<InferenceResult> PLNInferenceEngine::backwardChain(const State& state, const std::string& goal, int maxDepth) {
    std::vector<InferenceResult> results;
    
    if (maxDepth <= 0) {
        return results;
    }
    
    auto applicableRules = getApplicableRules(goal);
    
    for (const auto& rule : applicableRules) {
        std::vector<VariableBinding> bindings;
        if (unify(rule.conclusion, goal, bindings)) {
            std::string premise = substituteVariables(rule.pattern, bindings);
            TruthValue premiseTruth = evaluatePattern(premise, state);
            
            // If premise is true, we can conclude the goal
            if (premiseTruth.strength > 0.5) {
                TruthValue finalTruth = propagateConfidence(premiseTruth, rule.truth);
                
                InferenceResult result(goal, finalTruth, finalTruth.confidence);
                result.reasoningChain.push_back("Goal: " + goal);
                result.reasoningChain.push_back("Applied rule: " + rule.name);
                result.reasoningChain.push_back("Premise: " + premise);
                
                results.push_back(result);
            } else {
                // Try to prove the premise
                auto subResults = backwardChain(state, premise, maxDepth - 1);
                for (const auto& subResult : subResults) {
                    if (subResult.truth.strength > 0.5) {
                        TruthValue finalTruth = propagateConfidence(subResult.truth, rule.truth);
                        
                        InferenceResult result(goal, finalTruth, finalTruth.confidence);
                        result.reasoningChain = subResult.reasoningChain;
                        result.reasoningChain.push_back("Applied rule: " + rule.name + " to prove: " + goal);
                        
                        results.push_back(result);
                    }
                }
            }
        }
    }
    
    return results;
}

InferenceResult PLNInferenceEngine::bestInference(const State& state, const std::string& query) {
    auto forwardResults = forwardChain(state, query, 3);
    auto backwardResults = backwardChain(state, query, 3);
    
    // Combine results
    std::vector<InferenceResult> allResults = forwardResults;
    allResults.insert(allResults.end(), backwardResults.begin(), backwardResults.end());
    
    if (allResults.empty()) {
        return InferenceResult(query, TruthValue(0.0, 0.0), 0.0);
    }
    
    // Find the result with highest confidence
    auto best = std::max_element(allResults.begin(), allResults.end(),
                                [](const InferenceResult& a, const InferenceResult& b) {
                                    return a.confidence < b.confidence;
                                });
    
    return *best;
}

TruthValue PLNInferenceEngine::combineTruthValues(const TruthValue& tv1, const TruthValue& tv2, const std::string& operation) {
    if (operation == "AND" || operation == "conjunction") {
        return tv1.conjunction(tv2);
    } else if (operation == "OR" || operation == "disjunction") {
        return tv1.disjunction(tv2);
    } else if (operation == "IMPLIES" || operation == "implication") {
        return tv1.implication(tv2);
    }
    
    // Default: return the one with higher confidence
    return tv1.confidence > tv2.confidence ? tv1 : tv2;
}

TruthValue PLNInferenceEngine::propagateConfidence(const TruthValue& premise, const TruthValue& rule) {
    // Confidence propagation: multiply strengths and take minimum confidence
    double newStrength = premise.strength * rule.strength;
    double newConfidence = std::min(premise.confidence, rule.confidence);
    return TruthValue(newStrength, newConfidence);
}

void PLNInferenceEngine::setAtomSpace(const std::vector<std::shared_ptr<HypergraphNode>>& nodes,
                                     const std::vector<std::shared_ptr<HypergraphEdge>>& edges) {
    std::lock_guard<std::mutex> lock(atomSpaceMutex_);
    atomSpaceNodes_ = nodes;
    atomSpaceEdges_ = edges;
}

std::vector<std::shared_ptr<HypergraphNode>> PLNInferenceEngine::queryAtomSpace(const std::string& query) {
    std::lock_guard<std::mutex> lock(atomSpaceMutex_);
    std::vector<std::shared_ptr<HypergraphNode>> results;
    
    for (const auto& node : atomSpaceNodes_) {
        if (node->getLabel().find(query) != std::string::npos) {
            results.push_back(node);
        }
    }
    
    return results;
}

bool PLNInferenceEngine::unify(const std::string& pattern, const std::string& target, std::vector<VariableBinding>& bindings) {
    // Simple unification - check if pattern matches target
    // In a full implementation, this would handle variables and more complex patterns
    if (pattern == target) {
        return true;
    }
    
    // Check for variable patterns (starting with ?)
    if (pattern.length() > 1 && pattern[0] == '?') {
        std::string variable = pattern.substr(1);
        bindings.push_back(VariableBinding(variable, target));
        return true;
    }
    
    return false;
}

std::string PLNInferenceEngine::substituteVariables(const std::string& pattern, const std::vector<VariableBinding>& bindings) {
    std::string result = pattern;
    
    for (const auto& binding : bindings) {
        std::string varPattern = "?" + binding.variable;
        size_t pos = result.find(varPattern);
        if (pos != std::string::npos) {
            result.replace(pos, varPattern.length(), binding.value);
        }
    }
    
    return result;
}

TruthValue PLNInferenceEngine::evaluatePattern(const std::string& pattern, const State& state) {
    // Simple pattern evaluation based on content matching
    // In a full implementation, this would be more sophisticated
    
    // Check if pattern matches agent name or bio
    if (pattern.find(state.getAgentName()) != std::string::npos) {
        return TruthValue(0.9, 0.8);
    }
    
    if (pattern.find(state.getBio()) != std::string::npos) {
        return TruthValue(0.8, 0.7);
    }
    
    // Check against recent messages
    for (const auto& memory : state.getRecentMessages()) {
        if (memory->getContent().find(pattern) != std::string::npos) {
            return TruthValue(0.7, 0.6);
        }
    }
    
    return TruthValue(0.1, 0.1);  // Default low confidence
}

// CognitiveFusionEngine implementation
CognitiveFusionEngine::CognitiveFusionEngine() {
    plnEngine_ = std::make_shared<PLNInferenceEngine>();
}

CognitiveFusionEngine::~CognitiveFusionEngine() {
}

void CognitiveFusionEngine::registerSymbolicReasoner(std::shared_ptr<SymbolicReasoner> reasoner) {
    std::lock_guard<std::mutex> lock(reasonersMutex_);
    symbolicReasoners_.push_back(reasoner);
}

void CognitiveFusionEngine::registerConnectionistProcessor(std::shared_ptr<ConnectionistProcessor> processor) {
    std::lock_guard<std::mutex> lock(processorsMutex_);
    connectionistProcessors_.push_back(processor);
}

void CognitiveFusionEngine::registerPatternMatcher(std::shared_ptr<PatternMatcher> matcher) {
    std::lock_guard<std::mutex> lock(matchersMutex_);
    patternMatchers_.push_back(matcher);
}

void CognitiveFusionEngine::registerPLNEngine(std::shared_ptr<PLNInferenceEngine> engine) {
    plnEngine_ = engine;
}

CognitiveFusionEngine::ReasoningResult CognitiveFusionEngine::processQueryWithUncertainty(const State& state, const std::string& query) {
    ReasoningResult result;
    result.confidence = 0.0;
    result.overallTruth = TruthValue(0.0, 0.0);
    
    // Process with PLN engine
    if (plnEngine_) {
        result.plnResults = plnEngine_->forwardChain(state, query, 3);
        
        // Also try backward chaining
        auto backwardResults = plnEngine_->backwardChain(state, query, 3);
        result.plnResults.insert(result.plnResults.end(), backwardResults.begin(), backwardResults.end());
    }
    
    // Process with symbolic reasoners (extended for uncertainty)
    {
        std::lock_guard<std::mutex> lock(reasonersMutex_);
        for (const auto& reasoner : symbolicReasoners_) {
            auto symbolicResults = reasoner->reason(state, query);
            result.symbolicResults.insert(result.symbolicResults.end(), 
                                        symbolicResults.begin(), symbolicResults.end());
            
            // Try uncertainty-aware reasoning if supported
            try {
                auto uncertaintyResults = reasoner->reasonWithUncertainty(state, query);
                result.plnResults.insert(result.plnResults.end(), 
                                       uncertaintyResults.begin(), uncertaintyResults.end());
            } catch (...) {
                // Fallback to basic reasoning if uncertainty not supported
            }
        }
    }
    
    // Process with connectionist processors
    {
        std::lock_guard<std::mutex> lock(processorsMutex_);
        for (const auto& processor : connectionistProcessors_) {
            auto embedding = processor->generateEmbedding(query);
            auto connectionistResults = processor->generateResponse(embedding);
            result.connectionistResults.insert(result.connectionistResults.end(),
                                             connectionistResults.begin(), connectionistResults.end());
        }
    }
    
    // Process with pattern matchers for AtomSpace
    {
        std::lock_guard<std::mutex> lock(matchersMutex_);
        for (const auto& matcher : patternMatchers_) {
            try {
                AtomSpacePattern pattern(query);
                auto matches = matcher->findAllMatches(pattern, atomSpaceNodes_, atomSpaceEdges_);
                result.patternMatches.insert(result.patternMatches.end(), 
                                           matches.begin(), matches.end());
            } catch (...) {
                // Fallback to basic pattern matching
                auto patterns = matcher->extractPatterns(query);
                // Convert to basic results
                for (const auto& p : patterns) {
                    result.fusedResults.push_back(p);
                }
            }
        }
    }
    
    // Fusion logic with uncertainty
    result.fusedResults = result.symbolicResults;
    result.fusedResults.insert(result.fusedResults.end(), 
                              result.connectionistResults.begin(), result.connectionistResults.end());
    
    // Add PLN results to fused results
    for (const auto& plnResult : result.plnResults) {
        result.fusedResults.push_back(plnResult.conclusion);
    }
    
    // Add pattern match results
    for (const auto& match : result.patternMatches) {
        if (match.isMatch) {
            result.fusedResults.push_back(match.matchedPattern);
        }
    }
    
    // Calculate overall truth value and confidence
    result.overallTruth = fuseResults(result.plnResults);
    result.confidence = calculateOverallConfidence(result);
    
    return result;
}

void CognitiveFusionEngine::buildAtomSpaceFromMemories() {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    std::lock_guard<std::mutex> atomLock(atomSpaceMutex_);
    
    atomSpaceNodes_.clear();
    atomSpaceEdges_.clear();
    
    // Convert memories to AtomSpace nodes
    for (const auto& memory : memoryStore_) {
        auto node = std::make_shared<HypergraphNode>(memory->getId(), memory->getContent());
        
        // Add metadata as attributes
        node->setAttribute("agent_id", memory->getAgentId());
        node->setAttribute("entity_id", memory->getEntityId());
        node->setAttribute("room_id", memory->getRoomId());
        
        // Add embedding if available
        if (memory->getEmbedding().has_value()) {
            const auto& embedding = memory->getEmbedding().value();
            std::string embeddingStr = "";
            for (float val : embedding) {
                embeddingStr += std::to_string(val) + ",";
            }
            node->setAttribute("embedding", embeddingStr);
        }
        
        atomSpaceNodes_.push_back(node);
    }
    
    // Create edges between related memories
    for (size_t i = 0; i < memoryStore_.size(); ++i) {
        for (size_t j = i + 1; j < memoryStore_.size(); ++j) {
            const auto& mem1 = memoryStore_[i];
            const auto& mem2 = memoryStore_[j];
            
            // Create edge if memories share agent or entity
            if (mem1->getAgentId() == mem2->getAgentId() || 
                mem1->getEntityId() == mem2->getEntityId()) {
                
                std::string edgeId = mem1->getId() + "_" + mem2->getId();
                std::vector<UUID> nodeIds = {mem1->getId(), mem2->getId()};
                
                auto edge = std::make_shared<HypergraphEdge>(edgeId, "relates", nodeIds);
                
                // Set weight based on similarity if embeddings are available
                if (mem1->getEmbedding().has_value() && mem2->getEmbedding().has_value()) {
                    // Simple cosine similarity
                    const auto& emb1 = mem1->getEmbedding().value();
                    const auto& emb2 = mem2->getEmbedding().value();
                    
                    double similarity = 0.0;
                    double norm1 = 0.0, norm2 = 0.0;
                    
                    for (size_t k = 0; k < std::min(emb1.size(), emb2.size()); ++k) {
                        similarity += emb1[k] * emb2[k];
                        norm1 += emb1[k] * emb1[k];
                        norm2 += emb2[k] * emb2[k];
                    }
                    
                    if (norm1 > 0 && norm2 > 0) {
                        similarity /= (std::sqrt(norm1) * std::sqrt(norm2));
                        edge->setWeight(similarity);
                    }
                }
                
                atomSpaceEdges_.push_back(edge);
            }
        }
    }
    
    // Update PLN engine with new AtomSpace
    if (plnEngine_) {
        plnEngine_->setAtomSpace(atomSpaceNodes_, atomSpaceEdges_);
    }
}

std::vector<std::shared_ptr<HypergraphNode>> CognitiveFusionEngine::getAtomSpaceNodes() const {
    std::lock_guard<std::mutex> lock(atomSpaceMutex_);
    return atomSpaceNodes_;
}

std::vector<std::shared_ptr<HypergraphEdge>> CognitiveFusionEngine::getAtomSpaceEdges() const {
    std::lock_guard<std::mutex> lock(atomSpaceMutex_);
    return atomSpaceEdges_;
}

TruthValue CognitiveFusionEngine::fuseResults(const std::vector<InferenceResult>& results) {
    if (results.empty()) {
        return TruthValue(0.0, 0.0);
    }
    
    // Combine truth values using weighted average
    double totalStrength = 0.0;
    double totalConfidence = 0.0;
    double weightSum = 0.0;
    
    for (const auto& result : results) {
        double weight = result.confidence;
        totalStrength += result.truth.strength * weight;
        totalConfidence += result.truth.confidence * weight;
        weightSum += weight;
    }
    
    if (weightSum > 0) {
        return TruthValue(totalStrength / weightSum, totalConfidence / weightSum);
    }
    
    return TruthValue(0.0, 0.0);
}

double CognitiveFusionEngine::calculateOverallConfidence(const ReasoningResult& result) {
    double confidence = 0.0;
    int count = 0;
    
    // Factor in PLN results
    for (const auto& plnResult : result.plnResults) {
        confidence += plnResult.confidence;
        count++;
    }
    
    // Factor in pattern matches
    for (const auto& match : result.patternMatches) {
        if (match.isMatch) {
            confidence += match.confidence;
            count++;
        }
    }
    
    // Factor in number of supporting results - always add this if we have results
    if (!result.fusedResults.empty()) {
        double resultsConfidence = std::min(1.0, static_cast<double>(result.fusedResults.size()) / 5.0);
        confidence += resultsConfidence;
        count++;
    }
    
    // If we have symbolic or connectionist results but no other confidence measures,
    // provide a minimal baseline confidence
    if (count == 0 && (!result.symbolicResults.empty() || !result.connectionistResults.empty())) {
        confidence = 0.1;  // Minimal confidence for having some results
        count = 1;
    }
    
    return count > 0 ? confidence / count : 0.0;
}

CognitiveFusionEngine::ReasoningResult CognitiveFusionEngine::processQuery(const State& state, const std::string& query) {
    ReasoningResult result;
    result.confidence = 0.0;
    
    // Process with symbolic reasoners
    {
        std::lock_guard<std::mutex> lock(reasonersMutex_);
        for (const auto& reasoner : symbolicReasoners_) {
            auto symbolicResults = reasoner->reason(state, query);
            result.symbolicResults.insert(result.symbolicResults.end(), 
                                        symbolicResults.begin(), symbolicResults.end());
        }
    }
    
    // Process with connectionist processors
    {
        std::lock_guard<std::mutex> lock(processorsMutex_);
        for (const auto& processor : connectionistProcessors_) {
            auto embedding = processor->generateEmbedding(query);
            auto connectionistResults = processor->generateResponse(embedding);
            result.connectionistResults.insert(result.connectionistResults.end(),
                                             connectionistResults.begin(), connectionistResults.end());
        }
    }
    
    // Fusion logic - combine symbolic and connectionist results
    // Simple fusion strategy: merge results and compute confidence based on agreement
    result.fusedResults = result.symbolicResults;
    result.fusedResults.insert(result.fusedResults.end(), 
                              result.connectionistResults.begin(), result.connectionistResults.end());
    
    // Calculate confidence based on number of supporting results
    if (!result.fusedResults.empty()) {
        result.confidence = std::min(1.0, static_cast<double>(result.fusedResults.size()) / 5.0);
    }
    
    return result;
}

void CognitiveFusionEngine::integrateMemory(std::shared_ptr<Memory> memory) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    memoryStore_.push_back(memory);
}

std::vector<std::shared_ptr<Memory>> CognitiveFusionEngine::retrieveRelevantMemories(const std::string& query, size_t maxResults) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    std::vector<std::shared_ptr<Memory>> relevantMemories;
    
    // Simple relevance check based on content matching
    for (const auto& memory : memoryStore_) {
        if (memory->getContent().find(query) != std::string::npos) {
            relevantMemories.push_back(memory);
            if (relevantMemories.size() >= maxResults) {
                break;
            }
        }
    }
    
    return relevantMemories;
}

} // namespace elizaos