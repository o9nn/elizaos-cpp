#pragma once

/**
 * ElizaOS C++ - AgentAgenda Module
 *
 * Task queue with goal/plan/step tracking. Tasks are persisted via
 * AgentMemoryManager.
 */

#include <any>
#include <chrono>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "elizaos/core.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentlogger.hpp"

namespace elizaos {

using JsonValue = std::unordered_map<std::string, std::any>;

enum class AgendaTaskStatus {
    IN_PROGRESS,
    COMPLETE,
    CANCELLED
};

inline std::ostream& operator<<(std::ostream& os, AgendaTaskStatus s) {
    switch (s) {
        case AgendaTaskStatus::IN_PROGRESS: return os << "IN_PROGRESS";
        case AgendaTaskStatus::COMPLETE:    return os << "COMPLETE";
        case AgendaTaskStatus::CANCELLED:   return os << "CANCELLED";
        default:                            return os << "UNKNOWN";
    }
}

struct AgendaTaskStep {
    std::string content;
    bool completed = false;

    AgendaTaskStep() = default;
    AgendaTaskStep(const std::string& c, bool comp = false)
        : content(c), completed(comp) {}
};

struct AgendaTask {
    std::string id;
    std::string goal;
    std::string plan;
    std::vector<AgendaTaskStep> steps;
    AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point updated_at;
    bool current = false;

    AgendaTask() = default;
    explicit AgendaTask(const std::string& goal_text)
        : goal(goal_text),
          created_at(std::chrono::system_clock::now()),
          updated_at(std::chrono::system_clock::now()) {}

    bool isComplete() const { return status == AgendaTaskStatus::COMPLETE; }
    bool isCancelled() const { return status == AgendaTaskStatus::CANCELLED; }
    bool isInProgress() const { return status == AgendaTaskStatus::IN_PROGRESS; }
};

class AgentAgenda {
public:
    AgentAgenda();
    ~AgentAgenda();

    AgendaTask createTask(const std::string& goal,
                          const std::string& plan = "",
                          const std::vector<AgendaTaskStep>& steps = {});

    std::vector<AgendaTask> listTasks(AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    std::vector<AgendaTask> searchTasks(const std::string& search_term,
                                         AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    AgendaTask getTaskById(const std::string& task_id);

    bool deleteTask(const std::string& task_id);
    bool finishTask(const std::string& task_id);
    bool cancelTask(const std::string& task_id);

    AgendaTask getCurrentTask();
    bool setCurrentTask(const std::string& task_id);

    AgendaTask getLastCreatedTask();
    AgendaTask getLastUpdatedTask();

    std::string createPlan(const std::string& goal);
    bool updatePlan(const std::string& task_id, const std::string& plan);
    
    /**
     * @brief Re-plan an existing task based on current progress
     * 
     * This method analyzes the current state of a task (completed steps, 
     * remaining steps, and the original goal) and generates a new plan
     * that accounts for what has already been done.
     * 
     * @param task_id The ID of the task to re-plan
     * @param context Optional additional context for re-planning (e.g., why re-planning is needed)
     * @param regenerate_steps If true, also regenerates the steps based on the new plan
     * @return The new plan string, or empty string if task not found
     */
    std::string planAgain(const std::string& task_id, 
                          const std::string& context = "",
                          bool regenerate_steps = false);

    std::vector<AgendaTaskStep> createSteps(const std::string& goal, const std::string& plan);
    bool addStep(const std::string& task_id, const std::string& step_content);
    bool finishStep(const std::string& task_id, const std::string& step_content);
    bool cancelStep(const std::string& task_id, const std::string& step_content);
    bool updateStep(const std::string& task_id, const std::string& old_step,
                    const AgendaTaskStep& new_step);

    std::string getTaskAsFormattedString(const AgendaTask& task,
                                          bool include_plan = true,
                                          bool include_current_step = true,
                                          bool include_status = true,
                                          bool include_steps = true);
    std::string listTasksAsFormattedString();
    void clearTasks();

private:
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger> logger_;

    std::string statusToString(AgendaTaskStatus status);
    AgendaTaskStatus stringToStatus(const std::string& status_str);
    void saveTaskToMemory(const AgendaTask& task);
    AgendaTask loadTaskFromMemory(const std::shared_ptr<Memory>& memory_obj);
    std::string generateSimpleUUID();
    std::string serializeSteps(const std::vector<AgendaTaskStep>& steps);
    std::vector<AgendaTaskStep> deserializeSteps(const std::string& steps_json);
    std::string timestampToString(const std::chrono::system_clock::time_point& timepoint);
    std::chrono::system_clock::time_point stringToTimestamp(const std::string& timestamp_str);
};

} // namespace elizaos
