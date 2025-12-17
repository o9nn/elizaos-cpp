#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define Task type based on backend structure
// NOTE: Adjust this type based on the actual structure returned by IAgentRuntime and modified by API routes
struct TaskMetadata {
    std::optional<std::string> dueDate;
    std::optional<double> streak;
    std::optional<bool> completedToday;
    std::optional<std::string> lastReminderSent;
    std::optional<double> pointsAwarded;
    std::optional<std::string> completedAt;
};

struct Task {
    std::string id;
    std::string name;
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> tags;
    std::optional<TaskMetadata> metadata;
    std::string roomId;
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
    std::optional<std::string> entityId;
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
