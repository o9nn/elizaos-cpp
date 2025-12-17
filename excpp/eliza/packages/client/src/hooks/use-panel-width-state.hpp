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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Keys for storing panel width state in localStorage

// Default panel sizes

/**
 * Custom hook to manage panel width state with localStorage persistence
 * Includes resize detection for automatic floating mode switching
 */
void usePanelWidthState();

} // namespace elizaos
