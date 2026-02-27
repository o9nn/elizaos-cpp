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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Added import
 // Import useQueryClient
 // Added Users icon for groups and Hash for channels

 // Added useNavigate

/* ---------- helpers ---------- */

/* ---------- tiny components ---------- */

// Updated RoomListSection to GroupChannelListSection

// Updated CreateButton: Removed DropdownMenu, simplified to a single action (Create Agent)
// For "Create Group", users will use the button in the "Groups" section header.

struct AppSidebarProps {
};

/**
 * Renders the main application sidebar, displaying navigation, agent lists, group rooms, and utility links.
 *
 * The sidebar includes sections for online and offline agents, group rooms, a create button for agents and groups, and footer links to documentation, logs, and settings. It handles loading and error states for agent and room data, and conditionally displays a group creation panel.
 */
void AppSidebar(auto isMobile);

/* ---------- footer link ---------- */


} // namespace elizaos
