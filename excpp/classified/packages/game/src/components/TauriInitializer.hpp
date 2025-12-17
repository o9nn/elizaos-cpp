#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".services/TauriService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TauriInitializerProps {
    ReactNode children;
};

/**
 * Component that ensures Tauri API is initialized before rendering children
 * This solves the timing issue where TauriService tries to detect Tauri before it's available
 */
void TauriInitializer();

} // namespace elizaos
