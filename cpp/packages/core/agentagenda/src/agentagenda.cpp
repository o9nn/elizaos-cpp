#include "elizaos/agentagenda.hpp"
#include <sstream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <variant>
#include <cctype>
#include <ctime>

namespace elizaos {

namespace {

// Constants for display formatting
constexpr size_t MAX_COMPLETED_STEP_DISPLAY_LENGTH = 60;
constexpr size_t MAX_REMAINING_STEP_DISPLAY_LENGTH = 55;
constexpr size_t MAX_CONTEXT_STEP_LENGTH = 50;
constexpr size_t MAX_REMAINING_STEPS_TO_SHOW = 3;

std::string toLowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

bool containsCaseInsensitive(const std::string& haystack, const std::string& needle) {
    if (needle.empty()) {
        return true;
    }
    return toLowerCopy(haystack).find(toLowerCopy(needle)) != std::string::npos;
}

std::string escapeStepContent(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size());
    for (char ch : value) {
        switch (ch) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += ch; break;
        }
    }
    return escaped;
}

bool parseJsonString(const std::string& text, size_t& pos, std::string& out) {
    if (pos >= text.size() || text[pos] != '"') {
        return false;
    }
    ++pos;
    out.clear();
    while (pos < text.size()) {
        char ch = text[pos++];
        if (ch == '"') {
            return true;
        }
        if (ch == '\\' && pos < text.size()) {
            char esc = text[pos++];
            switch (esc) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default: out += esc; break;
            }
        } else {
            out += ch;
        }
    }
    return false;
}

} // namespace

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
    // Persist the FULL clock resolution (nanoseconds since epoch), not whole
    // seconds. Serializing via to_time_t() truncated every timestamp down to the
    // second, so a save/load round-trip could return a value strictly less than
    // the original in-memory timestamp -- breaking monotonic "updated_at moved
    // forward" invariants for sub-second updates. Storing the raw tick count is
    // lossless and round-trips exactly.
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        timepoint.time_since_epoch())
                        .count();
    std::stringstream ss;
    ss << ns;
    return ss.str();
}

std::chrono::system_clock::time_point AgentAgenda::stringToTimestamp(const std::string& timestamp_str) {
    if (timestamp_str.empty()) {
        return std::chrono::system_clock::now();
    }

    long long parsed = 0;
    std::istringstream ss(timestamp_str);
    ss >> parsed;
    if (!ss || !ss.eof()) {
        // Backward compatibility: legacy records serialized as whole seconds are
        // far smaller than a nanosecond tick count but still parse as an integer,
        // so a parse failure here means genuinely malformed data -- fall back to
        // "now" rather than fabricating an epoch time.
        return std::chrono::system_clock::now();
    }
    return std::chrono::system_clock::time_point(std::chrono::nanoseconds(parsed));
}

std::string AgentAgenda::serializeSteps(const std::vector<AgendaTaskStep>& steps) {
    std::stringstream json;
    json << "[";
    for (size_t i = 0; i < steps.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"content\":\"" << escapeStepContent(steps[i].content) << "\",\"completed\":"
             << (steps[i].completed ? "true" : "false") << "}";
    }
    json << "]";
    return json.str();
}

std::vector<AgendaTaskStep> AgentAgenda::deserializeSteps(const std::string& steps_json) {
    std::vector<AgendaTaskStep> steps;
    size_t pos = 0;

    while ((pos = steps_json.find("\"content\"", pos)) != std::string::npos) {
        size_t colon = steps_json.find(':', pos);
        if (colon == std::string::npos) {
            break;
        }

        size_t value_pos = steps_json.find('\"', colon + 1);
        if (value_pos == std::string::npos) {
            break;
        }

        std::string content;
        if (!parseJsonString(steps_json, value_pos, content)) {
            break;
        }

        bool completed = false;
        size_t comp_pos = steps_json.find("\"completed\"", value_pos);
        if (comp_pos != std::string::npos) {
            size_t comp_colon = steps_json.find(':', comp_pos);
            if (comp_colon != std::string::npos) {
                size_t flag_pos = steps_json.find_first_not_of(" \t\r\n", comp_colon + 1);
                completed = flag_pos != std::string::npos && steps_json.compare(flag_pos, 4, "true") == 0;
            }
        }

        steps.emplace_back(content, completed);
        pos = value_pos;
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
    
    // Filter by search term across user-visible task fields and status.
    for (const auto& memory : memories) {
        AgendaTask task = loadTaskFromMemory(memory);
        bool step_match = std::any_of(task.steps.begin(), task.steps.end(),
            [&search_term](const AgendaTaskStep& step) {
                return containsCaseInsensitive(step.content, search_term);
            });
        if (task.status == status &&
            (containsCaseInsensitive(task.goal, search_term) ||
             containsCaseInsensitive(task.plan, search_term) ||
             step_match)) {
            tasks.push_back(task);
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
    
    if (task.status != AgendaTaskStatus::IN_PROGRESS) {
        return false;
    }

    // Unset current task only after the target has been validated.
    auto current_task = getCurrentTask();
    if (!current_task.id.empty() && current_task.id != task.id) {
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
    MemorySearchParams params;
    params.tableName = "task";
    params.count = 100;

    auto memories = memory_->getMemories(params);
    std::vector<AgendaTask> tasks;
    tasks.reserve(memories.size());
    for (const auto& memory : memories) {
        tasks.push_back(loadTaskFromMemory(memory));
    }

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
    MemorySearchParams params;
    params.tableName = "task";
    params.count = 100;

    auto memories = memory_->getMemories(params);
    std::vector<AgendaTask> tasks;
    tasks.reserve(memories.size());
    for (const auto& memory : memories) {
        tasks.push_back(loadTaskFromMemory(memory));
    }

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
    std::stringstream plan;
    plan << "Clarify the intended outcome for: " << goal << "\n"
         << "Assess required context, dependencies, and constraints.\n"
         << "Execute the smallest verifiable next actions in order.\n"
         << "Validate results against the goal and capture follow-up work.";
    return plan.str();
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

std::string AgentAgenda::planAgain(const std::string& task_id, 
                                    const std::string& context,
                                    bool regenerate_steps) {
    auto task = getTaskById(task_id);
    if (task.id.empty()) {
        return "";
    }
    
    // Analyze current task state
    size_t completed_count = 0;
    size_t total_steps = task.steps.size();
    std::vector<std::string> completed_step_contents;
    std::vector<std::string> remaining_step_contents;
    
    for (const auto& step : task.steps) {
        if (step.completed) {
            completed_count++;
            completed_step_contents.push_back(step.content);
        } else {
            remaining_step_contents.push_back(step.content);
        }
    }
    
    // Build the new plan based on current progress
    std::stringstream new_plan;
    
    // Add context header
    new_plan << "Re-planned for goal: " << task.goal << "\n\n";
    
    // Add progress summary
    if (total_steps > 0) {
        new_plan << "Progress Summary:\n";
        new_plan << "  - Completed: " << completed_count << "/" << total_steps << " steps\n";
        
        if (!completed_step_contents.empty()) {
            new_plan << "  - Already done:\n";
            for (const auto& done : completed_step_contents) {
                new_plan << "    * " << (done.size() > MAX_COMPLETED_STEP_DISPLAY_LENGTH ? 
                    done.substr(0, MAX_COMPLETED_STEP_DISPLAY_LENGTH - 3) + "..." : done) << "\n";
            }
        }
        new_plan << "\n";
    }
    
    // Add re-planning context if provided
    if (!context.empty()) {
        new_plan << "Re-planning context: " << context << "\n\n";
    }
    
    // Generate the new plan strategy
    new_plan << "Revised Strategy:\n";
    
    if (remaining_step_contents.empty()) {
        // All steps completed - suggest verification or extension
        new_plan << "  1. Verify all completed work meets the original goal criteria\n";
        new_plan << "  2. Document any lessons learned or improvements identified\n";
        new_plan << "  3. Consider if additional scope or follow-up tasks are needed\n";
        new_plan << "  4. Mark task as complete or extend with new steps\n";
    } else {
        // Some steps remaining - create adapted plan
        new_plan << "  1. Review remaining work against original goal\n";
        new_plan << "  2. Prioritize remaining steps based on current context\n";
        new_plan << "  3. Execute highest-priority remaining actions:\n";
        
        size_t step_num = 0;
        for (const auto& remaining : remaining_step_contents) {
            if (step_num >= MAX_REMAINING_STEPS_TO_SHOW) {
                new_plan << "       (" << (remaining_step_contents.size() - MAX_REMAINING_STEPS_TO_SHOW) << " more steps...)\n";
                break;
            }
            new_plan << "     - " << (remaining.size() > MAX_REMAINING_STEP_DISPLAY_LENGTH ? 
                remaining.substr(0, MAX_REMAINING_STEP_DISPLAY_LENGTH - 3) + "..." : remaining) << "\n";
            step_num++;
        }
        
        new_plan << "  4. Validate results against the goal: " << task.goal << "\n";
    }
    
    std::string plan_result = new_plan.str();
    
    // Update the task with the new plan
    task.plan = plan_result;
    task.updated_at = std::chrono::system_clock::now();
    
    // Optionally regenerate steps
    if (regenerate_steps) {
        // Keep completed steps, regenerate remaining ones
        std::vector<AgendaTaskStep> new_steps;
        
        // Preserve completed steps
        for (const auto& step : task.steps) {
            if (step.completed) {
                new_steps.push_back(step);
            }
        }
        
        // Add new steps based on revised plan
        if (!context.empty()) {
            new_steps.emplace_back("Address re-planning context: " + 
                (context.size() > MAX_CONTEXT_STEP_LENGTH ? 
                    context.substr(0, MAX_CONTEXT_STEP_LENGTH - 3) + "..." : context), false);
        }
        
        // Add verification step
        new_steps.emplace_back("Verify revised approach aligns with goal: " + task.goal, false);
        
        // Add execution steps
        new_steps.emplace_back("Execute revised plan actions", false);
        
        // Add completion step
        new_steps.emplace_back("Validate final results and document completion", false);
        
        task.steps = new_steps;
    }
    
    saveTaskToMemory(task);
    
    return plan_result;
}

std::vector<AgendaTaskStep> AgentAgenda::createSteps(const std::string& goal, const std::string& plan) {
    std::vector<AgendaTaskStep> steps;
    const std::string plan_summary = plan.size() > 80 ? plan.substr(0, 77) + "..." : plan;

    steps.emplace_back("Define success criteria for: " + goal, false);
    steps.emplace_back("Gather context and dependencies needed for the task", false);
    steps.emplace_back("Execute plan segment: " + plan_summary, false);
    steps.emplace_back("Verify completion and record remaining follow-up work for: " + goal, false);

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