#include "elizaos/core.hpp"
#include "notificationManager.hpp"
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



// Define optional rolodex types for enhanced messaging
struct RolodexService {
    std::optional<(message: {> sendNotification;
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
    number; // How often to check for reminders (ms) reminderCheckInterval;
    number; // Minimum time between reminders for same todo (ms) minReminderInterval;
    number; // How far in advance to send upcoming reminders (ms) upcomingThreshold;
    number[]; // Hours of day to send daily reminders (0-23) dailyReminderHours;
};

/**
 * Main todo reminder service that handles all reminder functionality
 */
class TodoReminderService extends Service {
  static readonly serviceType: ServiceTypeName = 'TODO_REMINDER' as ServiceTypeName;
  static readonly serviceName = 'TODO_REMINDER';
  capabilityDescription = 'Manages todo reminders and notifications';

  // Configuration with sensible defaults
  private reminderConfig: TodoReminderConfig = {
    reminderCheckInterval: 30 * 1000, // 30 seconds
    minReminderInterval: 30 * 60 * 1000, // 30 minutes
    upcomingThreshold: 30 * 60 * 1000, // 30 minutes
    dailyReminderHours: [9, 18], // 9 AM and 6 PM
  };

    // Load configuration from runtime settings with defaults

        // Parse comma-separated string like "9,18"
        // Use array directly if provided

    // Load configuration from runtime settings

    // Initialize internal managers

    // Try to get rolodex service for enhanced entity management (optional)

    // Start reminder checking loop

    // Check for reminders at configured interval

    // Delay initial check to allow database initialization

    // Get overdue todos

    // Process each overdue todo

    // Check for upcoming todos (due within threshold)

    // Check last reminder time to avoid spam

    // Check if overdue
    // Check if upcoming (within configured threshold)
    // Check daily tasks (remind at configured hours)
      // Check if current hour matches any configured reminder hours
        // Check if completed today

      // Always send in-app notification

      // Send message through runtime's messaging system


} // namespace elizaos
