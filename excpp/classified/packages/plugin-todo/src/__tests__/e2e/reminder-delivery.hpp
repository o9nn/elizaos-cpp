#include "..scripts/demo-reminders.hpp"
#include "elizaos/core.hpp"
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



// Type for the MESSAGE_DELIVERY service from rolodex plugin
struct MessageDeliveryService {
    std::optional<(message: unknown) => Promise<void>> sendMessage;
};

        // Check if rolodex is available

        // Set up demo scenarios

        // Monitor for 30 seconds to see reminders being sent

        await monitorReminders(runtime, 30000);

        // Clean up
        await cleanupDemo(runtime, testUserId);

        // Create test data

        // Create daily tasks

        // Check current time

          // Trigger reminder check

        // Clean up

        // Create an overdue task

        // Get reminder service

        // Trigger multiple reminder checks

        // Clean up

        // Create tasks with different priorities

        // Trigger reminder check

        // Clean up


} // namespace elizaos
