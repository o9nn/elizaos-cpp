// goal_manager.hpp - Deterministic, thread-safe goal management for ElizaOS

#ifndef ELIZAOS_GOAL_MANAGER_HPP
#define ELIZAOS_GOAL_MANAGER_HPP

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace elizaos {

using UUID = std::string;
using Timestamp = std::chrono::system_clock::time_point;

enum class GoalPriority { CRITICAL = 0, HIGH = 1, NORMAL = 2, LOW = 3, BACKGROUND = 4 };
enum class GoalStatus { PENDING, ACTIVE, IN_PROGRESS, BLOCKED, COMPLETED, FAILED, CANCELLED };
enum class GoalType { ACHIEVEMENT, MAINTENANCE, IMPROVEMENT, EXPLORATION, REACTIVE };

/** A safe, deterministic and self-contained view of a Goal. */
struct GoalSnapshot {
    UUID id;
    std::string name;
    std::string description;
    GoalStatus status = GoalStatus::PENDING;
    GoalPriority priority = GoalPriority::NORMAL;
    GoalType type = GoalType::ACHIEVEMENT;
    double progress = 0.0;
    std::vector<UUID> dependencies;
    std::vector<UUID> subGoalIds;
    std::optional<UUID> parentGoalId;
    std::optional<Timestamp> deadline;
    Timestamp createdAt{};
    Timestamp updatedAt{};
    std::optional<Timestamp> completedAt;
    std::vector<std::string> tags;
    std::vector<std::pair<std::string, std::string>> metadata;
    double reward = 1.0;
    double estimatedEffort = 1.0;
    double actualEffort = 0.0;
};

class Goal : public std::enable_shared_from_this<Goal> {
public:
    static constexpr std::size_t MAX_ID_LENGTH = 128;
    static constexpr std::size_t MAX_NAME_LENGTH = 256;
    static constexpr std::size_t MAX_DESCRIPTION_LENGTH = 4096;
    static constexpr std::size_t MAX_TAG_LENGTH = 128;
    static constexpr std::size_t MAX_METADATA_KEY_LENGTH = 128;
    static constexpr std::size_t MAX_METADATA_VALUE_LENGTH = 4096;
    static constexpr std::size_t MAX_TAG_COUNT = 256;
    static constexpr std::size_t MAX_METADATA_COUNT = 256;
    static constexpr std::size_t MAX_DEPENDENCY_COUNT = 4096;

    Goal(const std::string& name, const std::string& description);
    Goal(const UUID& id, const std::string& name, const std::string& description);

    // These fields are immutable. The legacy reference getters therefore remain
    // valid for the Goal object's lifetime; copy forms make ownership explicit.
    const UUID& getId() const noexcept { return id_; }
    const std::string& getName() const noexcept { return name_; }
    const std::string& getDescription() const noexcept { return description_; }
    UUID getIdCopy() const { return id_; }
    std::string getNameCopy() const { return name_; }
    std::string getDescriptionCopy() const { return description_; }

    GoalStatus getStatus() const;
    void setStatus(GoalStatus status);
    bool isActive() const;
    bool isCompleted() const;
    bool isBlocked() const;

    GoalPriority getPriority() const;
    void setPriority(GoalPriority priority);
    int getPriorityValue() const;

    GoalType getType() const;
    void setType(GoalType type);

    double getProgress() const;
    void setProgress(double progress);
    void incrementProgress(double amount);

    void addDependency(const UUID& goalId);
    void removeDependency(const UUID& goalId);
    // Unsafe under concurrent mutation; retained only for legacy source
    // compatibility. New code should use getDependenciesCopy() or snapshot().
    const std::unordered_set<UUID>& getDependencies() const noexcept { return dependencies_; }
    std::unordered_set<UUID> getDependenciesCopy() const;
    bool hasDependencies() const;

    void addSubGoal(std::shared_ptr<Goal> subGoal);
    void removeSubGoal(const UUID& subGoalId);
    // Unsafe under concurrent mutation; retained for compatibility.
    const std::vector<std::shared_ptr<Goal>>& getSubGoals() const noexcept { return subGoals_; }
    std::vector<std::shared_ptr<Goal>> getSubGoalsCopy() const;
    std::shared_ptr<Goal> getParentGoal() const;
    void setParentGoal(std::shared_ptr<Goal> parent);
    bool hasSubGoals() const;

    void setDeadline(const Timestamp& deadline);
    std::optional<Timestamp> getDeadline() const;
    bool hasDeadline() const;
    bool isOverdue() const;
    std::chrono::duration<double> getTimeRemaining() const;

    Timestamp getCreatedAt() const;
    Timestamp getUpdatedAt() const;
    std::optional<Timestamp> getCompletedAt() const;

    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    // Unsafe under concurrent mutation; retained for compatibility.
    const std::unordered_set<std::string>& getTags() const noexcept { return tags_; }
    std::unordered_set<std::string> getTagsCopy() const;
    bool hasTag(const std::string& tag) const;

    void setMetadata(const std::string& key, const std::string& value);
    std::optional<std::string> getMetadata(const std::string& key) const;
    // Unsafe under concurrent mutation; retained for compatibility.
    const std::unordered_map<std::string, std::string>& getAllMetadata() const noexcept {
        return metadata_;
    }
    std::unordered_map<std::string, std::string> getAllMetadataCopy() const;

    double getReward() const;
    void setReward(double reward);
    double getEstimatedEffort() const;
    void setEstimatedEffort(double effort);
    double getActualEffort() const;
    void addActualEffort(double effort);

    double calculateScore() const;
    GoalSnapshot snapshot() const;

private:
    UUID id_;
    std::string name_;
    std::string description_;
    GoalStatus status_ = GoalStatus::PENDING;
    GoalPriority priority_ = GoalPriority::NORMAL;
    GoalType type_ = GoalType::ACHIEVEMENT;
    double progress_ = 0.0;
    std::unordered_set<UUID> dependencies_;
    std::vector<std::shared_ptr<Goal>> subGoals_;
    std::weak_ptr<Goal> parentGoal_;
    std::optional<Timestamp> deadline_;
    Timestamp createdAt_;
    Timestamp updatedAt_;
    std::optional<Timestamp> completedAt_;
    std::unordered_set<std::string> tags_;
    std::unordered_map<std::string, std::string> metadata_;
    double reward_ = 1.0;
    double estimatedEffort_ = 1.0;
    double actualEffort_ = 0.0;
    mutable std::mutex stateMutex_;

    static UUID generateId();
    void updateTimestampUnlocked();
    bool transitionToUnlocked(GoalStatus status);

    friend class GoalManager;
};

class GoalManager {
public:
    using GoalCallback = std::function<void(std::shared_ptr<Goal>)>;

    GoalManager();
    ~GoalManager();

    std::shared_ptr<Goal> createGoal(const std::string& name, const std::string& description);
    std::shared_ptr<Goal> createGoal(const std::string& name, const std::string& description,
                                     GoalPriority priority, GoalType type);
    bool removeGoal(const UUID& goalId);
    std::shared_ptr<Goal> getGoal(const UUID& goalId) const;
    std::optional<GoalSnapshot> getGoalSnapshot(const UUID& goalId) const;
    bool hasGoal(const UUID& goalId) const;

    std::vector<std::shared_ptr<Goal>> getAllGoals() const;
    std::vector<GoalSnapshot> getAllGoalSnapshots() const;
    std::vector<std::shared_ptr<Goal>> getGoalsByStatus(GoalStatus status) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByPriority(GoalPriority priority) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByType(GoalType type) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByTag(const std::string& tag) const;
    std::vector<std::shared_ptr<Goal>> getActiveGoals() const;
    std::vector<std::shared_ptr<Goal>> getBlockedGoals() const;
    std::vector<std::shared_ptr<Goal>> getOverdueGoals() const;

    std::shared_ptr<Goal> selectNextGoal() const;
    std::vector<std::shared_ptr<Goal>> getTopGoals(std::size_t count) const;

    bool addDependency(const UUID& goalId, const UUID& dependsOnId);
    bool removeDependency(const UUID& goalId, const UUID& dependsOnId);
    bool areDependenciesMet(const UUID& goalId) const;
    std::vector<UUID> getUnmetDependencies(const UUID& goalId) const;
    void updateBlockedGoals();

    bool addSubGoal(const UUID& parentId, const UUID& subGoalId);
    bool removeSubGoal(const UUID& parentId, const UUID& subGoalId);
    void updateParentProgress(const UUID& goalId);

    bool activateGoal(const UUID& goalId);
    bool completeGoal(const UUID& goalId);
    bool failGoal(const UUID& goalId, const std::string& reason = "");
    bool cancelGoal(const UUID& goalId);
    bool blockGoal(const UUID& goalId);

    bool updateProgress(const UUID& goalId, double progress);
    bool incrementProgress(const UUID& goalId, double amount);

    void onGoalCreated(GoalCallback callback);
    void onGoalCompleted(GoalCallback callback);
    void onGoalFailed(GoalCallback callback);
    void onGoalProgress(GoalCallback callback);

    std::size_t getTotalGoalCount() const;
    std::size_t getActiveGoalCount() const;
    std::size_t getCompletedGoalCount() const;
    double getOverallProgress() const;
    std::unordered_map<GoalStatus, std::size_t> getGoalCountByStatus() const;

    std::string serialize() const;
    bool deserialize(const std::string& data);

private:
    using GoalMap = std::unordered_map<UUID, std::shared_ptr<Goal>>;

    GoalMap goals_;
    mutable std::mutex mutex_;
    std::vector<GoalCallback> onCreatedCallbacks_;
    std::vector<GoalCallback> onCompletedCallbacks_;
    std::vector<GoalCallback> onFailedCallbacks_;
    std::vector<GoalCallback> onProgressCallbacks_;

    void notifyCreated(const std::shared_ptr<Goal>& goal);
    void notifyCompleted(const std::shared_ptr<Goal>& goal);
    void notifyFailed(const std::shared_ptr<Goal>& goal);
    void notifyProgress(const std::shared_ptr<Goal>& goal);
    static void invokeCallbacks(const std::vector<GoalCallback>& callbacks,
                                const std::shared_ptr<Goal>& goal) noexcept;

    bool detectCycle(const UUID& goalId, const UUID& newDependency) const;
    bool dependenciesMetUnlocked(const std::shared_ptr<Goal>& goal) const;
    bool hierarchyContainsUnlocked(const UUID& rootId, const UUID& soughtId) const;
    bool subGoalsCompletedUnlocked(const std::shared_ptr<Goal>& goal) const;
    void updateBlockedGoalsUnlocked();
    std::vector<std::shared_ptr<Goal>> updateAncestorProgressUnlocked(const UUID& goalId);
};

std::string goalStatusToString(GoalStatus status);
GoalStatus stringToGoalStatus(const std::string& str);
std::string goalPriorityToString(GoalPriority priority);
GoalPriority stringToGoalPriority(const std::string& str);
std::string goalTypeToString(GoalType type);
GoalType stringToGoalType(const std::string& str);

inline std::ostream& operator<<(std::ostream& os, GoalPriority priority) {
    return os << goalPriorityToString(priority);
}
inline std::ostream& operator<<(std::ostream& os, GoalStatus status) {
    return os << goalStatusToString(status);
}
inline std::ostream& operator<<(std::ostream& os, GoalType type) {
    return os << goalTypeToString(type);
}

} // namespace elizaos

#endif // ELIZAOS_GOAL_MANAGER_HPP
