#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Key for storing sidebar visibility state in localStorage

/**
 * Custom hook to manage agent sidebar visibility state with localStorage persistence
 * Remembers whether the sidebar should be open or closed across page refreshes
 */
void useSidebarState();

} // namespace elizaos
