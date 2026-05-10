#pragma once

/**
 * ElizaOS C++ - GoalManager Module
 *
 * Hierarchical goal management with priorities, dependencies,
 * progress tracking, and deadline awareness.
 */

#include "elizaos.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// ============================================================================
// Enumerations
// ============================================================================

enum class GoalStatus {
    PENDING,
    ACTIVE,
    IN_PROGRESS,
    BLOCKED,
    COMPLETED,
    FAILED,
    CANCELLED
};

enum class GoalPriority {
    CRITICAL,
    HIGH,
    NORMAL,
    LOW,
    BACKGROUND
};

enum class GoalType {
    OBJECTIVE,
    TASK,
    MILESTONE,
    CONSTRAINT,
    DESIRE,
    PLAN,
    UNKNOWN
};

// ============================================================================
// String conversion helpers
// ============================================================================

std::string goalStatusToString(GoalStatus status);
GoalStatus  stringToGoalStatus(const std::string& str);
std::string goalPriorityToString(GoalPriority priority);
GoalPriority stringToGoalPriority(const std::string& str);
std::string goalTypeToString(GoalType type);
GoalType    stringToGoalType(const std::string& str);

// ============================================================================
// Goal
// ============================================================================

class Goal {
public:
    Goal() = default;
    Goal(const std::string& name, const std::string& description);
    Goal(const UUID& id, const std::string& name, const std::string& description);

    // Getters
    const UUID&        getId()          const { return id_; }
    const std::string& getName()        const { return name_; }
    const std::string& getDescription() const { return description_; }
    GoalStatus         getStatus()      const { return status_; }
    GoalPriority       getPriority()    const { return priority_; }
    GoalType           getType()        const { return type_; }
    double             getProgress()    const { return progress_; }
    Timestamp          getCreatedAt()   const { return createdAt_; }
    Timestamp          getUpdatedAt()   const { return updatedAt_; }

    std::optional<Timestamp>             getDeadline()        const { return deadline_; }
    const std::vector<UUID>&             getDependencies()    const { return dependencies_; }
    const std::vector<std::shared_ptr<Goal>>& getSubGoals()  const { return subGoals_; }
    std::shared_ptr<Goal>                getParentGoal()      const { return parentGoal_.lock(); }
    const std::vector<std::string>&      getTags()            const { return tags_; }

    std::optional<std::string> getMetadata(const std::string& key) const;

    bool isOverdue() const;
    std::chrono::duration<double> getTimeRemaining() const;
    double calculateScore() const;

    // Setters / mutators
    void setStatus(GoalStatus status);
    void setPriority(GoalPriority priority)   { priority_ = priority; updateTimestamp(); }
    void setType(GoalType type)               { type_     = type;     updateTimestamp(); }
    void setProgress(double progress);
    void incrementProgress(double amount);
    void setDeadline(const Timestamp& deadline);
    void setMetadata(const std::string& key, const std::string& value);

    void addDependency(const UUID& goalId);
    void removeDependency(const UUID& goalId);

    void addSubGoal(std::shared_ptr<Goal> subGoal);
    void removeSubGoal(const UUID& subGoalId);
    void setParentGoal(std::shared_ptr<Goal> parent);

    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    bool hasTag(const std::string& tag) const;

private:
    UUID        generateId();
    void        updateTimestamp();

    UUID         id_;
    std::string  name_;
    std::string  description_;
    GoalStatus   status_   = GoalStatus::PENDING;
    GoalPriority priority_ = GoalPriority::NORMAL;
    GoalType     type_     = GoalType::TASK;
    double       progress_ = 0.0;

    Timestamp    createdAt_;
    Timestamp    updatedAt_;
    std::optional<Timestamp> deadline_;

    std::vector<UUID>                  dependencies_;
    std::vector<std::shared_ptr<Goal>> subGoals_;
    std::weak_ptr<Goal>                parentGoal_;
    std::vector<std::string>           tags_;
    std::unordered_map<std::string, std::string> metadata_;
};

// ============================================================================
// GoalManager
// ============================================================================

class GoalManager {
public:
    GoalManager();

    // Creation
    std::shared_ptr<Goal> createGoal(const std::string& name,
                                      const std::string& description);
    std::shared_ptr<Goal> createGoal(const std::string& name,
                                      const std::string& description,
                                      GoalPriority priority,
                                      GoalType type = GoalType::TASK);

    // Removal
    bool removeGoal(const UUID& goalId);

    // Lookup
    std::shared_ptr<Goal> getGoal(const UUID& goalId) const;
    bool                  hasGoal(const UUID& goalId) const;

    // Query
    std::vector<std::shared_ptr<Goal>> getAllGoals() const;
    std::vector<std::shared_ptr<Goal>> getGoalsByStatus(GoalStatus status) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByPriority(GoalPriority priority) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByType(GoalType type) const;
    std::vector<std::shared_ptr<Goal>> getGoalsByTag(const std::string& tag) const;
    std::vector<std::shared_ptr<Goal>> getActiveGoals() const;
    std::vector<std::shared_ptr<Goal>> getBlockedGoals() const;
    std::vector<std::shared_ptr<Goal>> getOverdueGoals() const;

    std::shared_ptr<Goal>              selectNextGoal() const;
    std::vector<std::shared_ptr<Goal>> getTopGoals(size_t count) const;

    // Goal updates
    bool updateGoalStatus(const UUID& goalId, GoalStatus status);
    bool updateGoalProgress(const UUID& goalId, double progress);

    // Dependency management
    bool addDependency(const UUID& goalId, const UUID& dependsOnId);
    bool removeDependency(const UUID& goalId, const UUID& dependsOnId);
    bool areDependenciesMet(const UUID& goalId) const;
    std::vector<UUID> getUnmetDependencies(const UUID& goalId) const;
    void updateBlockedGoals();

    // Hierarchy
    bool addSubGoal(const UUID& parentId, const UUID& subGoalId);
    bool removeSubGoal(const UUID& parentId, const UUID& subGoalId);

    // Persistence
    bool exportToFile(const std::string& filename) const;
    bool importFromFile(const std::string& filename);
    void clear();

    size_t getGoalCount() const;

private:
    std::unordered_map<UUID, std::shared_ptr<Goal>> goals_;
    mutable std::mutex goalsMutex_;
};

} // namespace elizaos
