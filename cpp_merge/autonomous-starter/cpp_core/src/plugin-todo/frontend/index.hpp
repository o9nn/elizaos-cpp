#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    std::optional<std:> dueDate;
    std::optional<double> streak;
    std::optional<bool> completedToday;
    std::optional<std:> lastReminderSent;
    std::optional<double> pointsAwarded;
    std::optional<std:> completedAt;
};

struct Task {
    std: id;
    std: name;
    std::optional<std:> description;
    std::optional<std::vector<std::string>> tags;
    std::optional<TaskMetadata> metadata;
    std: roomId;
};

struct RoomWithTasks {
    std: roomId;
    std: roomName;
    std::vector<Task> tasks;
};

struct WorldWithRooms {
    std: worldId;
    std: worldName;
    std::vector<RoomWithTasks> rooms;
};

// --- NEW: Interface for Task Identifiers ---
struct TaskIdentifier {
    std: id;
    std: name;
    std::optional<std:> entityId;
    std::optional<std:> roomId;
    std::optional<std:> worldId;
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
