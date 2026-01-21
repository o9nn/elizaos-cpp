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
#include "hooks/use-query-hooks.hpp"
#include "stop-agent-button.hpp"
#include "ui/button.hpp"
#include "ui/card.hpp"
#include "ui/tooltip.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ProfileOverlayProps {
    bool isOpen;
    UUID agentId;
};

/**
 * Displays a modal overlay with detailed information and controls for a specific agent.
 *
 * Renders an interactive profile card for the agent identified by {@link agentId}, including avatar, status, metadata, capabilities, plugins, and action buttons for starting, stopping, messaging, and navigating to settings. The overlay is visible when {@link isOpen} is true and can be closed via the provided callback.
 *
 * @param isOpen - Whether the overlay is visible.
 * @param onClose - Callback invoked to close the overlay.
 * @param agentId - UUID of the agent whose profile is displayed.
 *
 * @returns The profile overlay component, or null if not open.
 */

} // namespace elizaos
