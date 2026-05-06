#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "notificationManager.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define optional rolodex types for enhanced messaging
struct RolodexService {
    UUID entityId;
    std::string message;
    std::string priority;
    std::optional<std::vector<std::string>> platforms;
    std::optional<unknown> metadata;
};

/**
 * Configuration interface for TodoReminderService
 */
struct TodoReminderConfig {
    double reminderCheckInterval;
    double minReminderInterval;
    double upcomingThreshold;
    std::vector<double> dailyReminderHours;
};

/**
 * Main todo reminder service that handles all reminder functionality
 */
class TodoReminderService {
public:
    std::future<TodoReminderService> start(IAgentRuntime runtime);
    void loadConfiguration();
    std::future<void> initialize();
    void startReminderLoop();
    std::future<void> checkTasksForReminders();
    std::future<void> processTodoReminder(TodoData todo);
    std::future<void> sendReminder(TodoData todo, const std::variant<'overdue', 'upcoming', 'daily', 'system'>& reminderType, const std::variant<'low', 'medium', 'high'>& priority);
    std::string formatReminderTitle(TodoData todo, const std::string& reminderType);
    std::string formatReminderBody(TodoData todo, const std::string& reminderType);
    std::future<void> processBatchReminders();
    std::future<void> stop();
    std::future<void> stop(IAgentRuntime runtime);
};


} // namespace elizaos
