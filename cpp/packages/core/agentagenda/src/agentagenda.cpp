#include "elizaos/agentagenda.hpp"
#include <sstream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <variant>

namespace elizaos {

// Simple UUID generator
std::string AgentAgenda::generateSimpleUUID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    for (auto& c : uuid) {
        if (c == 'x') {
            c = "0123456789abcdef"[dis(gen)];
        } else if (c == 'y') {
            c = "89ab"[dis(gen) % 4];
        }
    }
    return uuid;
}

AgentAgenda::AgentAgenda() {
    memory_ = std::make_shared<AgentMemoryManager>();
    logger_ = std::make_shared<AgentLogger>();
}

AgentAgenda::~AgentAgenda() = default;

std::string AgentAgenda::statusToString(AgendaTaskStatus status) {
    switch (status) {
        case AgendaTaskStatus::IN_PROGRESS: return "in_progress";
        case AgendaTaskStatus::COMPLETE: return "complete";
        case AgendaTaskStatus::CANCELLED: return "cancelled";
        default: return "in_progress";
    }
}

AgendaTaskStatus AgentAgenda::stringToStatus(const std::string& status_str) {
    if (status_str == "complete") return AgendaTaskStatus::COMPLETE;
    if (status_str == "cancelled") return AgendaTaskStatus::CANCELLED;
    return AgendaTaskStatus::IN_PROGRESS;
}

std::string AgentAgenda::timestampToString(const std::chrono::system_clock::time_point& timepoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timepoint);
    std::stringstream ss;
    ss << time_t;
    return ss.str();
}

std::chrono::system_clock::time_point AgentAgenda::stringToTimestamp(const std::string& timestamp_str) {
    std::time_t time_t;
    std::istringstream ss(timestamp_str);
    ss >> time_t;
    return std::chrono::system_clock::from_time_t(time_t);
}

std::string AgentAgenda::serializeSteps(const std::vector<AgendaTaskStep>& steps) {
    std::stringstream json;
    json << "[";
    for (size_t i = 0; i < steps.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"content\":\"" << steps[i].content << "\",\"completed\":" 
             << (steps[i].completed ? "true" : "false") << "}";
    }
    json << "]";
    return json.str();
}

std::vector<AgendaTaskStep> AgentAgenda::deserializeSteps(const std::string& steps_json) {
    // Simple JSON parsing - in a real implementation, use a proper JSON library
    std::vector<AgendaTaskStep> steps;
    
    // Very basic parsing for our simple JSON format
    size_t pos = 0;
    while ((pos = steps_json.find("\"content\":\"", pos)) != std::string::npos) {
        pos += 11; // length of "content":"
        size_t end_pos = steps_json.find("\"", pos);
        if (end_pos != std::string::npos) {
            std::string content = steps_json.substr(pos, end_pos - pos);
            
            // Find completed status
            size_t comp_pos = steps_json.find("\"completed\":", end_pos);
            bool completed = false;
            if (comp_pos != std::string::npos) {
                comp_pos += 12; // length of "completed":
                completed = (steps_json.substr(comp_pos, 4) == "true");
            }
            
            steps.emplace_back(content, completed);
            pos = end_pos;
        } else {
            break;
        }
    }
    
    return steps;
}

void AgentAgenda::saveTaskToMemory(const AgendaTask& task) {
    UUID memoryId = task.id;
    UUID entityId = generateSimpleUUID();
    UUID agentId = generateSimpleUUID();
    
    std::string content = task.goal;
    
    // Create CustomMetadata for the task
    CustomMetadata customMeta;
    customMeta.customData["id"] = task.id;
    customMeta.customData["goal"] = task.goal;
    customMeta.customData["plan"] = task.plan;
    customMeta.customData["steps"] = serializeSteps(task.steps);
    customMeta.customData["status"] = statusToString(task.status);
    customMeta.customData["created_at"] = timestampToString(task.created_at);
    customMeta.customData["updated_at"] = timestampToString(task.updated_at);
    customMeta.customData["current"] = task.current ? "true" : "false";
    
    MemoryMetadata metadata = customMeta;
    
    auto memory = std::make_shared<Memory>(memoryId, content, entityId, agentId, metadata);
    memory_->createMemory(memory, "task");
}

AgendaTask AgentAgenda::loadTaskFromMemory(const std::shared_ptr<Memory>& memory_obj) {
    AgendaTask task;
    
    if (std::holds_alternative<CustomMetadata>(memory_obj->getMetadata())) {
        const auto& customMeta = std::get<CustomMetadata>(memory_obj->getMetadata());
        
        auto getValue = [&](const std::string& key) -> std::string {
            auto it = customMeta.customData.find(key);
            return it != customMeta.customData.end() ? it->second : "";
        };
        
        task.id = getValue("id");
        task.goal = getValue("goal");
        task.plan = getValue("plan");
        task.steps = deserializeSteps(getValue("steps"));
        task.status = stringToStatus(getValue("status"));
        task.created_at = stringToTimestamp(getValue("created_at"));
        task.updated_at = stringToTimestamp(getValue("updated_at"));
        task.current = (getValue("current") == "true");
    }
    
    return task;
}

AgendaTask AgentAgenda::createTask(const std::string& goal, const std::string& plan, const std::vector<AgendaTaskStep>& steps) {
    AgendaTask task(goal);
    task.id = generateSimpleUUID();
    
    // Set plan
    if (plan.empty()) {
        task.plan = createPlan(goal);
    } else {
        task.plan = plan;
    }
    
    // Set steps
    if (steps.empty()) {
        task.steps = createSteps(goal, task.plan);
    } else {
        task.steps = steps;
    }
    
    // Set this task as current (unset previous current task)
    auto current_tasks = listTasks(AgendaTaskStatus::IN_PROGRESS);
    for (const auto& existing_task : current_tasks) {
        if (existing_task.current) {
            AgendaTask updated_task = existing_task;
            updated_task.current = false;
            updated_task.updated_at = std::chrono::system_clock::now();
            saveTaskToMemory(updated_task);
        }
    }
    
    task.current = true;
    
    saveTaskToMemory(task);
    logger_->log("Created task: " + goal, "info");
    
    return task;
}

std::vector<AgendaTask> AgentAgenda::listTasks(AgendaTaskStatus status) {
    std::vector<AgendaTask> tasks;
    
    MemorySearchParams params;
    params.tableName = "task";
    params.count = 100; // Get all tasks
    
    auto memories = memory_->getMemories(params);
    
    for (const auto& memory : memories) {
        AgendaTask task = loadTaskFromMemory(memory);
        if (task.status == status) {
            tasks.push_back(task);
        }
    }
    
    return tasks;
}

std::vector<AgendaTask> AgentAgenda::searchTasks(const std::string& search_term, AgendaTaskStatus status) {
    std::vector<AgendaTask> tasks;
    
    MemorySearchParams params;
    params.tableName = "task";
    params.count = 100;
    
    auto memories = memory_->getMemories(params);
    
    // Filter by search term and status
    for (const auto& memory : memories) {
        if (memory->getContent().find(search_term) != std::string::npos) {
            AgendaTask task = loadTaskFromMemory(memory);
            if (task.status == status) {
                tasks.push_back(task);
            }
        }
    }
    
    return tasks;
}

AgendaTask AgentAgenda::getTaskById(const std::string& task_id) {
    MemorySearchParams params;
    params.tableName = "task";
    params.count = 100;
    
    auto memories = memory_->getMemories(params);
    
    for (const auto& memory : memories) {
        AgendaTask task = loadTaskFromMemory(memory);
        if (task.id == task_id) {
            return task;
        }
    }
    
    return AgendaTask(); // Return empty task if not found
}

bool AgentAgenda::deleteTask(const std::string& task_id) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    // Find and delete the memory
    auto memory_id_uuid = UUID(task_id);
    return memory_->deleteMemory(memory_id_uuid);
}

bool AgentAgenda::finishTask(const std::string& task_id) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    task.status = AgendaTaskStatus::COMPLETE;
    task.updated_at = std::chrono::system_clock::now();
    task.current = false; // Completed tasks are not current
    
    saveTaskToMemory(task);
    logger_->log("Finished task: " + task.goal, "info");
    
    return true;
}

bool AgentAgenda::cancelTask(const std::string& task_id) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    task.status = AgendaTaskStatus::CANCELLED;
    task.updated_at = std::chrono::system_clock::now();
    task.current = false; // Cancelled tasks are not current
    
    saveTaskToMemory(task);
    logger_->log("Cancelled task: " + task.goal, "info");
    
    return true;
}

AgendaTask AgentAgenda::getCurrentTask() {
    auto tasks = listTasks(AgendaTaskStatus::IN_PROGRESS);
    
    for (const auto& task : tasks) {
        if (task.current) {
            return task;
        }
    }
    
    return AgendaTask(); // Return empty task if no current task
}

bool AgentAgenda::setCurrentTask(const std::string& task_id) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    // Unset current task
    auto current_task = getCurrentTask();
    if (!current_task.id.empty()) {
        current_task.current = false;
        current_task.updated_at = std::chrono::system_clock::now();
        saveTaskToMemory(current_task);
    }
    
    // Set new current task
    task.current = true;
    task.updated_at = std::chrono::system_clock::now();
    saveTaskToMemory(task);
    
    return true;
}

AgendaTask AgentAgenda::getLastCreatedTask() {
    auto tasks = listTasks(AgendaTaskStatus::IN_PROGRESS);
    
    if (tasks.empty()) {
        return AgendaTask();
    }
    
    auto latest_task = std::max_element(tasks.begin(), tasks.end(),
        [](const AgendaTask& a, const AgendaTask& b) {
            return a.created_at < b.created_at;
        });
    
    return *latest_task;
}

AgendaTask AgentAgenda::getLastUpdatedTask() {
    auto tasks = listTasks(AgendaTaskStatus::IN_PROGRESS);
    
    if (tasks.empty()) {
        return AgendaTask();
    }
    
    auto latest_task = std::max_element(tasks.begin(), tasks.end(),
        [](const AgendaTask& a, const AgendaTask& b) {
            return a.updated_at < b.updated_at;
        });
    
    return *latest_task;
}

std::string AgentAgenda::createPlan(const std::string& goal) {
    // Simplified plan generation - in a real implementation, this would use LLM
    return "Step-by-step plan to achieve: " + goal + ". This includes gathering resources, executing actions, and validating completion.";
}

bool AgentAgenda::updatePlan(const std::string& task_id, const std::string& plan) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    task.plan = plan;
    task.updated_at = std::chrono::system_clock::now();
    
    saveTaskToMemory(task);
    return true;
}

std::vector<AgendaTaskStep> AgentAgenda::createSteps(const std::string& goal, const std::string& plan) {
    // Simplified step generation - in a real implementation, this would use LLM
    std::vector<AgendaTaskStep> steps;
    
    steps.emplace_back("Analyze the goal: " + goal, false);
    steps.emplace_back("Execute the plan: " + plan.substr(0, 50) + "...", false);
    steps.emplace_back("Validate completion of: " + goal, false);
    
    return steps;
}

bool AgentAgenda::addStep(const std::string& task_id, const std::string& step_content) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    task.steps.emplace_back(step_content, false);
    task.updated_at = std::chrono::system_clock::now();
    
    saveTaskToMemory(task);
    return true;
}

bool AgentAgenda::finishStep(const std::string& task_id, const std::string& step_content) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    for (auto& step : task.steps) {
        if (step.content == step_content) {
            step.completed = true;
            task.updated_at = std::chrono::system_clock::now();
            saveTaskToMemory(task);
            return true;
        }
    }
    
    return false; // Step not found
}

bool AgentAgenda::cancelStep(const std::string& task_id, const std::string& step_content) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    auto it = std::remove_if(task.steps.begin(), task.steps.end(),
        [&step_content](const AgendaTaskStep& step) {
            return step.content == step_content;
        });
    
    if (it != task.steps.end()) {
        task.steps.erase(it, task.steps.end());
        task.updated_at = std::chrono::system_clock::now();
        saveTaskToMemory(task);
        return true;
    }
    
    return false; // Step not found
}

bool AgentAgenda::updateStep(const std::string& task_id, const std::string& old_step, const AgendaTaskStep& new_step) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return false;
    }
    
    for (auto& step : task.steps) {
        if (step.content == old_step) {
            step = new_step;
            task.updated_at = std::chrono::system_clock::now();
            saveTaskToMemory(task);
            return true;
        }
    }
    
    return false; // Step not found
}

std::string AgentAgenda::getTaskAsFormattedString(const AgendaTask& task, bool include_plan, 
                                                bool include_current_step, bool include_status, 
                                                bool include_steps) {
    std::stringstream formatted;
    
    formatted << "Task: " << task.goal << "\n";
    formatted << "ID: " << task.id << "\n";
    
    if (include_status) {
        formatted << "Status: " << statusToString(task.status);
        if (task.current) {
            formatted << " (CURRENT)";
        }
        formatted << "\n";
    }
    
    if (include_plan && !task.plan.empty()) {
        formatted << "Plan: " << task.plan << "\n";
    }
    
    if (include_steps && !task.steps.empty()) {
        formatted << "Steps:\n";
        for (size_t i = 0; i < task.steps.size(); ++i) {
            formatted << "  " << (i + 1) << ". ";
            if (task.steps[i].completed) {
                formatted << "[DONE] ";
            } else {
                formatted << "[TODO] ";
            }
            formatted << task.steps[i].content << "\n";
        }
    }
    
    if (include_current_step) {
        // Find first uncompleted step
        for (const auto& step : task.steps) {
            if (!step.completed) {
                formatted << "Current Step: " << step.content << "\n";
                break;
            }
        }
    }
    
    return formatted.str();
}

std::string AgentAgenda::listTasksAsFormattedString() {
    auto tasks = listTasks(AgendaTaskStatus::IN_PROGRESS);
    
    if (tasks.empty()) {
        return "No tasks in progress.\n";
    }
    
    std::stringstream formatted;
    formatted << "Current Tasks (" << tasks.size() << "):\n\n";
    
    for (const auto& task : tasks) {
        formatted << getTaskAsFormattedString(task, true, true, true, false);
        formatted << "---\n";
    }
    
    return formatted.str();
}

void AgentAgenda::clearTasks() {
    memory_->clear();
}

} // namespace elizaos