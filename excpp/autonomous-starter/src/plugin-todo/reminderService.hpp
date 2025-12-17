#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * The Todo Reminder Service checks for overdue tasks and sends reminders to users.
 */
class TodoReminderService {
public:
    static std::future<TodoReminderService> start(IAgentRuntime runtime);
    void startTimer();
    void checkOverdueTasks();
    void sendReminder(Task task);
    void checkTasksNow();
    void stop();
};


} // namespace elizaos
