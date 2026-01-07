// goal_manager.hpp - Enhanced Goal Management System for ElizaOS
// Provides comprehensive goal tracking with priorities, dependencies, and progress

#ifndef ELIZAOS_GOAL_MANAGER_HPP
#define ELIZAOS_GOAL_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <mutex>
#include <optional>

namespace elizaos {

// ==============================================================================
// GOAL TYPES AND ENUMS
// ==============================================================================

using UUID = std::string;
using Timestamp = std::chrono::system_clock::time_point;

enum class GoalPriority {
    CRITICAL = 0,    // Must be achieved, blocks other goals
    HIGH = 1,        // Important, should be prioritized
    NORMAL = 2,      // Standard priority
    LOW = 3,         // Can be deferred
    BACKGROUND = 4   // Work on when nothing else is pending
};

enum class GoalStatus {
    PENDING,         // Not yet started
    ACTIVE,          // Currently being worked on
    IN_PROGRESS,     // Partially completed
    BLOCKED,         // Waiting for dependencies
    COMPLETED,       // Successfully achieved
    FAILED,          // Could not be achieved
    CANCELLED        // Explicitly cancelled
};

enum class GoalType {
    ACHIEVEMENT,     // One-time goal to be completed
    MAINTENANCE,     // Ongoing goal to maintain state
    IMPROVEMENT,     // Goal to improve metrics over time
    EXPLORATION,     // Goal to discover/learn something
    REACTIVE         // Goal triggered by external events
};

// ==============================================================================
// GOAL CLASS
// ==============================================================================

class Goal : public std::enable_shared_from_this<Goal> {
public:
    // Constructors
    Goal(const std::string& name, const std::string& description);
    Goal(const UUID& id, const std::string& name, const std::string& description);

    // Basic accessors
    const UUID& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }

    // Status management
    GoalStatus getStatus() const { return status_; }
    void setStatus(GoalStatus status);
    bool isActive() const { return status_ == GoalStatus::ACTIVE || status_ == GoalStatus::IN_PROGRESS; }
    bool isCompleted() const { return status_ == GoalStatus::COMPLETED; }
    bool isBlocked() const { return status_ == GoalStatus::BLOCKED; }

    // Priority management
    GoalPriority getPriority() const { return priority_; }
    void setPriority(GoalPriority priority) { priority_ = priority; }
    int getPriorityValue() const { return static_cast<int>(priority_); }

    // Type management
    GoalType getType() const { return type_; }
    void setType(GoalType type) { type_ = type; }

    // Progress tracking
    double getProgress() const { return progress_; }
    void setProgress(double progress);
    void incrementProgress(double amount);

    // Dependencies
    void addDependency(const UUID& goalId);
    void removeDependency(const UUID& goalId);
    const std::unordered_set<UUID>& getDependencies() const { return dependencies_; }
    bool hasDependencies() const { return !dependencies_.empty(); }

    // Sub-goals (hierarchical goals)
    void addSubGoal(std::shared_ptr<Goal> subGoal);
    void removeSubGoal(const UUID& subGoalId);
    const std::vector<std::shared_ptr<Goal>>& getSubGoals() const { return subGoals_; }
    std::shared_ptr<Goal> getParentGoal() const { return parentGoal_.lock(); }
    void setParentGoal(std::shared_ptr<Goal> parent);
    bool hasSubGoals() const { return !subGoals_.empty(); }

    // Deadline management
    void setDeadline(const Timestamp& deadline);
    std::optional<Timestamp> getDeadline() const { return deadline_; }
    bool hasDeadline() const { return deadline_.has_value(); }
    bool isOverdue() const;
    std::chrono::duration<double> getTimeRemaining() const;

    // Timestamps
    Timestamp getCreatedAt() const { return createdAt_; }
    Timestamp getUpdatedAt() const { return updatedAt_; }
    std::optional<Timestamp> getCompletedAt() const { return completedAt_; }

    // Tags for categorization
    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    const std::unordered_set<std::string>& getTags() const { return tags_; }
    bool hasTag(const std::string& tag) const;

    // Metadata
    void setMetadata(const std::string& key, const std::string& value);
    std::optional<std::string> getMetadata(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAllMetadata() const { return metadata_; }

    // Reward/weight for priority calculations
    double getReward() const { return reward_; }
    void setReward(double reward) { reward_ = reward; }

    // Effort estimation
    double getEstimatedEffort() const { return estimatedEffort_; }
    void setEstimatedEffort(double effort) { estimatedEffort_ = effort; }
    double getActualEffort() const { return actualEffort_; }
    void addActualEffort(double effort) { actualEffort_ += effort; }

    // Scoring for goal selection
    double calculateScore() const;

private:
    UUID id_;
    std::string name_;
    std::string description_;
    GoalStatus status_ = GoalStatus::PENDING;
    GoalPriority priority_ = GoalPriority::NORMAL;
    GoalType type_ = GoalType::ACHIEVEMENT;
    double progress_ = 0.0;  // 0.0 to 1.0

    // Dependencies
    std::unordered_set<UUID> dependencies_;

    // Hierarchical structure
    std::vector<std::shared_ptr<Goal>> subGoals_;
    std::weak_ptr<Goal> parentGoal_;

    // Timing
    std::optional<Timestamp> deadline_;
    Timestamp createdAt_;
    Timestamp updatedAt_;
    std::optional<Timestamp> completedAt_;

    // Categorization
    std::unordered_set<std::string> tags_;
    std::unordered_map<std::string, std::string> metadata_;

    // Effort and reward
    double reward_ = 1.0;
    double estimatedEffort_ = 1.0;
    double actualEffort_ = 0.0;

    // Helpers
    static UUID generateId();
    void updateTimestamp();
};

// ==============================================================================
// GOAL MANAGER
// ==============================================================================

class GoalManager {
public:
    using GoalCallback = std::function<void(std::shared_ptr<Goal>)>;

    GoalManager();
    ~GoalManager();

    // Goal lifecycle
    std::shared_ptr<Goal> createGoal(const std::string& name, const std::string& description);
    std::shared_ptr<Goal> createGoal(const std::string& name, const std::string& description,
                                     GoalPriority priority, GoalType type);
    bool removeGoal(const UUID& goalId);
    std::shared_ptr<Goal> getGoal(const UUID& goalId) const;
    bool hasGoal(const UUID& goalId) const;

    // Query goals
    std::vector<std::shared_ptr<Goal>> getAllGoals() const;
    std::vector<std::shared_ptr<Goal>> getGoalsByStatus(GoalStatus status) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByPriority(GoalPriority priority) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByType(GoalType type) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByTag(const std::string& tag) const;
    std::vector<std::shared_ptr<Goal>> getActiveGoals() const;
    std::vector<std::shared_ptr<Goal>> getBlockedGoals() const;
    std::vector<std::shared_ptr<Goal>> getOverdueGoals() const;

    // Goal selection
    std::shared_ptr<Goal> selectNextGoal() const;
    std::vector<std::shared_ptr<Goal>> getTopGoals(size_t count) const;

    // Dependency management
    bool addDependency(const UUID& goalId, const UUID& dependsOnId);
    bool removeDependency(const UUID& goalId, const UUID& dependsOnId);
    bool areDependenciesMet(const UUID& goalId) const;
    std::vector<UUID> getUnmetDependencies(const UUID& goalId) const;
    void updateBlockedGoals();

    // Hierarchical goals
    bool addSubGoal(const UUID& parentId, const UUID& subGoalId);
    bool removeSubGoal(const UUID& parentId, const UUID& subGoalId);
    void updateParentProgress(const UUID& goalId);

    // Status transitions
    bool activateGoal(const UUID& goalId);
    bool completeGoal(const UUID& goalId);
    bool failGoal(const UUID& goalId, const std::string& reason = "");
    bool cancelGoal(const UUID& goalId);
    bool blockGoal(const UUID& goalId);

    // Progress updates
    bool updateProgress(const UUID& goalId, double progress);
    bool incrementProgress(const UUID& goalId, double amount);

    // Callbacks
    void onGoalCreated(GoalCallback callback);
    void onGoalCompleted(GoalCallback callback);
    void onGoalFailed(GoalCallback callback);
    void onGoalProgress(GoalCallback callback);

    // Statistics
    size_t getTotalGoalCount() const;
    size_t getActiveGoalCount() const;
    size_t getCompletedGoalCount() const;
    double getOverallProgress() const;
    std::unordered_map<GoalStatus, size_t> getGoalCountByStatus() const;

    // Persistence (simple serialization)
    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    std::unordered_map<UUID, std::shared_ptr<Goal>> goals_;
    mutable std::mutex mutex_;

    // Callbacks
    std::vector<GoalCallback> onCreatedCallbacks_;
    std::vector<GoalCallback> onCompletedCallbacks_;
    std::vector<GoalCallback> onFailedCallbacks_;
    std::vector<GoalCallback> onProgressCallbacks_;

    // Helpers
    void notifyCreated(std::shared_ptr<Goal> goal);
    void notifyCompleted(std::shared_ptr<Goal> goal);
    void notifyFailed(std::shared_ptr<Goal> goal);
    void notifyProgress(std::shared_ptr<Goal> goal);
    bool detectCycle(const UUID& goalId, const UUID& newDependency) const;
};

// ==============================================================================
// FREE FUNCTIONS
// ==============================================================================

std::string goalStatusToString(GoalStatus status);
GoalStatus stringToGoalStatus(const std::string& str);
std::string goalPriorityToString(GoalPriority priority);
GoalPriority stringToGoalPriority(const std::string& str);
std::string goalTypeToString(GoalType type);
GoalType stringToGoalType(const std::string& str);

} // namespace elizaos

#endif // ELIZAOS_GOAL_MANAGER_HPP
