#include "elizaos/core.hpp"
#include <cctype>
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

// Utility std::function to generate UUIDs (simplified version)
std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    static std::mutex uuidMutex;
    std::lock_guard<std::mutex> lock(uuidMutex);
    
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

void State::addGoal(const StateGoal& goal) {
    goals_.push_back(goal);
}

namespace {
// Case-insensitive ASCII equality used by the goal-status helpers so that
// callers can match "Active"/"active"/"ACTIVE" interchangeably.
bool iequalsAscii(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}
} // namespace

bool State::updateGoalStatus(const UUID& goalId, const std::string& status) {
    for (auto& goal : goals_) {
        if (goal.id == goalId) {
            goal.status = status;
            goal.updatedAt = std::chrono::system_clock::now();
            return true;
        }
    }
    return false;
}

bool State::updateGoalStatusByDescription(const std::string& description,
                                          const std::string& status) {
    for (auto& goal : goals_) {
        if (iequalsAscii(goal.description, description)) {
            goal.status = status;
            goal.updatedAt = std::chrono::system_clock::now();
            return true;
        }
    }
    return false;
}

std::size_t State::countGoalsWithStatus(const std::string& status) const {
    std::size_t count = 0;
    for (const auto& goal : goals_) {
        if (iequalsAscii(goal.status, status)) {
            ++count;
        }
    }
    return count;
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
    : Task(id, name, description, "", "") {
}

Task::Task(const UUID& id, const std::string& name, const std::string& description,
           const UUID& roomId, const UUID& worldId)
    : id_(id), name_(name), description_(description), roomId_(roomId), worldId_(worldId),
      createdAt_(std::chrono::system_clock::now()),
      updatedAt_(std::chrono::system_clock::now()) {
}

Task::Task(const Task& other) {
    std::lock_guard<std::mutex> lock(other.mutex_);
    id_ = other.id_;
    name_ = other.name_;
    description_ = other.description_;
    roomId_ = other.roomId_;
    worldId_ = other.worldId_;
    status_ = other.status_;
    tags_ = other.tags_;
    options_ = other.options_;
    createdAt_ = other.createdAt_;
    updatedAt_ = other.updatedAt_;
    scheduledTime_ = other.scheduledTime_;
    priority_ = other.priority_;
}

Task& Task::operator=(const Task& other) {
    if (this == &other) {
        return *this;
    }

    std::scoped_lock lock(mutex_, other.mutex_);
    id_ = other.id_;
    name_ = other.name_;
    description_ = other.description_;
    roomId_ = other.roomId_;
    worldId_ = other.worldId_;
    status_ = other.status_;
    tags_ = other.tags_;
    options_ = other.options_;
    createdAt_ = other.createdAt_;
    updatedAt_ = other.updatedAt_;
    scheduledTime_ = other.scheduledTime_;
    priority_ = other.priority_;
    return *this;
}

TaskStatus Task::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

void Task::setStatus(TaskStatus status) {
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = status;
    updatedAt_ = std::chrono::system_clock::now();
}

bool Task::transitionStatus(TaskStatus expected, TaskStatus desired) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != expected) {
        return false;
    }
    status_ = desired;
    updatedAt_ = std::chrono::system_clock::now();
    return true;
}

std::vector<std::string> Task::getTagsSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tags_;
}

void Task::addTag(const std::string& tag) {
    std::lock_guard<std::mutex> lock(mutex_);
    tags_.push_back(tag);
    updatedAt_ = std::chrono::system_clock::now();
}

TaskOptions Task::getOptionsSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return options_;
}

void Task::setOptions(const TaskOptions& options) {
    std::lock_guard<std::mutex> lock(mutex_);
    options_ = options;
    updatedAt_ = std::chrono::system_clock::now();
}

Timestamp Task::getUpdatedAt() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return updatedAt_;
}

void Task::updateTimestamp() {
    std::lock_guard<std::mutex> lock(mutex_);
    updatedAt_ = std::chrono::system_clock::now();
}

std::optional<Timestamp> Task::getScheduledTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return scheduledTime_;
}

void Task::setScheduledTime(const Timestamp& time) {
    std::lock_guard<std::mutex> lock(mutex_);
    scheduledTime_ = time;
    updatedAt_ = std::chrono::system_clock::now();
}

int Task::getPriority() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return priority_;
}

void Task::setPriority(int priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    priority_ = priority;
    updatedAt_ = std::chrono::system_clock::now();
}

TaskSnapshot Task::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return TaskSnapshot{id_, name_, description_, roomId_, worldId_, status_,
                        tags_, options_, createdAt_, updatedAt_, scheduledTime_,
                        priority_};
}

// TaskManager implementation
TaskManager::TaskManager()
    : TaskManager(AgentConfig{"task-manager", "TaskManager",
                              "Core task orchestration context",
                              "Executes validated managed tasks",
                              "orchestrating"}) {
}

TaskManager::TaskManager(AgentConfig config) {
    setAgentConfig(config);
}

TaskManager::TaskManager(StateFactory stateFactory) {
    setStateFactory(std::move(stateFactory));
}

TaskManager::~TaskManager() {
    stop();
}

UUID TaskManager::createTask(const std::string& name, const std::string& description, 
                            const UUID& roomId, const UUID& worldId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    UUID taskId = generateUUID();
    auto task = std::make_shared<Task>(taskId, name, description, roomId, worldId);
    
    tasks_[taskId] = task;
    return taskId;
}

bool TaskManager::scheduleTask(const UUID& taskId, const Timestamp& scheduledTime) {
    {
        std::lock_guard<std::mutex> lock(tasksMutex_);
        auto it = tasks_.find(taskId);
        if (it == tasks_.end()) {
            return false;
        }
        it->second->setScheduledTime(scheduledTime);
    }
    lifecycleCv_.notify_all();
    return true;
}

bool TaskManager::cancelTask(const UUID& taskId) {
    std::shared_ptr<Task> task;
    {
        std::lock_guard<std::mutex> lock(tasksMutex_);
        auto it = tasks_.find(taskId);
        if (it == tasks_.end()) {
            return false;
        }
        task = it->second;
    }
    return task->transitionStatus(TaskStatus::PENDING, TaskStatus::CANCELLED);
}

std::shared_ptr<Task> TaskManager::getTask(const UUID& taskId) {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    auto it = tasks_.find(taskId);
    if (it != tasks_.end()) {
        return it->second;
    }
    return nullptr;
}

std::optional<TaskSnapshot> TaskManager::getTaskSnapshot(const UUID& taskId) const {
    std::shared_ptr<Task> task;
    {
        std::lock_guard<std::mutex> lock(tasksMutex_);
        auto it = tasks_.find(taskId);
        if (it == tasks_.end()) {
            return std::nullopt;
        }
        task = it->second;
    }
    return task->snapshot();
}

std::vector<std::shared_ptr<Task>> TaskManager::getPendingTasks() {
    std::lock_guard<std::mutex> lock(tasksMutex_);
    
    std::vector<std::shared_ptr<Task>> pendingTasks;
    for (const auto& entry : tasks_) {
        const auto& task = entry.second;
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
    for (const auto& entry : tasks_) {
        const auto& task = entry.second;
        const auto tags = task->getTagsSnapshot();
        if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
            taggedTasks.push_back(task);
        }
    }
    return taggedTasks;
}

void TaskManager::registerWorker(std::shared_ptr<TaskWorker> worker) {
    if (!worker) {
        return;
    }
    const auto workerName = worker->getName();
    std::lock_guard<std::mutex> lock(workersMutex_);
    workers_[workerName] = std::move(worker);
}

void TaskManager::unregisterWorker(const std::string& workerName) {
    std::lock_guard<std::mutex> lock(workersMutex_);
    workers_.erase(workerName);
}

void TaskManager::start() {
    std::lock_guard<std::mutex> controlLock(controlMutex_);
    std::thread staleThread;
    std::unique_lock<std::mutex> lock(lifecycleMutex_);
    if (running_) {
        return;
    }
    if (executionThread_.joinable()) {
        if (executionThreadId_ == std::this_thread::get_id()) {
            return;
        }
        // A callback may have requested self-stop, leaving its now-finished
        // thread joinable. Move it out while holding lifecycleMutex_ before
        // joining so no other caller can race over executionThread_.
        staleThread = std::move(executionThread_);
        lock.unlock();
        staleThread.join();
        lock.lock();
        if (running_) {
            return;
        }
    }
    running_ = true;
    paused_ = false;
    ++wakeGeneration_;
    executionThread_ = std::thread(&TaskManager::executionLoop, this);
    lock.unlock();
    lifecycleCv_.notify_all();
}

void TaskManager::stop() {
    std::unique_lock<std::mutex> controlLock(controlMutex_);
    std::thread threadToJoin;
    {
        std::unique_lock<std::mutex> lock(lifecycleMutex_);
        if (executionThreadId_ == std::this_thread::get_id()) {
            running_ = false;
            paused_ = false;
            ++wakeGeneration_;
            lock.unlock();
            lifecycleCv_.notify_all();
            return;
        }

        if (!executionThread_.joinable()) {
            running_ = false;
            paused_ = false;
            return;
        }

        running_ = false;
        paused_ = false;
        ++wakeGeneration_;
        threadToJoin = std::move(executionThread_);
    }
    lifecycleCv_.notify_all();

    threadToJoin.join();

    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        executionThreadId_ = std::thread::id{};
    }
    lifecycleCv_.notify_all();
}

void TaskManager::pause() {
    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        paused_ = true;
        ++wakeGeneration_;
    }
    lifecycleCv_.notify_all();
}

void TaskManager::resume() {
    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        paused_ = false;
        ++wakeGeneration_;
    }
    lifecycleCv_.notify_all();
}

bool TaskManager::isRunning() const {
    std::lock_guard<std::mutex> lock(lifecycleMutex_);
    return running_;
}

void TaskManager::setTickInterval(std::chrono::milliseconds interval) {
    {
        std::lock_guard<std::mutex> lock(lifecycleMutex_);
        tickInterval_ = std::max(interval, std::chrono::milliseconds(1));
        ++wakeGeneration_;
    }
    lifecycleCv_.notify_all();
}

void TaskManager::setAgentConfig(const AgentConfig& config) {
    setStateFactory([config](const TaskSnapshot&) {
        return std::make_shared<State>(config);
    });
}

void TaskManager::setStateFactory(StateFactory stateFactory) {
    std::lock_guard<std::mutex> lock(stateFactoryMutex_);
    stateFactory_ = std::move(stateFactory);
}

void TaskManager::executionLoop() {
    std::unique_lock<std::mutex> lock(lifecycleMutex_);
    executionThreadId_ = std::this_thread::get_id();

    while (running_) {
        if (paused_) {
            lifecycleCv_.wait(lock, [this] { return !running_ || !paused_; });
            continue;
        }

        const auto interval = tickInterval_;
        const auto generation = wakeGeneration_;
        lock.unlock();
        processPendingTasks();
        lock.lock();

        lifecycleCv_.wait_for(lock, interval, [this, generation] {
            return !running_ || paused_ || wakeGeneration_ != generation;
        });
    }

    executionThreadId_ = std::thread::id{};
    lifecycleCv_.notify_all();
}

void TaskManager::processPendingTasks() {
    auto pendingTasks = getPendingTasks();
    auto now = std::chrono::system_clock::now();
    
    for (auto& task : pendingTasks) {
        const auto taskSnapshot = task->snapshot();
        // Check if task should be executed now
        const auto& scheduledTime = taskSnapshot.scheduledTime;
        if (scheduledTime && scheduledTime.value() > now) {
            continue; // Not time yet
        }
        
        // Check if task has 'queue' tag (required for processing)
        const auto& tags = taskSnapshot.tags;
        if (std::find(tags.begin(), tags.end(), "queue") == tags.end()) {
            continue;
        }
        
        executeTask(task);
    }
}

bool TaskManager::executeTask(std::shared_ptr<Task> task) {
    if (!task) {
        return false;
    }

    const auto initialSnapshot = task->snapshot();
    std::shared_ptr<TaskWorker> worker;
    {
        std::lock_guard<std::mutex> lock(workersMutex_);
        auto workerIt = workers_.find(initialSnapshot.name);
        if (workerIt == workers_.end()) {
            // A task with no matching worker remains pending for a future
            // registration rather than reporting work that never ran as failed.
            return false;
        }
        worker = workerIt->second;
    }

    // Claim exactly one pending task before creating context or invoking user
    // code. Cancellation racing with this transition wins cleanly.
    if (!task->transitionStatus(TaskStatus::PENDING, TaskStatus::RUNNING)) {
        return false;
    }

    try {
        StateFactory stateFactory;
        {
            std::lock_guard<std::mutex> lock(stateFactoryMutex_);
            stateFactory = stateFactory_;
        }
        if (!stateFactory) {
            task->transitionStatus(TaskStatus::RUNNING, TaskStatus::FAILED);
            return false;
        }

        const auto claimedSnapshot = task->snapshot();
        auto state = stateFactory(claimedSnapshot);
        if (!state) {
            task->transitionStatus(TaskStatus::RUNNING, TaskStatus::FAILED);
            return false;
        }

        MessageMetadata metadata;
        metadata.source = "task_manager";
        metadata.scope = MemoryScope::ROOM;
        metadata.tags = {"task", claimedSnapshot.name};
        auto message = std::make_shared<Memory>(
            claimedSnapshot.id,
            claimedSnapshot.description,
            claimedSnapshot.roomId,
            state->getAgentId(),
            metadata);
        message->setRoomId(claimedSnapshot.roomId);
        message->setWorldId(claimedSnapshot.worldId);
        state->addRecentMessage(message);

        // Validation is a real gate and receives the same state/message context
        // that execution will use. Rejection is terminal for this attempt.
        if (!worker->validate(*task, *state, message)) {
            task->transitionStatus(TaskStatus::RUNNING, TaskStatus::FAILED);
            return false;
        }

        const bool success = worker->execute(
            *task, *state, claimedSnapshot.options);

        if (success) {
            // Check if task should repeat
            const auto tags = task->getTagsSnapshot();
            if (std::find(tags.begin(), tags.end(), "repeat") != tags.end()) {
                task->transitionStatus(TaskStatus::RUNNING, TaskStatus::PENDING);
            } else {
                if (task->transitionStatus(TaskStatus::RUNNING,
                                           TaskStatus::COMPLETED)) {
                    // Remove completed non-repeating tasks without holding a
                    // manager mutex while user code runs.
                    std::lock_guard<std::mutex> taskLock(tasksMutex_);
                    auto it = tasks_.find(initialSnapshot.id);
                    if (it != tasks_.end() && it->second == task) {
                        tasks_.erase(it);
                    }
                }
            }
        } else {
            task->transitionStatus(TaskStatus::RUNNING, TaskStatus::FAILED);
        }
        return success;
    } catch (...) {
        task->transitionStatus(TaskStatus::RUNNING, TaskStatus::FAILED);
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

namespace {
std::vector<std::string> splitTerms(const std::string& value) {
    std::istringstream stream(value);
    std::vector<std::string> terms;
    std::string term;
    while (stream >> term) {
        terms.push_back(term);
    }
    return terms;
}

bool variableAwareMatch(const std::string& pattern,
                        const std::string& target,
                        std::vector<VariableBinding>* bindings = nullptr) {
    if (pattern == target) {
        return true;
    }

    const auto patternTerms = splitTerms(pattern);
    const auto targetTerms = splitTerms(target);
    if (patternTerms.empty() || patternTerms.size() != targetTerms.size()) {
        return false;
    }

    std::unordered_map<std::string, std::string> localBindings;
    for (size_t i = 0; i < patternTerms.size(); ++i) {
        const auto& patternTerm = patternTerms[i];
        const auto& targetTerm = targetTerms[i];
        if (patternTerm.size() > 1 && patternTerm.front() == '?') {
            const std::string variable = patternTerm.substr(1);
            auto existing = localBindings.find(variable);
            if (existing != localBindings.end() && existing->second != targetTerm) {
                return false;
            }
            localBindings[variable] = targetTerm;
            continue;
        }
        if (patternTerm != targetTerm) {
            return false;
        }
    }

    if (bindings) {
        for (const auto& [variable, value] : localBindings) {
            bindings->push_back(VariableBinding(variable, value));
        }
    }
    return true;
}
}

std::vector<InferenceRule> PLNInferenceEngine::getApplicableRules(const std::string& query) const {
    std::lock_guard<std::mutex> lock(rulesMutex_);
    std::vector<InferenceRule> applicable;
    
    for (const auto& rule : rules_) {
        if (variableAwareMatch(rule.pattern, query) ||
            variableAwareMatch(rule.conclusion, query) ||
            rule.pattern.find(query) != std::string::npos ||
            rule.conclusion.find(query) != std::string::npos ||
            query.find(rule.pattern) != std::string::npos ||
            query.find(rule.conclusion) != std::string::npos) {
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
    if (variableAwareMatch(pattern, target, &bindings)) {
        return true;
    }
    
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
        while (pos != std::string::npos) {
            result.replace(pos, varPattern.length(), binding.value);
            pos = result.find(varPattern, pos + binding.value.length());
        }
    }
    
    return result;
}

TruthValue PLNInferenceEngine::evaluatePattern(const std::string& pattern, const State& state) {
    // Simple pattern evaluation based on content matching
    // In a full implementation, this would be more sophisticated
    
    // Check if pattern matches non-empty agent identity fields.
    if (!state.getAgentName().empty() && pattern.find(state.getAgentName()) != std::string::npos) {
        return TruthValue(0.9, 0.8);
    }
    
    if (!state.getBio().empty() && pattern.find(state.getBio()) != std::string::npos) {
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
