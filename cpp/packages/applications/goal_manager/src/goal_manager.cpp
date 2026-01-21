// goal_manager.cpp - Enhanced Goal Management System for ElizaOS
// Provides comprehensive goal tracking with priorities, dependencies, and progress

#include "elizaos/goal_manager.hpp"
#include <algorithm>
#include <sstream>
#include <random>
#include <queue>

namespace elizaos {

// ==============================================================================
// UTILITY FUNCTIONS
// ==============================================================================

std::string goalStatusToString(GoalStatus status) {
    switch (status) {
        case GoalStatus::PENDING: return "PENDING";
        case GoalStatus::ACTIVE: return "ACTIVE";
        case GoalStatus::IN_PROGRESS: return "IN_PROGRESS";
        case GoalStatus::BLOCKED: return "BLOCKED";
        case GoalStatus::COMPLETED: return "COMPLETED";
        case GoalStatus::FAILED: return "FAILED";
        case GoalStatus::CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

GoalStatus stringToGoalStatus(const std::string& str) {
    if (str == "PENDING") return GoalStatus::PENDING;
    if (str == "ACTIVE") return GoalStatus::ACTIVE;
    if (str == "IN_PROGRESS") return GoalStatus::IN_PROGRESS;
    if (str == "BLOCKED") return GoalStatus::BLOCKED;
    if (str == "COMPLETED") return GoalStatus::COMPLETED;
    if (str == "FAILED") return GoalStatus::FAILED;
    if (str == "CANCELLED") return GoalStatus::CANCELLED;
    return GoalStatus::PENDING;
}

std::string goalPriorityToString(GoalPriority priority) {
    switch (priority) {
        case GoalPriority::CRITICAL: return "CRITICAL";
        case GoalPriority::HIGH: return "HIGH";
        case GoalPriority::NORMAL: return "NORMAL";
        case GoalPriority::LOW: return "LOW";
        case GoalPriority::BACKGROUND: return "BACKGROUND";
        default: return "NORMAL";
    }
}

GoalPriority stringToGoalPriority(const std::string& str) {
    if (str == "CRITICAL") return GoalPriority::CRITICAL;
    if (str == "HIGH") return GoalPriority::HIGH;
    if (str == "NORMAL") return GoalPriority::NORMAL;
    if (str == "LOW") return GoalPriority::LOW;
    if (str == "BACKGROUND") return GoalPriority::BACKGROUND;
    return GoalPriority::NORMAL;
}

std::string goalTypeToString(GoalType type) {
    switch (type) {
        case GoalType::ACHIEVEMENT: return "ACHIEVEMENT";
        case GoalType::MAINTENANCE: return "MAINTENANCE";
        case GoalType::IMPROVEMENT: return "IMPROVEMENT";
        case GoalType::EXPLORATION: return "EXPLORATION";
        case GoalType::REACTIVE: return "REACTIVE";
        default: return "ACHIEVEMENT";
    }
}

GoalType stringToGoalType(const std::string& str) {
    if (str == "ACHIEVEMENT") return GoalType::ACHIEVEMENT;
    if (str == "MAINTENANCE") return GoalType::MAINTENANCE;
    if (str == "IMPROVEMENT") return GoalType::IMPROVEMENT;
    if (str == "EXPLORATION") return GoalType::EXPLORATION;
    if (str == "REACTIVE") return GoalType::REACTIVE;
    return GoalType::ACHIEVEMENT;
}

// ==============================================================================
// GOAL IMPLEMENTATION
// ==============================================================================

Goal::Goal(const std::string& name, const std::string& description)
    : id_(generateId())
    , name_(name)
    , description_(description)
    , createdAt_(std::chrono::system_clock::now())
    , updatedAt_(createdAt_) {}

Goal::Goal(const UUID& id, const std::string& name, const std::string& description)
    : id_(id)
    , name_(name)
    , description_(description)
    , createdAt_(std::chrono::system_clock::now())
    , updatedAt_(createdAt_) {}

UUID Goal::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    return "goal_" + std::to_string(dis(gen)) + "_" +
           std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000000);
}

void Goal::updateTimestamp() {
    updatedAt_ = std::chrono::system_clock::now();
}

void Goal::setStatus(GoalStatus status) {
    status_ = status;
    updateTimestamp();

    if (status == GoalStatus::COMPLETED) {
        completedAt_ = std::chrono::system_clock::now();
        progress_ = 1.0;
    }
}

void Goal::setProgress(double progress) {
    progress_ = std::clamp(progress, 0.0, 1.0);
    updateTimestamp();

    if (progress_ >= 1.0 && status_ != GoalStatus::COMPLETED) {
        status_ = GoalStatus::IN_PROGRESS; // Will be completed by manager
    }
}

void Goal::incrementProgress(double amount) {
    setProgress(progress_ + amount);
}

void Goal::addDependency(const UUID& goalId) {
    dependencies_.insert(goalId);
    updateTimestamp();
}

void Goal::removeDependency(const UUID& goalId) {
    dependencies_.erase(goalId);
    updateTimestamp();
}

void Goal::addSubGoal(std::shared_ptr<Goal> subGoal) {
    if (subGoal && subGoal->getId() != id_) {
        subGoal->setParentGoal(shared_from_this());
        subGoals_.push_back(subGoal);
        updateTimestamp();
    }
}

void Goal::removeSubGoal(const UUID& subGoalId) {
    subGoals_.erase(
        std::remove_if(subGoals_.begin(), subGoals_.end(),
            [&subGoalId](const std::shared_ptr<Goal>& g) {
                return g->getId() == subGoalId;
            }),
        subGoals_.end());
    updateTimestamp();
}

void Goal::setParentGoal(std::shared_ptr<Goal> parent) {
    parentGoal_ = parent;
    updateTimestamp();
}

void Goal::setDeadline(const Timestamp& deadline) {
    deadline_ = deadline;
    updateTimestamp();
}

bool Goal::isOverdue() const {
    if (!deadline_.has_value()) return false;
    return std::chrono::system_clock::now() > deadline_.value();
}

std::chrono::duration<double> Goal::getTimeRemaining() const {
    if (!deadline_.has_value()) {
        return std::chrono::duration<double>::max();
    }
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(deadline_.value() - now);
}

void Goal::addTag(const std::string& tag) {
    tags_.insert(tag);
    updateTimestamp();
}

void Goal::removeTag(const std::string& tag) {
    tags_.erase(tag);
    updateTimestamp();
}

bool Goal::hasTag(const std::string& tag) const {
    return tags_.find(tag) != tags_.end();
}

void Goal::setMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    updateTimestamp();
}

std::optional<std::string> Goal::getMetadata(const std::string& key) const {
    auto it = metadata_.find(key);
    if (it != metadata_.end()) {
        return it->second;
    }
    return std::nullopt;
}

double Goal::calculateScore() const {
    // Higher score = higher priority for selection
    double score = 0.0;

    // Priority weight (critical = highest score)
    score += (4 - static_cast<int>(priority_)) * 10.0;

    // Urgency based on deadline
    if (deadline_.has_value()) {
        auto remaining = getTimeRemaining().count();
        if (remaining < 0) {
            score += 50.0; // Overdue gets high priority
        } else if (remaining < 3600) { // Less than 1 hour
            score += 30.0;
        } else if (remaining < 86400) { // Less than 1 day
            score += 15.0;
        }
    }

    // Reward/effort ratio
    if (estimatedEffort_ > 0) {
        score += (reward_ / estimatedEffort_) * 5.0;
    }

    // Favor goals with higher progress (almost done)
    score += progress_ * 10.0;

    // Penalize blocked goals
    if (status_ == GoalStatus::BLOCKED) {
        score -= 20.0;
    }

    return score;
}

// ==============================================================================
// GOAL MANAGER IMPLEMENTATION
// ==============================================================================

GoalManager::GoalManager() = default;

GoalManager::~GoalManager() = default;

std::shared_ptr<Goal> GoalManager::createGoal(const std::string& name, const std::string& description) {
    return createGoal(name, description, GoalPriority::NORMAL, GoalType::ACHIEVEMENT);
}

std::shared_ptr<Goal> GoalManager::createGoal(const std::string& name, const std::string& description,
                                               GoalPriority priority, GoalType type) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto goal = std::make_shared<Goal>(name, description);
    goal->setPriority(priority);
    goal->setType(type);

    goals_[goal->getId()] = goal;
    notifyCreated(goal);

    return goal;
}

bool GoalManager::removeGoal(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    // Remove from other goals' dependencies
    for (auto& pair : goals_) {
        pair.second->removeDependency(goalId);
    }

    goals_.erase(it);
    return true;
}

std::shared_ptr<Goal> GoalManager::getGoal(const UUID& goalId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it != goals_.end()) {
        return it->second;
    }
    return nullptr;
}

bool GoalManager::hasGoal(const UUID& goalId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return goals_.find(goalId) != goals_.end();
}

std::vector<std::shared_ptr<Goal>> GoalManager::getAllGoals() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    result.reserve(goals_.size());
    for (const auto& pair : goals_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByStatus(GoalStatus status) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->getStatus() == status) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByPriority(GoalPriority priority) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->getPriority() == priority) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByType(GoalType type) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->getType() == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->hasTag(tag)) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getActiveGoals() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->isActive()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getBlockedGoals() const {
    return getGoalsByStatus(GoalStatus::BLOCKED);
}

std::vector<std::shared_ptr<Goal>> GoalManager::getOverdueGoals() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::shared_ptr<Goal>> result;
    for (const auto& pair : goals_) {
        if (pair.second->isOverdue() && !pair.second->isCompleted()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::shared_ptr<Goal> GoalManager::selectNextGoal() const {
    auto goals = getTopGoals(1);
    if (!goals.empty()) {
        return goals[0];
    }
    return nullptr;
}

std::vector<std::shared_ptr<Goal>> GoalManager::getTopGoals(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);

    // Get all eligible goals (pending or in progress, not blocked)
    std::vector<std::shared_ptr<Goal>> eligible;
    for (const auto& pair : goals_) {
        auto status = pair.second->getStatus();
        if (status == GoalStatus::PENDING || status == GoalStatus::ACTIVE ||
            status == GoalStatus::IN_PROGRESS) {
            // Check if dependencies are met
            bool blocked = false;
            for (const auto& depId : pair.second->getDependencies()) {
                auto depIt = goals_.find(depId);
                if (depIt != goals_.end() && !depIt->second->isCompleted()) {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                eligible.push_back(pair.second);
            }
        }
    }

    // Sort by score (descending)
    std::sort(eligible.begin(), eligible.end(),
        [](const std::shared_ptr<Goal>& a, const std::shared_ptr<Goal>& b) {
            return a->calculateScore() > b->calculateScore();
        });

    // Return top N
    if (eligible.size() > count) {
        eligible.resize(count);
    }
    return eligible;
}

bool GoalManager::addDependency(const UUID& goalId, const UUID& dependsOnId) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (goalId == dependsOnId) {
        return false; // Can't depend on itself
    }

    auto goalIt = goals_.find(goalId);
    auto depIt = goals_.find(dependsOnId);

    if (goalIt == goals_.end() || depIt == goals_.end()) {
        return false;
    }

    // Check for cycles
    if (detectCycle(goalId, dependsOnId)) {
        return false;
    }

    goalIt->second->addDependency(dependsOnId);
    return true;
}

bool GoalManager::removeDependency(const UUID& goalId, const UUID& dependsOnId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->removeDependency(dependsOnId);
    return true;
}

bool GoalManager::areDependenciesMet(const UUID& goalId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    for (const auto& depId : it->second->getDependencies()) {
        auto depIt = goals_.find(depId);
        if (depIt == goals_.end() || !depIt->second->isCompleted()) {
            return false;
        }
    }
    return true;
}

std::vector<UUID> GoalManager::getUnmetDependencies(const UUID& goalId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<UUID> unmet;
    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return unmet;
    }

    for (const auto& depId : it->second->getDependencies()) {
        auto depIt = goals_.find(depId);
        if (depIt == goals_.end() || !depIt->second->isCompleted()) {
            unmet.push_back(depId);
        }
    }
    return unmet;
}

void GoalManager::updateBlockedGoals() {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& pair : goals_) {
        auto& goal = pair.second;
        if (goal->getStatus() == GoalStatus::BLOCKED) {
            // Check if dependencies are now met
            bool allMet = true;
            for (const auto& depId : goal->getDependencies()) {
                auto depIt = goals_.find(depId);
                if (depIt == goals_.end() || !depIt->second->isCompleted()) {
                    allMet = false;
                    break;
                }
            }
            if (allMet) {
                goal->setStatus(GoalStatus::PENDING);
            }
        }
    }
}

bool GoalManager::addSubGoal(const UUID& parentId, const UUID& subGoalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto parentIt = goals_.find(parentId);
    auto subIt = goals_.find(subGoalId);

    if (parentIt == goals_.end() || subIt == goals_.end()) {
        return false;
    }

    parentIt->second->addSubGoal(subIt->second);
    return true;
}

bool GoalManager::removeSubGoal(const UUID& parentId, const UUID& subGoalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto parentIt = goals_.find(parentId);
    if (parentIt == goals_.end()) {
        return false;
    }

    parentIt->second->removeSubGoal(subGoalId);
    return true;
}

void GoalManager::updateParentProgress(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return;
    }

    auto parent = it->second->getParentGoal();
    if (!parent) {
        return;
    }

    // Calculate parent progress based on sub-goals
    const auto& subGoals = parent->getSubGoals();
    if (subGoals.empty()) {
        return;
    }

    double totalProgress = 0.0;
    for (const auto& sub : subGoals) {
        totalProgress += sub->getProgress();
    }
    parent->setProgress(totalProgress / subGoals.size());
}

bool GoalManager::activateGoal(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    auto& goal = it->second;
    if (goal->getStatus() != GoalStatus::PENDING) {
        return false;
    }

    // Check dependencies
    for (const auto& depId : goal->getDependencies()) {
        auto depIt = goals_.find(depId);
        if (depIt == goals_.end() || !depIt->second->isCompleted()) {
            goal->setStatus(GoalStatus::BLOCKED);
            return false;
        }
    }

    goal->setStatus(GoalStatus::ACTIVE);
    return true;
}

bool GoalManager::completeGoal(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->setStatus(GoalStatus::COMPLETED);
    notifyCompleted(it->second);

    // Update blocked goals
    for (auto& pair : goals_) {
        if (pair.second->getStatus() == GoalStatus::BLOCKED) {
            const auto& deps = pair.second->getDependencies();
            if (deps.find(goalId) != deps.end()) {
                // Check if all dependencies are now met
                bool allMet = true;
                for (const auto& depId : deps) {
                    auto depIt = goals_.find(depId);
                    if (depIt == goals_.end() || !depIt->second->isCompleted()) {
                        allMet = false;
                        break;
                    }
                }
                if (allMet) {
                    pair.second->setStatus(GoalStatus::PENDING);
                }
            }
        }
    }

    return true;
}

bool GoalManager::failGoal(const UUID& goalId, const std::string& reason) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->setStatus(GoalStatus::FAILED);
    if (!reason.empty()) {
        it->second->setMetadata("failure_reason", reason);
    }
    notifyFailed(it->second);

    return true;
}

bool GoalManager::cancelGoal(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->setStatus(GoalStatus::CANCELLED);
    return true;
}

bool GoalManager::blockGoal(const UUID& goalId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->setStatus(GoalStatus::BLOCKED);
    return true;
}

bool GoalManager::updateProgress(const UUID& goalId, double progress) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->setProgress(progress);
    notifyProgress(it->second);

    return true;
}

bool GoalManager::incrementProgress(const UUID& goalId, double amount) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = goals_.find(goalId);
    if (it == goals_.end()) {
        return false;
    }

    it->second->incrementProgress(amount);
    notifyProgress(it->second);

    return true;
}

void GoalManager::onGoalCreated(GoalCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onCreatedCallbacks_.push_back(std::move(callback));
}

void GoalManager::onGoalCompleted(GoalCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onCompletedCallbacks_.push_back(std::move(callback));
}

void GoalManager::onGoalFailed(GoalCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onFailedCallbacks_.push_back(std::move(callback));
}

void GoalManager::onGoalProgress(GoalCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onProgressCallbacks_.push_back(std::move(callback));
}

size_t GoalManager::getTotalGoalCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return goals_.size();
}

size_t GoalManager::getActiveGoalCount() const {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t count = 0;
    for (const auto& pair : goals_) {
        if (pair.second->isActive()) {
            ++count;
        }
    }
    return count;
}

size_t GoalManager::getCompletedGoalCount() const {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t count = 0;
    for (const auto& pair : goals_) {
        if (pair.second->isCompleted()) {
            ++count;
        }
    }
    return count;
}

double GoalManager::getOverallProgress() const {
    std::lock_guard<std::mutex> lock(mutex_);

    if (goals_.empty()) {
        return 0.0;
    }

    double total = 0.0;
    for (const auto& pair : goals_) {
        total += pair.second->getProgress();
    }
    return total / goals_.size();
}

std::unordered_map<GoalStatus, size_t> GoalManager::getGoalCountByStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::unordered_map<GoalStatus, size_t> counts;
    for (const auto& pair : goals_) {
        counts[pair.second->getStatus()]++;
    }
    return counts;
}

std::string GoalManager::serialize() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::ostringstream oss;
    oss << "GOALS:" << goals_.size() << "\n";

    for (const auto& pair : goals_) {
        const auto& g = pair.second;
        oss << "GOAL|" << g->getId() << "|" << g->getName() << "|"
            << goalStatusToString(g->getStatus()) << "|"
            << goalPriorityToString(g->getPriority()) << "|"
            << g->getProgress() << "\n";
    }

    return oss.str();
}

bool GoalManager::deserialize(const std::string& data) {
    // Simple deserialization - would need more robust implementation
    return !data.empty();
}

void GoalManager::notifyCreated(std::shared_ptr<Goal> goal) {
    for (const auto& callback : onCreatedCallbacks_) {
        callback(goal);
    }
}

void GoalManager::notifyCompleted(std::shared_ptr<Goal> goal) {
    for (const auto& callback : onCompletedCallbacks_) {
        callback(goal);
    }
}

void GoalManager::notifyFailed(std::shared_ptr<Goal> goal) {
    for (const auto& callback : onFailedCallbacks_) {
        callback(goal);
    }
}

void GoalManager::notifyProgress(std::shared_ptr<Goal> goal) {
    for (const auto& callback : onProgressCallbacks_) {
        callback(goal);
    }
}

bool GoalManager::detectCycle(const UUID& goalId, const UUID& newDependency) const {
    // BFS to detect if adding newDependency would create a cycle
    std::queue<UUID> toVisit;
    std::unordered_set<UUID> visited;

    toVisit.push(newDependency);

    while (!toVisit.empty()) {
        UUID current = toVisit.front();
        toVisit.pop();

        if (current == goalId) {
            return true; // Cycle detected
        }

        if (visited.find(current) != visited.end()) {
            continue;
        }
        visited.insert(current);

        auto it = goals_.find(current);
        if (it != goals_.end()) {
            for (const auto& depId : it->second->getDependencies()) {
                toVisit.push(depId);
            }
        }
    }

    return false;
}

} // namespace elizaos
