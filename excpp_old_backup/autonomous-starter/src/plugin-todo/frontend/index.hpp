#include "loader.hpp"
#include "ui/button.hpp"
#include "ui/card.hpp"
#include "ui/checkbox.hpp"
#include "ui/collapsible.hpp"
#include "ui/input.hpp"
#include "ui/label.hpp"
#include "ui/select.hpp"
#include "ui/separator.hpp"
#include "utils.hpp"
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



// Define Task type based on backend structure
// NOTE: Adjust this type based on the actual structure returned by IAgentRuntime and modified by API routes
struct TaskMetadata {
    std::optional<string; // ISO string> dueDate;
    std::optional<double> streak;
    std::optional<bool> completedToday;
    std::optional<string; // ISO string> lastReminderSent;
    std::optional<double> pointsAwarded;
    std::optional<string; // ISO string> completedAt;
};

struct Task {
    std::string id;
    std::string name;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> tags;
    std::optional<TaskMetadata> metadata;
    string; // Added roomId as it's crucial roomId;
};

struct RoomWithTasks {
    std::string roomId;
    std::string roomName;
    std::vector<Task> tasks;
};

struct WorldWithRooms {
    std::string worldId;
    std::string worldName;
    std::vector<RoomWithTasks> rooms;
};

// --- NEW: Interface for Task Identifiers ---
struct TaskIdentifier {
    std::string id;
    std::string name;
    std::optional<string; // Make optional if they can be null/undefined> entityId;
    std::optional<std::string> roomId;
    std::optional<std::string> worldId;
};

// Helper to extract context from URL

// --- API Interaction Hooks ---

// --- Hook to fetch tags ---

// --- Hook to fetch ALL tasks (for debugging) ---

// --- Components ---

// --- Component to display ALL tasks --- MODIFY THIS COMPONENT

// --- END ALL TASKS COMPONENT ---

void App();


} // namespace elizaos
