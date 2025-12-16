#include ".services/reminderService.hpp"
#include ".services/todoService.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Demo script to test reminder delivery through rolodex
 * This creates various todo scenarios to test the reminder system
 */
std::future<void> setupReminderDemo(IAgentRuntime runtime);

/**
 * Monitor reminder delivery for testing
 */
std::future<void> monitorReminders(IAgentRuntime runtime, number = 60000 duration);

/**
 * Clean up demo tasks
 */
std::future<void> cleanupDemo(IAgentRuntime runtime, UUID testUserId);

// Export for use in tests or direct execution

} // namespace elizaos
