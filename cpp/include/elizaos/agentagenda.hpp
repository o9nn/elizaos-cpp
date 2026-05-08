#pragma once

/**
 * ElizaOS C++ - AgentAgenda Module
 *
 * Task queue with goal/plan/step tracking.  Tasks are persisted via
 * AgentMemoryManager.
 */

#include "elizaos.hpp"
#include "core.hpp"
#include "agentmemory.hpp"
#include "agentlogger.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

namespace elizaos {

// ============================================================================
// AgendaTaskStatus
// ============================================================================

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

// ============================================================================
// AgendaTaskStep
// ============================================================================

struct AgendaTaskStep {
    std::string content;
    bool        completed = false;
    bool        cancelled = false;
};

// ============================================================================
// AgendaTask
// ============================================================================

struct AgendaTask {
    std::string              id;
    std::string              goal;
    std::string              plan;
    std::vector<AgendaTaskStep> steps;
    AgendaTaskStatus         status = AgendaTaskStatus::IN_PROGRESS;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;

    AgendaTask() = default;
    explicit AgendaTask(const std::string& goal) : goal(goal) {}

    bool isComplete()   const { return status == AgendaTaskStatus::COMPLETE; }
    bool isCancelled()  const { return status == AgendaTaskStatus::CANCELLED; }
    bool isInProgress() const { return status == AgendaTaskStatus::IN_PROGRESS; }
};

// ============================================================================
// AgentAgenda
// ============================================================================

class AgentAgenda {
public:
    AgentAgenda();
    ~AgentAgenda();

    // Task creation
    AgendaTask createTask(const std::string& goal,
                          const std::string& plan  = "",
                          const std::vector<AgendaTaskStep>& steps = {});

    // Task queries
    std::vector<AgendaTask> listTasks(AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    std::vector<AgendaTask> searchTasks(const std::string& search_term,
                                         AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    AgendaTask getTaskById(const std::string& task_id);

    // Task lifecycle
    bool deleteTask(const std::string& task_id);
    bool finishTask(const std::string& task_id);
    bool cancelTask(const std::string& task_id);

    // Current task
    AgendaTask getCurrentTask();
    bool       setCurrentTask(const std::string& task_id);

    // Recent queries
    AgendaTask getLastCreatedTask();
    AgendaTask getLastUpdatedTask();

    // Plan management
    std::string createPlan(const std::string& goal);
    bool        updatePlan(const std::string& task_id, const std::string& plan);

    // Step management
    std::vector<AgendaTaskStep> createSteps(const std::string& goal, const std::string& plan);
    bool addStep(const std::string& task_id, const std::string& step_content);
    bool finishStep(const std::string& task_id, const std::string& step_content);
    bool cancelStep(const std::string& task_id, const std::string& step_content);
    bool updateStep(const std::string& task_id, const std::string& old_step,
                    const AgendaTaskStep& new_step);

private:
    std::string generateSimpleUUID();

    std::string statusToString(AgendaTaskStatus status) const;
    AgendaTaskStatus stringToStatus(const std::string& status_str) const;
    std::string timestampToString(const std::chrono::system_clock::time_point& timepoint) const;
    std::chrono::system_clock::time_point stringToTimestamp(const std::string& timestamp_str) const;
    std::string serializeSteps(const std::vector<AgendaTaskStep>& steps) const;
    std::vector<AgendaTaskStep> deserializeSteps(const std::string& steps_json) const;

    void       saveTaskToMemory(const AgendaTask& task);
    AgendaTask loadTaskFromMemory(const std::shared_ptr<Memory>& memory_obj) const;

    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger>        logger_;
    mutable std::mutex                  agendaMutex_;
};

} // namespace elizaos
