#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <any>
#include "elizaos/core.hpp"
#include "elizaos/agentmemory.hpp"
#include "elizaos/agentlogger.hpp"

namespace elizaos {

/**
 * @brief Simple JSON-like container using std::any
 */
using JsonValue = std::unordered_map<std::string, std::any>;

/**
 * @brief Agenda task status enumeration (different from core TaskStatus)
 */
enum class AgendaTaskStatus {
    IN_PROGRESS,
    COMPLETE,
    CANCELLED
};

/**
 * @brief Represents a step within an agenda task
 */
struct AgendaTaskStep {
    std::string content;
    bool completed = false;
    
    AgendaTaskStep() = default;
    AgendaTaskStep(const std::string& c, bool comp = false) : content(c), completed(comp) {}
};

/**
 * @brief Represents a task in the agenda system
 */
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
    
    AgendaTask(const std::string& goal_text) 
        : goal(goal_text), 
          created_at(std::chrono::system_clock::now()),
          updated_at(std::chrono::system_clock::now()) {}
};

/**
 * @brief Agent agenda system for task scheduling and management
 */
class AgentAgenda {
public:
    AgentAgenda();
    ~AgentAgenda();
    
    /**
     * @brief Create a new task
     * @param goal The goal of the task
     * @param plan Optional plan for the task (will be generated if not provided)
     * @param steps Optional steps for the task (will be generated if not provided)
     * @return The created task
     */
    AgendaTask createTask(const std::string& goal, const std::string& plan = "", const std::vector<AgendaTaskStep>& steps = {});
    
    /**
     * @brief List all tasks with specified status
     * @param status The status to filter by
     * @return Vector of tasks with the specified status
     */
    std::vector<AgendaTask> listTasks(AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    
    /**
     * @brief Search for tasks by search term
     * @param search_term The term to search for
     * @param status Optional status filter
     * @return Vector of matching tasks
     */
    std::vector<AgendaTask> searchTasks(const std::string& search_term, AgendaTaskStatus status = AgendaTaskStatus::IN_PROGRESS);
    
    /**
     * @brief Get task by ID
     * @param task_id The ID of the task
     * @return The task if found, empty AgendaTask otherwise
     */
    AgendaTask getTaskById(const std::string& task_id);
    
    /**
     * @brief Delete a task
     * @param task_id The ID of the task to delete
     * @return True if task was deleted, false otherwise
     */
    bool deleteTask(const std::string& task_id);
    
    /**
     * @brief Mark a task as complete
     * @param task_id The ID of the task to finish
     * @return True if task was updated, false otherwise
     */
    bool finishTask(const std::string& task_id);
    
    /**
     * @brief Cancel a task
     * @param task_id The ID of the task to cancel
     * @return True if task was updated, false otherwise
     */
    bool cancelTask(const std::string& task_id);
    
    /**
     * @brief Get the current task
     * @return The current task, empty AgendaTask if none
     */
    AgendaTask getCurrentTask();
    
    /**
     * @brief Set a task as the current task
     * @param task_id The ID of the task to set as current
     * @return True if task was set as current, false otherwise
     */
    bool setCurrentTask(const std::string& task_id);
    
    /**
     * @brief Get the last created task
     * @return The most recently created task
     */
    AgendaTask getLastCreatedTask();
    
    /**
     * @brief Get the last updated task
     * @return The most recently updated task
     */
    AgendaTask getLastUpdatedTask();
    
    /**
     * @brief Create a plan for a given goal
     * @param goal The goal to create a plan for
     * @return Generated plan string
     */
    std::string createPlan(const std::string& goal);
    
    /**
     * @brief Update the plan of a task
     * @param task_id The ID of the task
     * @param plan The new plan
     * @return True if plan was updated, false otherwise
     */
    bool updatePlan(const std::string& task_id, const std::string& plan);

    /**
     * @brief Re-plan an existing task based on current progress
     *
     * Analyzes the current state of a task (completed steps, remaining steps,
     * and the original goal) and generates a new plan that accounts for what
     * has already been done. Backported from hurdcog fork for cross-fork parity.
     *
     * @param task_id The ID of the task to re-plan
     * @param context Optional additional context for re-planning
     * @param regenerate_steps If true, also regenerates the steps based on the new plan
     * @return The new plan string, or empty string if task not found
     */
    std::string planAgain(const std::string& task_id,
                          const std::string& context = "",
                          bool regenerate_steps = false);

    /**
     * @brief Create steps for a goal and plan
     * @param goal The goal
     * @param plan The plan
     * @return Vector of generated steps
     */
    std::vector<AgendaTaskStep> createSteps(const std::string& goal, const std::string& plan);
    
    /**
     * @brief Add a step to a task
     * @param task_id The ID of the task
     * @param step_content The content of the step to add
     * @return True if step was added, false otherwise
     */
    bool addStep(const std::string& task_id, const std::string& step_content);
    
    /**
     * @brief Mark a step as finished
     * @param task_id The ID of the task
     * @param step_content The content of the step to finish
     * @return True if step was marked as finished, false otherwise
     */
    bool finishStep(const std::string& task_id, const std::string& step_content);
    
    /**
     * @brief Cancel a step
     * @param task_id The ID of the task
     * @param step_content The content of the step to cancel
     * @return True if step was cancelled (removed), false otherwise
     */
    bool cancelStep(const std::string& task_id, const std::string& step_content);
    
    /**
     * @brief Update a step in a task
     * @param task_id The ID of the task
     * @param old_step The old step content
     * @param new_step The new step
     * @return True if step was updated, false otherwise
     */
    bool updateStep(const std::string& task_id, const std::string& old_step, const AgendaTaskStep& new_step);
    
    /**
     * @brief Get task as formatted string
     * @param task The task to format
     * @param include_plan Include plan in output
     * @param include_current_step Include current step in output
     * @param include_status Include status in output
     * @param include_steps Include all steps in output
     * @return Formatted string representation of the task
     */
    std::string getTaskAsFormattedString(const AgendaTask& task, bool include_plan = true, 
                                       bool include_current_step = true, bool include_status = true, 
                                       bool include_steps = true);
    
    /**
     * @brief List all tasks as a formatted string
     * @return Formatted string containing all current tasks
     */
    std::string listTasksAsFormattedString();
    
    /**
     * @brief Clear all tasks (for testing)
     */
    void clearTasks();

private:
    std::shared_ptr<AgentMemoryManager> memory_;
    std::shared_ptr<AgentLogger> logger_;
    
    /**
     * @brief Convert task status to string
     * @param status The status to convert
     * @return String representation of the status
     */
    std::string statusToString(AgendaTaskStatus status);
    
    /**
     * @brief Convert string to task status
     * @param status_str The status string
     * @return AgendaTaskStatus enum value
     */
    AgendaTaskStatus stringToStatus(const std::string& status_str);
    
    /**
     * @brief Save task to memory
     * @param task The task to save
     */
    void saveTaskToMemory(const AgendaTask& task);
    
    /**
     * @brief Load task from memory
     * @param memory_obj The memory object containing task data
     * @return AgendaTask object
     */
    AgendaTask loadTaskFromMemory(const std::shared_ptr<Memory>& memory_obj);
    
    /**
     * @brief Generate simple UUID
     * @return UUID string
     */
    std::string generateSimpleUUID();
    
    /**
     * @brief Serialize steps to JSON string
     * @param steps The steps to serialize
     * @return JSON string representation
     */
    std::string serializeSteps(const std::vector<AgendaTaskStep>& steps);
    
    /**
     * @brief Deserialize steps from JSON string
     * @param steps_json The JSON string
     * @return Vector of AgendaTaskStep objects
     */
    std::vector<AgendaTaskStep> deserializeSteps(const std::string& steps_json);
    
    /**
     * @brief Convert timestamp to string
     * @param timepoint The timepoint to convert
     * @return String representation of timestamp
     */
    std::string timestampToString(const std::chrono::system_clock::time_point& timepoint);
    
    /**
     * @brief Convert string to timestamp
     * @param timestamp_str The timestamp string
     * @return system_clock time_point
     */
    std::chrono::system_clock::time_point stringToTimestamp(const std::string& timestamp_str);
};

} // namespace elizaos